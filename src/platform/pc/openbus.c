// Emulates GBA reads and writes through NULL (or nearly NULL) pointers.
//
// The game sometimes reads members of a NULL pointer. On the GBA that reads the
// BIOS area, which returns "open bus" (the last BIOS opcode fetched) instead of
// crashing; writes there are ignored. The PC cannot map memory at address 0, so
// a SIGSEGV handler decodes the faulting x86 instruction, performs it with the
// open bus value (or drops the write) and resumes after it. On Windows, a
// vectored exception handler does the same.
//
// Only the instruction forms GCC emits for loads, stores, compares and simple
// arithmetic on memory are handled. Anything else still crashes, with the
// instruction bytes printed so the decoder can be extended.
// TMC_LOG_OPENBUS=1 logs each site the first time it is hit.

#define _GNU_SOURCE // REG_* register names in ucontext.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"

#if (defined(__linux__) || defined(_WIN32)) && (defined(__i386__) || defined(__x86_64__))
#include <stddef.h>
#include <stdint.h>

// Value of the GBA BIOS area after returning from a BIOS call (open bus).
#define BIOS_OPEN_BUS 0xE3A02004u
// Neither Linux (vm.mmap_min_addr) nor Windows maps below this address.
#define LOW_MEMORY_END 0x10000u

#define EFLAGS_CF (1u << 0)
#define EFLAGS_PF (1u << 2)
#define EFLAGS_ZF (1u << 6)
#define EFLAGS_SF (1u << 7)
#define EFLAGS_OF (1u << 11)
#define EFLAGS_ARITH (EFLAGS_CF | EFLAGS_PF | (1u << 4) | EFLAGS_ZF | EFLAGS_SF | EFLAGS_OF)

// The faulting thread's registers: Machine is the OS's register context,
// Word a general register in it. Reg(mc, r) takes the x86 register number.
#ifdef _WIN32
#include <windows.h>

typedef CONTEXT Machine;
typedef DWORD_PTR Word;
#define FLAGS(mc) ((mc)->EFlags)
#ifdef __x86_64__
#define IP(mc) ((mc)->Rip)
#define NUM_REGS 16
static const size_t sRegs[NUM_REGS] = {
    offsetof(CONTEXT, Rax), offsetof(CONTEXT, Rcx), offsetof(CONTEXT, Rdx), offsetof(CONTEXT, Rbx),
    offsetof(CONTEXT, Rsp), offsetof(CONTEXT, Rbp), offsetof(CONTEXT, Rsi), offsetof(CONTEXT, Rdi),
    offsetof(CONTEXT, R8),  offsetof(CONTEXT, R9),  offsetof(CONTEXT, R10), offsetof(CONTEXT, R11),
    offsetof(CONTEXT, R12), offsetof(CONTEXT, R13), offsetof(CONTEXT, R14), offsetof(CONTEXT, R15),
};
#else
#define IP(mc) ((mc)->Eip)
#define NUM_REGS 8
static const size_t sRegs[NUM_REGS] = {
    offsetof(CONTEXT, Eax), offsetof(CONTEXT, Ecx), offsetof(CONTEXT, Edx), offsetof(CONTEXT, Ebx),
    offsetof(CONTEXT, Esp), offsetof(CONTEXT, Ebp), offsetof(CONTEXT, Esi), offsetof(CONTEXT, Edi),
};
#endif

static Word* Reg(Machine* mc, int r) {
    return (Word*)((char*)mc + sRegs[r]);
}
#else
#include <signal.h>
#include <ucontext.h>

typedef mcontext_t Machine;
typedef greg_t Word;
#define FLAGS(mc) ((mc)->gregs[REG_EFL])
#ifdef __x86_64__
#define IP(mc) ((mc)->gregs[REG_RIP])
#define NUM_REGS 16
static const int sRegs[NUM_REGS] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RBX, REG_RSP, REG_RBP, REG_RSI, REG_RDI,
    REG_R8,  REG_R9,  REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_R15,
};
#else
#define IP(mc) ((mc)->gregs[REG_EIP])
#define NUM_REGS 8
static const int sRegs[NUM_REGS] = { REG_EAX, REG_ECX, REG_EDX, REG_EBX, REG_UESP, REG_EBP, REG_ESI, REG_EDI };
#endif

static Word* Reg(Machine* mc, int r) {
    return &mc->gregs[sRegs[r]];
}
#endif
#define SP(mc) (*Reg(mc, 4))

static int sLog;

static uint64_t OpenBusRead(uintptr_t addr, int size) {
    uint64_t value = 0;
    int i;
    for (i = 0; i < size; i++) {
        uintptr_t a = addr + i;
        uint64_t byte = a < 0x4000 ? (BIOS_OPEN_BUS >> ((a & 3) * 8)) & 0xFF : 0;
        value |= byte << (i * 8);
    }
    return value;
}

// The decoded prefixes of an instruction.
typedef struct {
    int size; // operand size: 1, 2, 4 or 8 bytes
    int rex;  // REX prefix (x86_64), or 0
} Prefix;

#define REX_W 8
#define REX_R 4
#define REX_X 2
#define REX_B 1

// Byte registers: without a REX prefix, 4-7 are AH, CH, DH, BH.
static uint64_t GetReg(Machine* mc, const Prefix* px, int r, int size) {
    uint64_t v;
    if (size == 1 && !px->rex && r >= 4)
        return ((uint64_t)*Reg(mc, r - 4) >> 8) & 0xFF;
    v = (uint64_t)*Reg(mc, r);
    return size == 8 ? v : v & ((1ull << (size * 8)) - 1);
}

static void SetReg(Machine* mc, const Prefix* px, int r, int size, uint64_t value) {
    Word* reg;
    if (size == 1 && !px->rex && r >= 4) {
        reg = Reg(mc, r - 4);
        *reg = (*reg & ~(Word)0xFF00) | (Word)((value & 0xFF) << 8);
        return;
    }
    reg = Reg(mc, r);
    if (size == 1)
        *reg = (*reg & ~(Word)0xFF) | (Word)(value & 0xFF);
    else if (size == 2)
        *reg = (*reg & ~(Word)0xFFFF) | (Word)(value & 0xFFFF);
    else
        *reg = (Word)(size == 4 ? (uint32_t)value : value); // a 32-bit write clears the upper half
}

static uint64_t Mask(int size) {
    return size == 8 ? ~0ull : (1ull << (size * 8)) - 1;
}

static uint64_t SignBit(int size) {
    return 1ull << (size * 8 - 1);
}

static void SetFlags(Machine* mc, uint64_t result, int cf, int of, int size) {
    Word flags = FLAGS(mc) & ~(Word)EFLAGS_ARITH;
    uint8_t low = result & 0xFF;
    result &= Mask(size);
    if (cf)
        flags |= EFLAGS_CF;
    if (of)
        flags |= EFLAGS_OF;
    if (result == 0)
        flags |= EFLAGS_ZF;
    if (result & SignBit(size))
        flags |= EFLAGS_SF;
    if (!__builtin_parity(low))
        flags |= EFLAGS_PF;
    FLAGS(mc) = flags;
}

// Applies an ALU operation (x86 /digit encoding) and returns the result.
static uint64_t Alu(Machine* mc, int op, uint64_t a, uint64_t b, int size) {
    uint64_t m = Mask(size), r;
    a &= m;
    b &= m;
    switch (op) {
        case 0: // add
            r = (a + b) & m;
            SetFlags(mc, r, r < a, ((~(a ^ b) & (a ^ r)) & SignBit(size)) != 0, size);
            return r;
        case 1: // or
            r = a | b;
            break;
        case 4: // and
            r = a & b;
            break;
        case 6: // xor
            r = a ^ b;
            break;
        case 5: // sub
        case 7: // cmp
            r = (a - b) & m;
            SetFlags(mc, r, a < b, (((a ^ b) & (a ^ r)) & SignBit(size)) != 0, size);
            return r;
        default:
            return a;
    }
    SetFlags(mc, r, 0, 0, size);
    return r;
}

typedef struct {
    int reg;    // ModRM reg field (with REX.R)
    int length; // bytes of ModRM + SIB + displacement
} ModRM;

static ModRM DecodeModRM(const uint8_t* p, const Prefix* px) {
    ModRM m;
    int mod = p[0] >> 6, rm = p[0] & 7;
    m.reg = ((p[0] >> 3) & 7) | (px->rex & REX_R ? 8 : 0);
    m.length = 1;
    if (mod == 3)
        return m;
    if (rm == 4) { // SIB
        int base = p[1] & 7;
        m.length++;
        if (mod == 0 && base == 5)
            m.length += 4;
    } else if (mod == 0 && rm == 5) { // disp32 (RIP-relative on x86_64)
        m.length += 4;
    }
    if (mod == 1)
        m.length += 1;
    else if (mod == 2)
        m.length += 4;
    return m;
}

// Emulates the instruction at ip touching addr. Returns its length, or 0.
static int Emulate(Machine* mc, const uint8_t* ip, uintptr_t addr) {
    const uint8_t* p = ip;
    Prefix px = { 4, 0 };
    ModRM m;
    uint64_t v;

    if (*p == 0x66) {
        px.size = 2;
        p++;
    }
#ifdef __x86_64__
    if ((*p & 0xF0) == 0x40) {
        px.rex = *p++;
        if (px.rex & REX_W)
            px.size = 8;
    }
#endif
    switch (*p) {
        case 0x8A: // mov r8, r/m8
        case 0x8B: // mov r, r/m
            if (*p == 0x8A)
                px.size = 1;
            m = DecodeModRM(p + 1, &px);
            SetReg(mc, &px, m.reg, px.size, OpenBusRead(addr, px.size));
            return p - ip + 1 + m.length;
        case 0x63: // movsxd r64, r/m32
            m = DecodeModRM(p + 1, &px);
            SetReg(mc, &px, m.reg, px.size, (uint64_t)(int64_t)(int32_t)OpenBusRead(addr, 4));
            return p - ip + 1 + m.length;
        case 0x88: // mov r/m8, r8
        case 0x89: // mov r/m, r
            m = DecodeModRM(p + 1, &px);
            return p - ip + 1 + m.length;
        case 0xC6: // mov r/m8, imm8
            m = DecodeModRM(p + 1, &px);
            return p - ip + 1 + m.length + 1;
        case 0xC7: // mov r/m, imm (at most 4 bytes)
            m = DecodeModRM(p + 1, &px);
            return p - ip + 1 + m.length + (px.size == 2 ? 2 : 4);
        case 0x0F: {
            int src = (p[1] == 0xB6 || p[1] == 0xBE) ? 1 : 2;
            if (p[1] != 0xB6 && p[1] != 0xB7 && p[1] != 0xBE && p[1] != 0xBF)
                return 0;
            m = DecodeModRM(p + 2, &px);
            v = OpenBusRead(addr, src);
            if (p[1] == 0xBE)
                v = (uint64_t)(int64_t)(int8_t)v;
            else if (p[1] == 0xBF)
                v = (uint64_t)(int64_t)(int16_t)v;
            SetReg(mc, &px, m.reg, px.size, v);
            return p - ip + 2 + m.length;
        }
        case 0x80: // op r/m8, imm8
        case 0x81: // op r/m, imm
        case 0x83: // op r/m, simm8
        {
            int opSize = *p == 0x80 ? 1 : px.size;
            int immSize = *p == 0x81 ? (px.size == 2 ? 2 : 4) : 1;
            uint64_t imm;
            m = DecodeModRM(p + 1, &px);
            imm = immSize == 1 ? p[1 + m.length]
                  : immSize == 2 ? *(const uint16_t*)(p + 1 + m.length)
                                 : (uint64_t)(int64_t)*(const int32_t*)(p + 1 + m.length);
            if (*p == 0x83)
                imm = (uint64_t)(int64_t)(int8_t)imm;
            Alu(mc, m.reg & 7, OpenBusRead(addr, opSize), imm, opSize); // result written to memory is dropped
            return p - ip + 1 + m.length + immSize;
        }
        case 0x38: // cmp r/m8, r8
        case 0x39: // cmp r/m, r
            if (*p == 0x38)
                px.size = 1;
            m = DecodeModRM(p + 1, &px);
            Alu(mc, 7, OpenBusRead(addr, px.size), GetReg(mc, &px, m.reg, px.size), px.size);
            return p - ip + 1 + m.length;
        case 0x02: // add r8, r/m8
        case 0x03: // add r, r/m
        case 0x0A: // or
        case 0x0B:
        case 0x22: // and
        case 0x23:
        case 0x2A: // sub
        case 0x2B:
        case 0x32: // xor
        case 0x33:
        case 0x3A: // cmp r8, r/m8
        case 0x3B: // cmp r, r/m
        {
            int op = *p >> 3;
            if (!(*p & 1))
                px.size = 1;
            m = DecodeModRM(p + 1, &px);
            v = Alu(mc, op, GetReg(mc, &px, m.reg, px.size), OpenBusRead(addr, px.size), px.size);
            if (op != 7)
                SetReg(mc, &px, m.reg, px.size, v);
            return p - ip + 1 + m.length;
        }
        case 0x84: // test r/m8, r8
        case 0x85: // test r/m, r
            if (*p == 0x84)
                px.size = 1;
            m = DecodeModRM(p + 1, &px);
            Alu(mc, 4, OpenBusRead(addr, px.size), GetReg(mc, &px, m.reg, px.size), px.size);
            return p - ip + 1 + m.length;
        case 0xF6: // test r/m8, imm8
            m = DecodeModRM(p + 1, &px);
            if ((m.reg & 7) != 0)
                return 0;
            Alu(mc, 4, OpenBusRead(addr, 1), p[1 + m.length], 1);
            return p - ip + 1 + m.length + 1;
        case 0xFF: // push r/m
            m = DecodeModRM(p + 1, &px);
            if ((m.reg & 7) != 6)
                return 0;
            SP(mc) -= sizeof(uintptr_t);
            *(uintptr_t*)SP(mc) = (uintptr_t)OpenBusRead(addr, sizeof(uintptr_t));
            return p - ip + 1 + m.length;
    }
    return 0;
}

// Performs a faulting access to low memory and skips the instruction. Returns
// 0 for any other fault.
static int HandleFault(Machine* mc, uintptr_t addr) {
    const uint8_t* ip = (const uint8_t*)IP(mc);
    int len = 0;

    if (addr < LOW_MEMORY_END && (uintptr_t)ip != addr)
        len = Emulate(mc, ip, addr);
    if (len == 0)
        return 0;
    if (sLog)
        fprintf(stderr, "tmc: open bus access %#lx at %p\n", (unsigned long)addr, (void*)ip);
    IP(mc) += len;
    return 1;
}

// Describes a fault that HandleFault did not handle, with the instruction bytes.
static void DescribeCrash(char* msg, size_t size, const Machine* mc, uintptr_t addr) {
    const uint8_t* ip = (const uint8_t*)IP(mc);
    int n = snprintf(msg, size, "crash at %p accessing %#lx", (void*)ip, (unsigned long)addr);
    if ((uintptr_t)ip >= LOW_MEMORY_END && n > 0 && (size_t)n < size)
        snprintf(msg + n, size - n, ", code %02x %02x %02x %02x", ip[0], ip[1], ip[2], ip[3]);
}

#ifdef _WIN32
static LONG CALLBACK Handler(EXCEPTION_POINTERS* info) {
    const EXCEPTION_RECORD* rec = info->ExceptionRecord;
    if (rec->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || rec->NumberParameters < 2)
        return EXCEPTION_CONTINUE_SEARCH;
    // Vectored handlers see every access violation first, including ones that
    // system DLLs and graphics drivers raise and catch themselves: leave those
    // to their own handlers.
    if (!HandleFault(info->ContextRecord, (uintptr_t)rec->ExceptionInformation[1]))
        return EXCEPTION_CONTINUE_SEARCH;
    return EXCEPTION_CONTINUE_EXECUTION;
}

// An access violation that nothing handled: a crash.
static LONG CALLBACK CrashFilter(EXCEPTION_POINTERS* info) {
    const EXCEPTION_RECORD* rec = info->ExceptionRecord;
    char msg[128];
    if (rec->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || rec->NumberParameters < 2)
        return EXCEPTION_CONTINUE_SEARCH;
    DescribeCrash(msg, sizeof(msg), info->ContextRecord, (uintptr_t)rec->ExceptionInformation[1]);
    PcFatal("%s", msg);
}

void PcInstallOpenBus(void) {
    sLog = getenv("TMC_LOG_OPENBUS") != NULL;
    AddVectoredExceptionHandler(1, Handler);
    SetUnhandledExceptionFilter(CrashFilter);
}
#else
static void Handler(int sig, siginfo_t* info, void* context) {
    mcontext_t* mc = &((ucontext_t*)context)->uc_mcontext;
    char msg[128];
    (void)sig;
    if (HandleFault(mc, (uintptr_t)info->si_addr))
        return;
    DescribeCrash(msg, sizeof(msg), mc, (uintptr_t)info->si_addr);
    fprintf(stderr, "tmc: %s\n", msg);
    abort();
}

void PcInstallOpenBus(void) {
    struct sigaction sa;
    sLog = getenv("TMC_LOG_OPENBUS") != NULL;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = Handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
}
#endif

#else

void PcInstallOpenBus(void) {
}

#endif
