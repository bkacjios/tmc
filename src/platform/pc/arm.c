// An ARM7TDMI interpreter (ARMv4T: ARM and Thumb), used to run the GBA BIOS's
// startup animation (bootintro.c). The game itself never runs through it.
//
// Memory is the GBA's map: the BIOS image at 0, then the regions the platform
// layer already maps at their GBA addresses (RAM, I/O, palette, VRAM, OAM,
// ROM). I/O writes with side effects (interrupt flags, HALTCNT, DMA) are
// handled here. Timing is approximate: the caller runs a number of
// instructions per scanline.

#include <string.h>
#include "pc.h"

#define MODE_USR 0x10
#define MODE_FIQ 0x11
#define MODE_IRQ 0x12
#define MODE_SVC 0x13
#define MODE_ABT 0x17
#define MODE_UND 0x1B
#define MODE_SYS 0x1F

#define FLAG_N (1u << 31)
#define FLAG_Z (1u << 30)
#define FLAG_C (1u << 29)
#define FLAG_V (1u << 28)
#define FLAG_I (1u << 7)
#define FLAG_T (1u << 5)

enum { BANK_USR, BANK_FIQ, BANK_IRQ, BANK_SVC, BANK_ABT, BANK_UND, NUM_BANKS };

static struct {
    uint32_t r[16];
    uint32_t cpsr;
    uint32_t spsr[NUM_BANKS];
    uint32_t bankR13[NUM_BANKS], bankR14[NUM_BANKS];
    uint32_t fiqR8[5], usrR8[5]; // r8-r12 of FIQ mode and of the others
    uint32_t next;               // address of the next instruction
    int halted;
    const uint8_t* bios;
} sCpu;

// DMA channels' internal addresses, and FIFO sound streams (see ArmFifoSamples).
static uint32_t sDmaSrc[4], sDmaDest[4];

static int Bank(uint32_t mode) {
    switch (mode & 0x1F) {
        case MODE_FIQ:
            return BANK_FIQ;
        case MODE_IRQ:
            return BANK_IRQ;
        case MODE_SVC:
            return BANK_SVC;
        case MODE_ABT:
            return BANK_ABT;
        case MODE_UND:
            return BANK_UND;
        default:
            return BANK_USR;
    }
}

// Switches the banked registers for a change of mode (CPSR is set by the caller).
static void SwitchMode(uint32_t newMode) {
    int from = Bank(sCpu.cpsr), to = Bank(newMode);
    if (from == to)
        return;
    sCpu.bankR13[from] = sCpu.r[13];
    sCpu.bankR14[from] = sCpu.r[14];
    if (from == BANK_FIQ) {
        memcpy(sCpu.fiqR8, &sCpu.r[8], sizeof(sCpu.fiqR8));
        memcpy(&sCpu.r[8], sCpu.usrR8, sizeof(sCpu.usrR8));
    } else if (to == BANK_FIQ) {
        memcpy(sCpu.usrR8, &sCpu.r[8], sizeof(sCpu.usrR8));
        memcpy(&sCpu.r[8], sCpu.fiqR8, sizeof(sCpu.fiqR8));
    }
    sCpu.r[13] = sCpu.bankR13[to];
    sCpu.r[14] = sCpu.bankR14[to];
}

static void SetCpsr(uint32_t value) {
    SwitchMode(value);
    sCpu.cpsr = value;
}

// ---------------------------------------------------------------------------
// Memory

static uint8_t* HostAddress(uint32_t addr, int write) {
    switch (addr >> 24) {
        case 0x00:
            return !write && addr < 0x4000 ? (uint8_t*)sCpu.bios + addr : NULL;
        case 0x02:
            return PC_EWRAM + (addr & 0x3FFFF);
        case 0x03:
            return PC_IWRAM + (addr & 0x7FFF);
        case 0x04:
            return (addr & 0xFFFFFF) < PC_IO_SIZE ? PC_IO + (addr & 0x3FF) : NULL;
        case 0x05:
            return PC_PLTT + (addr & 0x3FF);
        case 0x06:
            addr &= 0x1FFFF;
            return PC_VRAM + (addr >= 0x18000 ? addr - 0x8000 : addr);
        case 0x07:
            return PC_OAM + (addr & 0x3FF);
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
            return !write && (addr & 0x1FFFFFF) < 0x1000000 ? (uint8_t*)0x08000000 + (addr & 0xFFFFFF) : NULL;
        default:
            return NULL;
    }
}

static uint32_t Read32(uint32_t addr) {
    uint8_t* p = HostAddress(addr & ~3, 0);
    return p ? *(uint32_t*)p : 0;
}

static uint32_t Read16(uint32_t addr) {
    uint8_t* p = HostAddress(addr & ~1, 0);
    return p ? *(uint16_t*)p : 0;
}

static uint32_t Read8(uint32_t addr) {
    uint8_t* p = HostAddress(addr, 0);
    return p ? *p : 0;
}

static void IoWrite16(uint32_t offset, uint32_t value);

static void Write32(uint32_t addr, uint32_t value) {
    addr &= ~3;
    if ((addr >> 24) == 0x04) {
        IoWrite16(addr & 0x3FF, value & 0xFFFF);
        IoWrite16((addr & 0x3FF) + 2, value >> 16);
        return;
    }
    uint8_t* p = HostAddress(addr, 1);
    if (p)
        *(uint32_t*)p = value;
}

static void Write16(uint32_t addr, uint32_t value) {
    addr &= ~1;
    if ((addr >> 24) == 0x04) {
        IoWrite16(addr & 0x3FF, value & 0xFFFF);
        return;
    }
    uint8_t* p = HostAddress(addr, 1);
    if (p)
        *(uint16_t*)p = (uint16_t)value;
}

static void Write8(uint32_t addr, uint32_t value) {
    uint32_t region = addr >> 24;
    value &= 0xFF;
    if (region == 0x04) {
        uint32_t offset = addr & 0x3FF;
        if (offset == 0x301) { // HALTCNT
            sCpu.halted = 1;
            return;
        }
        if ((offset & ~1) == 0x202) { // IF: writing 1 clears
            PC_REG16(0x202) &= ~(value << ((offset & 1) * 8));
            return;
        }
        IoWrite16(offset & ~1, (offset & 1) ? (PC_REG16(offset & ~1) & 0xFF) | (value << 8)
                                            : (PC_REG16(offset) & 0xFF00) | value);
        return;
    }
    if (region == 0x05 || (region == 0x06 && (addr & 0x1FFFF) < 0x10000)) {
        Write16(addr, value | (value << 8)); // 8-bit writes to palette and BG VRAM fill a halfword
        return;
    }
    if (region == 0x07 || region == 0x06)
        return; // ignored for OAM and OBJ VRAM
    uint8_t* p = HostAddress(addr, 1);
    if (p)
        *p = (uint8_t)value;
}

// ---------------------------------------------------------------------------
// DMA

#define DMA_CNT(ch) (0xB8 + (ch) * 12)

static void DmaTransfer(int ch) {
    uint32_t control = PC_REG32(DMA_CNT(ch));
    uint32_t count = control & (ch == 3 ? 0xFFFF : 0x3FFF);
    int word = (control >> 26) & 1;
    uint32_t unit = word ? 4 : 2;
    int srcMode = (control >> 23) & 3, destMode = (control >> 21) & 3;
    int32_t srcStep = srcMode == 0 ? (int32_t)unit : srcMode == 1 ? -(int32_t)unit : 0;
    int32_t destStep = destMode == 0 || destMode == 3 ? (int32_t)unit : destMode == 1 ? -(int32_t)unit : 0;

    if (count == 0)
        count = ch == 3 ? 0x10000 : 0x4000;
    while (count--) {
        if (word)
            Write32(sDmaDest[ch], Read32(sDmaSrc[ch]));
        else
            Write16(sDmaDest[ch], Read16(sDmaSrc[ch]));
        sDmaSrc[ch] += srcStep;
        sDmaDest[ch] += destStep;
    }
    if (destMode == 3)
        sDmaDest[ch] = PC_REG32(DMA_CNT(ch) - 4);
    if (!((control >> 25) & 1) || ((control >> 28) & 3) == 0) {
        PC_REG32(DMA_CNT(ch)) = control & ~(1u << 31);
        if ((control >> 30) & 1)
            PC_REG16(0x202) |= 1 << (8 + ch);
    }
}

void ArmRunTimedDma(int timing) {
    int ch;
    for (ch = 0; ch < 4; ch++) {
        uint32_t control = PC_REG32(DMA_CNT(ch));
        if ((control >> 31) && ((control >> 28) & 3) == (uint32_t)timing)
            DmaTransfer(ch);
    }
}

// Takes count bytes from a sound FIFO's DMA stream (DMA 1 or 2 in special
// timing, feeding FIFO A at 0x040000A0): what the FIFO plays this frame.
// Returns 0 if no DMA feeds it.
int ArmFifoSamples(int8_t* out, int count) {
    int ch, i;
    for (ch = 1; ch <= 2; ch++) {
        uint32_t control = PC_REG32(DMA_CNT(ch));
        if ((control >> 31) && ((control >> 28) & 3) == 3 && PC_REG32(DMA_CNT(ch) - 4) == 0x040000A0) {
            for (i = 0; i < count; i++)
                out[i] = (int8_t)Read8(sDmaSrc[ch]++);
            return 1;
        }
    }
    return 0;
}

static void IoWrite16(uint32_t offset, uint32_t value) {
    if (offset == 0x202) { // IF: writing 1 clears
        PC_REG16(0x202) &= ~value;
        return;
    }
    if (offset >= 0x400)
        return;
    if (offset == 0x130 || offset == 0x006) // KEYINPUT, VCOUNT: read only
        return;
    if (offset >= 0xB0 && offset < 0xE0 && (offset - 0xB0) % 12 == 10) { // DMAxCNT_H
        int ch = (offset - 0xB0) / 12;
        uint32_t old = PC_REG16(offset);
        PC_REG16(offset) = (uint16_t)value;
        if ((value & 0x8000) && !(old & 0x8000)) {
            sDmaSrc[ch] = PC_REG32(offset - 10);
            sDmaDest[ch] = PC_REG32(offset - 6);
            if (((value >> 12) & 3) == 0)
                DmaTransfer(ch);
        }
        return;
    }
    PC_REG16(offset) = (uint16_t)value;
}

// ---------------------------------------------------------------------------
// Execution

static int ConditionPassed(uint32_t cond) {
    uint32_t f = sCpu.cpsr;
    int n = !!(f & FLAG_N), z = !!(f & FLAG_Z), c = !!(f & FLAG_C), v = !!(f & FLAG_V);
    switch (cond) {
        case 0x0:
            return z;
        case 0x1:
            return !z;
        case 0x2:
            return c;
        case 0x3:
            return !c;
        case 0x4:
            return n;
        case 0x5:
            return !n;
        case 0x6:
            return v;
        case 0x7:
            return !v;
        case 0x8:
            return c && !z;
        case 0x9:
            return !c || z;
        case 0xA:
            return n == v;
        case 0xB:
            return n != v;
        case 0xC:
            return !z && n == v;
        case 0xD:
            return z || n != v;
        case 0xE:
            return 1;
        default:
            return 0;
    }
}

static void SetNZ(uint32_t result) {
    sCpu.cpsr = (sCpu.cpsr & ~(FLAG_N | FLAG_Z)) | (result & FLAG_N) | (result == 0 ? FLAG_Z : 0);
}

static void SetC(int c) {
    sCpu.cpsr = c ? sCpu.cpsr | FLAG_C : sCpu.cpsr & ~FLAG_C;
}

static void SetV(int v) {
    sCpu.cpsr = v ? sCpu.cpsr | FLAG_V : sCpu.cpsr & ~FLAG_V;
}

// a + b + carry, setting C and V if flags.
static uint32_t Add(uint32_t a, uint32_t b, uint32_t carry, int flags) {
    uint64_t wide = (uint64_t)a + b + carry;
    uint32_t r = (uint32_t)wide;
    if (flags) {
        SetNZ(r);
        SetC(wide >> 32);
        SetV(((a ^ r) & (b ^ r)) >> 31);
    }
    return r;
}

// a - b - !carry (ARM's carry is "no borrow").
static uint32_t Sub(uint32_t a, uint32_t b, uint32_t carry, int flags) {
    return Add(a, ~b, carry, flags);
}

static void WritePc(uint32_t value) {
    sCpu.next = value & (sCpu.cpsr & FLAG_T ? ~1u : ~3u);
}

// A barrel shifter operation. Shift by an immediate amount (byReg 0) has the
// special cases of amount 0 (LSR/ASR #32, RRX).
static uint32_t Shift(uint32_t value, int type, uint32_t amount, int byReg, int* carry) {
    switch (type) {
        case 0: // LSL
            if (amount == 0)
                return value;
            if (amount < 32) {
                *carry = (value >> (32 - amount)) & 1;
                return value << amount;
            }
            *carry = amount == 32 ? value & 1 : 0;
            return 0;
        case 1: // LSR
            if (amount == 0) {
                if (byReg)
                    return value;
                amount = 32;
            }
            if (amount < 32) {
                *carry = (value >> (amount - 1)) & 1;
                return value >> amount;
            }
            *carry = amount == 32 ? value >> 31 : 0;
            return 0;
        case 2: // ASR
            if (amount == 0) {
                if (byReg)
                    return value;
                amount = 32;
            }
            if (amount < 32) {
                *carry = (value >> (amount - 1)) & 1;
                return (uint32_t)((int32_t)value >> amount);
            }
            *carry = value >> 31;
            return value >> 31 ? 0xFFFFFFFF : 0;
        default: // ROR, RRX
            if (amount == 0) {
                if (byReg)
                    return value;
                {
                    int c = *carry;
                    *carry = value & 1;
                    return (value >> 1) | ((uint32_t)c << 31);
                }
            }
            amount &= 31;
            if (amount == 0) {
                *carry = value >> 31;
                return value;
            }
            *carry = (value >> (amount - 1)) & 1;
            return (value >> amount) | (value << (32 - amount));
    }
}

static void Exception(uint32_t vector, uint32_t mode, uint32_t returnAddress) {
    uint32_t old = sCpu.cpsr;
    SetCpsr((old & ~(0x1F | FLAG_T)) | mode | FLAG_I);
    sCpu.spsr[Bank(mode)] = old;
    sCpu.r[14] = returnAddress;
    sCpu.next = vector;
}

// Data processing: AND, EOR, SUB, ... MVN.
static void DataProcessing(uint32_t op, uint32_t rd, uint32_t a, uint32_t b, int s, int shifterCarry) {
    uint32_t c = !!(sCpu.cpsr & FLAG_C);
    uint32_t r = 0;
    int logical = 0, write = 1;

    switch (op) {
        case 0x0:
            r = a & b, logical = 1;
            break;
        case 0x1:
            r = a ^ b, logical = 1;
            break;
        case 0x2:
            r = Sub(a, b, 1, s);
            break;
        case 0x3:
            r = Sub(b, a, 1, s);
            break;
        case 0x4:
            r = Add(a, b, 0, s);
            break;
        case 0x5:
            r = Add(a, b, c, s);
            break;
        case 0x6:
            r = Sub(a, b, c, s);
            break;
        case 0x7:
            r = Sub(b, a, c, s);
            break;
        case 0x8:
            r = a & b, logical = 1, write = 0;
            break;
        case 0x9:
            r = a ^ b, logical = 1, write = 0;
            break;
        case 0xA:
            r = Sub(a, b, 1, s), write = 0;
            break;
        case 0xB:
            r = Add(a, b, 0, s), write = 0;
            break;
        case 0xC:
            r = a | b, logical = 1;
            break;
        case 0xD:
            r = b, logical = 1;
            break;
        case 0xE:
            r = a & ~b, logical = 1;
            break;
        default:
            r = ~b, logical = 1;
            break;
    }
    if (s && logical) {
        SetNZ(r);
        SetC(shifterCarry);
    }
    if (write) {
        if (rd == 15) {
            if (s) // return from an exception: CPSR = SPSR
                SetCpsr(sCpu.spsr[Bank(sCpu.cpsr)]);
            WritePc(r);
        } else {
            sCpu.r[rd] = r;
        }
    }
}

// Loads or stores a register list (LDM/STM, PUSH/POP), lowest register at the
// lowest address. userBank: transfer the user mode registers (S bit).
static void BlockTransfer(uint32_t addr, uint32_t list, int load, int userBank) {
    uint32_t savedCpsr = sCpu.cpsr;
    int i;
    if (userBank)
        SetCpsr((sCpu.cpsr & ~0x1F) | MODE_SYS);
    for (i = 0; i < 16; i++) {
        if (!(list & (1 << i)))
            continue;
        if (load) {
            uint32_t v = Read32(addr);
            if (i == 15)
                WritePc(v);
            else
                sCpu.r[i] = v;
        } else {
            Write32(addr, i == 15 ? sCpu.r[15] + 4 : sCpu.r[i]);
        }
        addr += 4;
    }
    if (userBank)
        SetCpsr(savedCpsr);
}

static int PopCount(uint32_t x) {
    return __builtin_popcount(x);
}

static void ExecuteArm(uint32_t instr) {
    uint32_t rn = (instr >> 16) & 15, rd = (instr >> 12) & 15;

    if (!ConditionPassed(instr >> 28))
        return;

    switch ((instr >> 25) & 7) {
        case 0:
        case 1: {
            if ((instr & 0x0FFFFFF0) == 0x012FFF10) { // BX
                uint32_t target = sCpu.r[instr & 15];
                sCpu.cpsr = target & 1 ? sCpu.cpsr | FLAG_T : sCpu.cpsr & ~FLAG_T;
                WritePc(target);
                return;
            }
            if ((instr & 0x0E000090) == 0x00000090 && !(instr & (1 << 25))) {
                uint32_t rm = instr & 15, rs = (instr >> 8) & 15;
                if ((instr & 0x0FC000F0) == 0x00000090) { // MUL, MLA
                    uint32_t r = sCpu.r[rm] * sCpu.r[rs] + ((instr >> 21) & 1 ? sCpu.r[rd] : 0);
                    sCpu.r[rn] = r; // Rd is in bits 16-19
                    if ((instr >> 20) & 1)
                        SetNZ(r);
                    return;
                }
                if ((instr & 0x0F8000F0) == 0x00800090) { // UMULL, UMLAL, SMULL, SMLAL
                    uint64_t r = (instr >> 22) & 1 ? (uint64_t)((int64_t)(int32_t)sCpu.r[rm] * (int32_t)sCpu.r[rs])
                                                   : (uint64_t)sCpu.r[rm] * sCpu.r[rs];
                    if ((instr >> 21) & 1)
                        r += ((uint64_t)sCpu.r[rn] << 32) | sCpu.r[rd];
                    sCpu.r[rd] = (uint32_t)r;
                    sCpu.r[rn] = (uint32_t)(r >> 32);
                    if ((instr >> 20) & 1)
                        sCpu.cpsr = (sCpu.cpsr & ~(FLAG_N | FLAG_Z)) | ((r >> 32) & FLAG_N) | (r == 0 ? FLAG_Z : 0);
                    return;
                }
                if ((instr & 0x0FB00FF0) == 0x01000090) { // SWP, SWPB
                    uint32_t addr = sCpu.r[rn];
                    if ((instr >> 22) & 1) {
                        uint32_t v = Read8(addr);
                        Write8(addr, sCpu.r[rm]);
                        sCpu.r[rd] = v;
                    } else {
                        uint32_t v = Read32(addr);
                        uint32_t rot = (addr & 3) * 8;
                        Write32(addr, sCpu.r[rm]);
                        sCpu.r[rd] = rot ? (v >> rot) | (v << (32 - rot)) : v;
                    }
                    return;
                }
                { // LDRH, STRH, LDRSB, LDRSH
                    int pre = (instr >> 24) & 1, up = (instr >> 23) & 1, wb = (instr >> 21) & 1, load = (instr >> 20) & 1;
                    uint32_t offset = (instr >> 22) & 1 ? ((instr >> 4) & 0xF0) | (instr & 15) : sCpu.r[rm];
                    uint32_t base = sCpu.r[rn];
                    uint32_t addr = pre ? (up ? base + offset : base - offset) : base;
                    uint32_t value = 0;
                    int sh = (instr >> 5) & 3;
                    if (load) {
                        if (sh == 1) {
                            value = Read16(addr);
                            if (addr & 1)
                                value = (value >> 8) | (value << 24);
                        } else if (sh == 2) {
                            value = (uint32_t)(int8_t)Read8(addr);
                        } else {
                            value = (addr & 1) ? (uint32_t)(int8_t)Read8(addr) : (uint32_t)(int16_t)Read16(addr);
                        }
                    } else {
                        Write16(addr, rd == 15 ? sCpu.r[15] + 4 : sCpu.r[rd]);
                    }
                    if (!pre)
                        sCpu.r[rn] = up ? base + offset : base - offset;
                    else if (wb)
                        sCpu.r[rn] = addr;
                    if (load) {
                        if (rd == 15)
                            WritePc(value);
                        else
                            sCpu.r[rd] = value;
                    }
                    return;
                }
            }
            if ((instr & 0x0FBF0FFF) == 0x010F0000) { // MRS
                sCpu.r[rd] = (instr >> 22) & 1 ? sCpu.spsr[Bank(sCpu.cpsr)] : sCpu.cpsr;
                return;
            }
            if ((instr & 0x0DB0F000) == 0x0120F000) { // MSR
                uint32_t value, mask = 0;
                if ((instr >> 25) & 1) {
                    uint32_t rot = ((instr >> 8) & 15) * 2, imm = instr & 0xFF;
                    value = rot ? (imm >> rot) | (imm << (32 - rot)) : imm;
                } else {
                    value = sCpu.r[instr & 15];
                }
                if ((instr >> 16) & 1)
                    mask |= 0x000000FF;
                if ((instr >> 19) & 1)
                    mask |= 0xFF000000;
                if ((instr >> 22) & 1) {
                    uint32_t* spsr = &sCpu.spsr[Bank(sCpu.cpsr)];
                    *spsr = (*spsr & ~mask) | (value & mask);
                } else {
                    if ((sCpu.cpsr & 0x1F) == MODE_USR)
                        mask &= 0xFF000000;
                    SetCpsr((sCpu.cpsr & ~mask) | (value & mask));
                }
                return;
            }
            { // Data processing
                uint32_t op = (instr >> 21) & 15, b;
                int s = (instr >> 20) & 1, carry = !!(sCpu.cpsr & FLAG_C);
                uint32_t a = sCpu.r[rn];
                if ((instr >> 25) & 1) {
                    uint32_t rot = ((instr >> 8) & 15) * 2, imm = instr & 0xFF;
                    b = rot ? (imm >> rot) | (imm << (32 - rot)) : imm;
                    if (rot)
                        carry = b >> 31;
                } else {
                    uint32_t rm = instr & 15, value = sCpu.r[rm], amount;
                    int byReg = (instr >> 4) & 1;
                    if (byReg) {
                        amount = sCpu.r[(instr >> 8) & 15] & 0xFF;
                        if (rm == 15)
                            value += 4; // PC reads 12 ahead with a register shift
                        if (rn == 15)
                            a += 4;
                    } else {
                        amount = (instr >> 7) & 31;
                    }
                    b = Shift(value, (instr >> 5) & 3, amount, byReg, &carry);
                }
                DataProcessing(op, rd, a, b, s, carry);
            }
            return;
        }
        case 2:
        case 3: { // LDR, STR, LDRB, STRB
            int pre = (instr >> 24) & 1, up = (instr >> 23) & 1, byte = (instr >> 22) & 1, wb = (instr >> 21) & 1,
                load = (instr >> 20) & 1;
            uint32_t offset, base = sCpu.r[rn], addr;
            if ((instr >> 25) & 1) {
                int carry = !!(sCpu.cpsr & FLAG_C);
                if (instr & 0x10) { // undefined
                    Exception(0x04, MODE_UND, sCpu.r[15] - 4);
                    return;
                }
                offset = Shift(sCpu.r[instr & 15], (instr >> 5) & 3, (instr >> 7) & 31, 0, &carry);
            } else {
                offset = instr & 0xFFF;
            }
            addr = pre ? (up ? base + offset : base - offset) : base;
            if (load) {
                uint32_t v;
                if (byte) {
                    v = Read8(addr);
                } else {
                    uint32_t rot = (addr & 3) * 8;
                    v = Read32(addr);
                    if (rot)
                        v = (v >> rot) | (v << (32 - rot));
                }
                if (!pre)
                    sCpu.r[rn] = up ? base + offset : base - offset;
                else if (wb)
                    sCpu.r[rn] = addr;
                if (rd == 15)
                    WritePc(v);
                else
                    sCpu.r[rd] = v;
            } else {
                uint32_t v = rd == 15 ? sCpu.r[15] + 4 : sCpu.r[rd];
                if (byte)
                    Write8(addr, v);
                else
                    Write32(addr, v);
                if (!pre)
                    sCpu.r[rn] = up ? base + offset : base - offset;
                else if (wb)
                    sCpu.r[rn] = addr;
            }
            return;
        }
        case 4: { // LDM, STM
            int pre = (instr >> 24) & 1, up = (instr >> 23) & 1, s = (instr >> 22) & 1, wb = (instr >> 21) & 1,
                load = (instr >> 20) & 1;
            uint32_t list = instr & 0xFFFF, n = PopCount(list), base = sCpu.r[rn], start, end;
            if (n == 0)
                return;
            if (up) {
                start = pre ? base + 4 : base;
                end = base + n * 4;
            } else {
                start = pre ? base - n * 4 : base - n * 4 + 4;
                end = base - n * 4;
            }
            if (load) {
                int userBank = s && !(list & (1 << 15));
                if (wb)
                    sCpu.r[rn] = end;
                BlockTransfer(start, list, 1, userBank);
                if (s && (list & (1 << 15)))
                    SetCpsr(sCpu.spsr[Bank(sCpu.cpsr)]);
            } else {
                BlockTransfer(start, list, 0, s);
                if (wb)
                    sCpu.r[rn] = end;
            }
            return;
        }
        case 5: { // B, BL
            int32_t offset = (int32_t)(instr << 8) >> 6;
            if ((instr >> 24) & 1)
                sCpu.r[14] = sCpu.r[15] - 4;
            WritePc(sCpu.r[15] + offset);
            return;
        }
        case 7:
            if (((instr >> 24) & 15) == 15) { // SWI
                Exception(0x08, MODE_SVC, sCpu.r[15] - 4);
                return;
            }
            // fall through: coprocessor
        default:
            Exception(0x04, MODE_UND, sCpu.r[15] - 4);
            return;
    }
}

static void ExecuteThumb(uint32_t instr) {
    uint32_t* r = sCpu.r;
    int carry = !!(sCpu.cpsr & FLAG_C);

    switch (instr >> 13) {
        case 0: {
            uint32_t rd = instr & 7, rs = (instr >> 3) & 7;
            if (((instr >> 11) & 3) == 3) { // ADD/SUB register or imm3
                uint32_t b = (instr >> 10) & 1 ? (instr >> 6) & 7 : r[(instr >> 6) & 7];
                r[rd] = (instr >> 9) & 1 ? Sub(r[rs], b, 1, 1) : Add(r[rs], b, 0, 1);
            } else { // LSL, LSR, ASR #imm
                r[rd] = Shift(r[rs], (instr >> 11) & 3, (instr >> 6) & 31, 0, &carry);
                SetNZ(r[rd]);
                SetC(carry);
            }
            return;
        }
        case 1: { // MOV, CMP, ADD, SUB #imm8
            uint32_t rd = (instr >> 8) & 7, imm = instr & 0xFF;
            switch ((instr >> 11) & 3) {
                case 0:
                    r[rd] = imm;
                    SetNZ(imm);
                    break;
                case 1:
                    Sub(r[rd], imm, 1, 1);
                    break;
                case 2:
                    r[rd] = Add(r[rd], imm, 0, 1);
                    break;
                default:
                    r[rd] = Sub(r[rd], imm, 1, 1);
                    break;
            }
            return;
        }
        case 2:
            if ((instr >> 10) == 0x10) { // ALU operations
                uint32_t rd = instr & 7, rs = (instr >> 3) & 7, a = r[rd], b = r[rs], res;
                switch ((instr >> 6) & 15) {
                    case 0x0:
                        SetNZ(r[rd] = a & b);
                        break;
                    case 0x1:
                        SetNZ(r[rd] = a ^ b);
                        break;
                    case 0x2:
                    case 0x3:
                    case 0x4:
                    case 0x7: {
                        static const int sTypes[8] = { 0, 0, 0, 1, 2, 0, 0, 3 };
                        res = Shift(a, sTypes[(instr >> 6) & 7], b & 0xFF, 1, &carry);
                        r[rd] = res;
                        SetNZ(res);
                        SetC(carry);
                        break;
                    }
                    case 0x5:
                        r[rd] = Add(a, b, carry, 1);
                        break;
                    case 0x6:
                        r[rd] = Sub(a, b, carry, 1);
                        break;
                    case 0x8:
                        SetNZ(a & b);
                        break;
                    case 0x9:
                        r[rd] = Sub(0, b, 1, 1);
                        break;
                    case 0xA:
                        Sub(a, b, 1, 1);
                        break;
                    case 0xB:
                        Add(a, b, 0, 1);
                        break;
                    case 0xC:
                        SetNZ(r[rd] = a | b);
                        break;
                    case 0xD:
                        SetNZ(r[rd] = a * b);
                        break;
                    case 0xE:
                        SetNZ(r[rd] = a & ~b);
                        break;
                    default:
                        SetNZ(r[rd] = ~b);
                        break;
                }
                return;
            }
            if ((instr >> 10) == 0x11) { // hi register operations, BX
                uint32_t rd = (instr & 7) | ((instr >> 4) & 8), rs = (instr >> 3) & 15;
                switch ((instr >> 8) & 3) {
                    case 0:
                        if (rd == 15)
                            WritePc(r[15] + r[rs]);
                        else
                            r[rd] += r[rs];
                        break;
                    case 1:
                        Sub(r[rd], r[rs], 1, 1);
                        break;
                    case 2:
                        if (rd == 15)
                            WritePc(r[rs]);
                        else
                            r[rd] = r[rs];
                        break;
                    default: {
                        uint32_t target = r[rs];
                        if (!(target & 1))
                            sCpu.cpsr &= ~FLAG_T;
                        WritePc(target);
                        break;
                    }
                }
                return;
            }
            if ((instr >> 11) == 0x09) { // LDR Rd, [PC, #imm]
                r[(instr >> 8) & 7] = Read32((r[15] & ~2) + (instr & 0xFF) * 4);
                return;
            }
            { // load/store with register offset
                uint32_t rd = instr & 7, addr = r[(instr >> 3) & 7] + r[(instr >> 6) & 7];
                if (!((instr >> 9) & 1)) {
                    switch ((instr >> 10) & 3) {
                        case 0:
                            Write32(addr, r[rd]);
                            break;
                        case 1:
                            Write8(addr, r[rd]);
                            break;
                        case 2: {
                            uint32_t v = Read32(addr), rot = (addr & 3) * 8;
                            r[rd] = rot ? (v >> rot) | (v << (32 - rot)) : v;
                            break;
                        }
                        default:
                            r[rd] = Read8(addr);
                            break;
                    }
                } else {
                    switch ((instr >> 10) & 3) {
                        case 0:
                            Write16(addr, r[rd]);
                            break;
                        case 1:
                            r[rd] = (uint32_t)(int8_t)Read8(addr);
                            break;
                        case 2:
                            r[rd] = Read16(addr);
                            break;
                        default:
                            r[rd] = (addr & 1) ? (uint32_t)(int8_t)Read8(addr) : (uint32_t)(int16_t)Read16(addr);
                            break;
                    }
                }
                return;
            }
        case 3: { // load/store with immediate offset
            uint32_t rd = instr & 7, base = r[(instr >> 3) & 7], imm = (instr >> 6) & 31;
            int byte = (instr >> 12) & 1, load = (instr >> 11) & 1;
            uint32_t addr = base + (byte ? imm : imm * 4);
            if (load) {
                if (byte) {
                    r[rd] = Read8(addr);
                } else {
                    uint32_t v = Read32(addr), rot = (addr & 3) * 8;
                    r[rd] = rot ? (v >> rot) | (v << (32 - rot)) : v;
                }
            } else if (byte) {
                Write8(addr, r[rd]);
            } else {
                Write32(addr, r[rd]);
            }
            return;
        }
        case 4:
            if (!((instr >> 12) & 1)) { // LDRH, STRH #imm
                uint32_t rd = instr & 7, addr = r[(instr >> 3) & 7] + ((instr >> 6) & 31) * 2;
                if ((instr >> 11) & 1)
                    r[rd] = Read16(addr);
                else
                    Write16(addr, r[rd]);
            } else { // SP-relative load/store
                uint32_t rd = (instr >> 8) & 7, addr = r[13] + (instr & 0xFF) * 4;
                if ((instr >> 11) & 1) {
                    uint32_t v = Read32(addr), rot = (addr & 3) * 8;
                    r[rd] = rot ? (v >> rot) | (v << (32 - rot)) : v;
                } else {
                    Write32(addr, r[rd]);
                }
            }
            return;
        case 5:
            if (!((instr >> 12) & 1)) { // ADD Rd, PC/SP, #imm
                uint32_t rd = (instr >> 8) & 7;
                r[rd] = ((instr >> 11) & 1 ? r[13] : r[15] & ~2) + (instr & 0xFF) * 4;
                return;
            }
            if (((instr >> 8) & 15) == 0) { // ADD SP, #±imm
                uint32_t imm = (instr & 0x7F) * 4;
                r[13] = (instr >> 7) & 1 ? r[13] - imm : r[13] + imm;
                return;
            }
            if (((instr >> 9) & 3) == 2) { // PUSH, POP
                uint32_t list = instr & 0xFF;
                if ((instr >> 11) & 1) { // POP
                    if ((instr >> 8) & 1)
                        list |= 1 << 15;
                    BlockTransfer(r[13], list, 1, 0);
                    r[13] += PopCount(list) * 4;
                } else {
                    if ((instr >> 8) & 1)
                        list |= 1 << 14;
                    r[13] -= PopCount(list) * 4;
                    BlockTransfer(r[13], list, 0, 0);
                }
                return;
            }
            Exception(0x04, MODE_UND, r[15] - 2);
            return;
        case 6:
            if (!((instr >> 12) & 1)) { // LDMIA, STMIA
                uint32_t rb = (instr >> 8) & 7, list = instr & 0xFF, base = r[rb];
                if (list == 0)
                    return;
                if ((instr >> 11) & 1) {
                    r[rb] = base + PopCount(list) * 4;
                    BlockTransfer(base, list, 1, 0); // a loaded Rb wins over the writeback
                } else {
                    BlockTransfer(base, list, 0, 0);
                    r[rb] = base + PopCount(list) * 4;
                }
                return;
            }
            if (((instr >> 8) & 15) == 15) { // SWI
                Exception(0x08, MODE_SVC, r[15] - 2);
                return;
            }
            if (ConditionPassed((instr >> 8) & 15))
                WritePc(r[15] + ((int32_t)(int8_t)(instr & 0xFF) << 1));
            return;
        default:
            if (((instr >> 11) & 3) == 0) { // B
                WritePc(r[15] + ((int32_t)(instr << 21) >> 20));
            } else if (((instr >> 11) & 3) == 2) { // BL, first half
                r[14] = r[15] + ((int32_t)(instr << 21) >> 9);
            } else if (((instr >> 11) & 3) == 3) { // BL, second half
                uint32_t target = r[14] + (instr & 0x7FF) * 2;
                r[14] = (r[15] - 2) | 1;
                WritePc(target);
            } else {
                Exception(0x04, MODE_UND, r[15] - 2);
            }
            return;
    }
}

// ---------------------------------------------------------------------------
// Interface

void ArmReset(const uint8_t* bios) {
    memset(&sCpu, 0, sizeof(sCpu));
    memset(sDmaSrc, 0, sizeof(sDmaSrc));
    memset(sDmaDest, 0, sizeof(sDmaDest));
    sCpu.bios = bios;
    sCpu.cpsr = MODE_SVC | FLAG_I | (1u << 6); // IRQ and FIQ disabled
    sCpu.next = 0;
}

uint32_t ArmNextPc(void) {
    return sCpu.next;
}

// Runs up to count instructions; stops early at address stopAt. Takes the
// IRQ exception if one is pending and enabled, and wakes a halted CPU when
// any enabled interrupt is flagged.
void ArmRun(int count, uint32_t stopAt) {
    while (count-- > 0) {
        uint32_t pending = PC_REG16(0x200) & PC_REG16(0x202);
        if (sCpu.halted) {
            if (!pending)
                return;
            sCpu.halted = 0;
        }
        if (pending && (PC_REG16(0x208) & 1) && !(sCpu.cpsr & FLAG_I))
            Exception(0x18, MODE_IRQ, sCpu.next + 4);
        if (sCpu.next == stopAt)
            return;
        if (sCpu.cpsr & FLAG_T) {
            uint32_t pc = sCpu.next;
            uint32_t instr = Read16(pc);
            sCpu.next = pc + 2;
            sCpu.r[15] = pc + 4;
            ExecuteThumb(instr);
        } else {
            uint32_t pc = sCpu.next;
            uint32_t instr = Read32(pc);
            sCpu.next = pc + 4;
            sCpu.r[15] = pc + 8;
            ExecuteArm(instr);
        }
    }
}
