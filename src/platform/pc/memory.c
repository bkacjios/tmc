// Maps the GBA memory regions at their GBA addresses. The game's RAM variables
// are linked at fixed EWRAM/IWRAM addresses and it reads hardware registers,
// palette, VRAM and OAM through fixed addresses as well.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "pc.h"

#if UINTPTR_MAX > 0xFFFFFFFF
// On the GBA, gPlayerEntity, gAuxPlayerEntities and gEntities are adjacent,
// each slot the size of a GenericEntity, and the game walks them as one array
// (ClearAllDeletedEntities, CreateEntity). Separate commons would land in any
// order, so the 64-bit build defines them here as parts of one object
// (gen_linker.py leaves them out of the commons).
static struct {
    union {
        PlayerEntity player;
        GenericEntity slot;
    } player;
    GenericEntity aux[MAX_AUX_PLAYER_ENTITIES];
    GenericEntity entities[MAX_ENTITIES];
} sEntitySlots __attribute__((section(".gba_ram"), aligned(16)));

__asm__(".globl gPlayerEntity\n.set gPlayerEntity, %c0\n"
        ".globl gAuxPlayerEntities\n.set gAuxPlayerEntities, %c1\n"
        ".globl gEntities\n.set gEntities, %c2"
        :
        : "s"(&sEntitySlots.player), "s"(sEntitySlots.aux), "s"(sEntitySlots.entities));
#endif

#ifdef _WIN32
#include <windows.h>

// Set in the child process of PcEnsureGbaRanges, whose GBA ranges the parent
// reserved before the child ran.
#define RESERVED_ENV "TMC_GBA_RESERVED"

// Returns 1 if addr starts a reservation (not committed) made by the parent.
static int IsReservation(void* addr) {
    MEMORY_BASIC_INFORMATION info;
    return getenv(RESERVED_ENV) != NULL && VirtualQuery(addr, &info, sizeof(info)) != 0 &&
           info.State == MEM_RESERVE && info.AllocationBase == addr;
}

static int MapRegion(void* addr, size_t size) {
    if (IsReservation(addr))
        return VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE) == addr;
    return VirtualAlloc(addr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) == addr;
}

void PcReleaseReservation(void* addr) {
    if (IsReservation(addr))
        VirtualFree(addr, 0, MEM_RELEASE);
}

static int ProtectReadOnly(void* addr, size_t size) {
    DWORD old;
    return VirtualProtect(addr, size, PAGE_READONLY, &old);
}
#else
#include <sys/mman.h>

#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x100000
#endif

static int MapRegion(void* addr, size_t size) {
    void* p = mmap(addr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    return p == addr;
}

static int ProtectReadOnly(void* addr, size_t size) {
    return mprotect(addr, size, PROT_READ) == 0;
}

void PcReleaseReservation(void* addr) {
    (void)addr;
}
#endif

int PcMapReadOnlyCopy(void* addr, const void* src, size_t size) {
    if (!MapRegion(addr, size))
        return 0;
    memcpy(addr, src, size);
    return ProtectReadOnly(addr, size);
}

static const struct {
    uint8_t* addr;
    size_t size;
    const char* name;
} sRegions[] = {
    // IWRAM's last word holds the interrupt handler's address (INTR_VECTOR),
    // 8 bytes in the 64-bit build: an extra page takes the overflow.
    { PC_EWRAM, PC_EWRAM_SIZE, "EWRAM" }, { PC_IWRAM, PC_IWRAM_SIZE + 0x1000, "IWRAM" },
    { PC_IO, 0x1000, "I/O" },             { PC_PLTT, 0x1000, "palette" },
    { PC_VRAM, PC_VRAM_SIZE, "VRAM" },    { PC_OAM, 0x1000, "OAM" },
};

#ifdef _WIN32
static int IsFree(void* addr, size_t size) {
    MEMORY_BASIC_INFORMATION info;
    return VirtualQuery(addr, &info, sizeof(info)) != 0 && info.State == MEM_FREE && info.RegionSize >= size;
}

// On 64-bit Windows, WoW64 maps large sections low in a 32-bit process while
// it starts (a 32 MiB one at 0x00A80000-0x02A80000 was seen on Windows 11),
// before any of the program's code runs. A process created suspended has not
// done that yet, so the ranges can be reserved in it first.
void PcEnsureGbaRanges(void) {
    STARTUPINFOW startup;
    PROCESS_INFORMATION child;
    DWORD exitCode = 1;
    int taken = !IsFree((void*)0x08000000, PC_ROM_SIZE);
    size_t i;

    for (i = 0; i < sizeof(sRegions) / sizeof(sRegions[0]); i++)
        taken |= !IsFree(sRegions[i].addr, sRegions[i].size);
    if (!taken || getenv(RESERVED_ENV) != NULL)
        return;

    GetStartupInfoW(&startup);
    SetEnvironmentVariableA(RESERVED_ENV, "1");
    if (!CreateProcessW(NULL, GetCommandLineW(), NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &startup, &child))
        return; // PcMapMemory reports the conflict
    for (i = 0; i < sizeof(sRegions) / sizeof(sRegions[0]); i++)
        VirtualAllocEx(child.hProcess, sRegions[i].addr, sRegions[i].size, MEM_RESERVE, PAGE_NOACCESS);
    VirtualAllocEx(child.hProcess, (void*)0x08000000, PC_ROM_SIZE, MEM_RESERVE, PAGE_NOACCESS);
    ResumeThread(child.hThread);
    WaitForSingleObject(child.hProcess, INFINITE);
    GetExitCodeProcess(child.hProcess, &exitCode);
    ExitProcess(exitCode);
}
#else
void PcEnsureGbaRanges(void) {
}
#endif

void PcMapMemory(void) {
    size_t i;
    for (i = 0; i < sizeof(sRegions) / sizeof(sRegions[0]); i++) {
        if (!MapRegion(sRegions[i].addr, sRegions[i].size))
            PcFatal("cannot map %s at %p", sRegions[i].name, (void*)sRegions[i].addr);
    }
#if UINTPTR_MAX > 0xFFFFFFFF && !defined(_WIN32)
    // The 64-bit build keeps the game's RAM variables in .gba_ram instead
    // (gen_linker.py). Only gSaveHeader (EWRAM's start) and the interrupt
    // words at IWRAM's end still live here: any other access through a GBA
    // RAM address is stale, so it faults instead of corrupting memory.
    mprotect(PC_EWRAM + 0x1000, PC_EWRAM_SIZE - 0x1000, PROT_NONE);
    mprotect(PC_IWRAM, PC_IWRAM_SIZE - 0x1000, PROT_NONE);
#endif
    PcResetIo();
}

void PcResetIo(void) {
    memset(PC_IO, 0, PC_IO_SIZE);
    PC_REG16(0x130) = 0x3FF; // KEYINPUT: no buttons pressed
    PC_REG16(0x088) = 0x200; // SOUNDBIAS
}
