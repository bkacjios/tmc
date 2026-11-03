// The GBA's startup animation (the logo and chime), played by running the
// BIOS, gba_bios.bin, in the ARM interpreter (arm.c) before the game starts.
//
// The BIOS is found like the ROM: $TMC_BIOS, ./gba_bios.bin, or gba_bios.bin
// two levels above the executable, unless it is built in (embed.c). Without
// it the game starts right away.
// The machine runs scanline by scanline, drawn by the PPU (ppu.c) and with
// the sound registers and FIFO A's DMA stream mixed by audio.c. It stops when
// the BIOS jumps to the cartridge; then the hardware is reset, so the game
// starts as it does without the animation.
//
// Build with BIOS_INTRO_SKIPPABLE=1 (PC_BIOS_INTRO_SKIPPABLE) to let A or
// Start skip it, like the game's Nintendo/Capcom logos. Headless, it only plays when TMC_BIOS is set (for tests);
// TMC_BIOS_SHOTS=n then saves every n-th frame as bios_<frame>.bmp.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"

#define CART_ENTRY 0x08000000
#define MAX_FRAMES 900 // safety net: the animation takes about 3 seconds
// The GBA runs 1232 cycles per scanline, 960 of them before HBlank; the BIOS
// mostly runs 1-cycle instructions from its own 32-bit ROM, so this
// approximates them with 1 instruction per 2 cycles.
#define INSTRUCTIONS_VISIBLE 480
#define INSTRUCTIONS_HBLANK 136
#define CPU_HZ 16777216.0
#define FRAME_HZ (CPU_HZ / 280896.0)

extern uint32_t gPcFramebuffer[160][240];

static int LoadBios(uint8_t* bios) {
    char path[4096];
    FILE* f;
    size_t n;
    if (gPcEmbeddedBios != NULL) {
        memcpy(bios, gPcEmbeddedBios, PC_BIOS_SIZE);
        return 1;
    }
    if (!PcFindFile("TMC_BIOS", "gba_bios.bin", path, sizeof(path)) || (f = fopen(path, "rb")) == NULL)
        return 0;
    n = fread(bios, 1, PC_BIOS_SIZE, f);
    fclose(f);
    if (n != PC_BIOS_SIZE) {
        PcError("%s is not a 16 KiB GBA BIOS", path);
        return 0;
    }
    return 1;
}

static void RaiseIrq(uint16_t flag) {
    PC_REG16(0x202) |= flag;
}

// FIFO A's samples for one frame, at the rate timer 0 sets, mixed with the
// PSG channels by audio.c.
static void SubmitAudio(double* pending) {
    static const int sPrescale[4] = { 1, 64, 256, 1024 };
    static int8_t samples[2048];
    uint16_t timer = PC_REG16(0x102);
    int frequency = 32768, count;

    if (timer & 0x80) {
        uint32_t ticks = (0x10000 - PC_REG16(0x100)) * sPrescale[timer & 3];
        frequency = (int)(CPU_HZ / ticks);
    }
    *pending += frequency / FRAME_HZ;
    count = (int)*pending;
    if (count > (int)sizeof(samples))
        count = sizeof(samples);
    *pending -= count;
    if (!ArmFifoSamples(samples, count))
        memset(samples, 0, count);
    PcAudioSubmit(samples, count, frequency);
}

static void RunFrame(double* pendingSamples) {
    int line;
    for (line = 0; line < 228; line++) {
        uint16_t dispstat = PC_REG16(0x004) & ~7;
        PC_REG16(0x006) = line;
        if (line >= 160 && line < 227)
            dispstat |= 1;
        if (line == (dispstat >> 8)) {
            dispstat |= 4;
            if (dispstat & 0x20)
                RaiseIrq(4);
        }
        PC_REG16(0x004) = dispstat;
        if (line == 160) {
            if (dispstat & 0x08)
                RaiseIrq(1);
            ArmRunTimedDma(1);
            SubmitAudio(pendingSamples);
        }

        ArmRun(INSTRUCTIONS_VISIBLE, CART_ENTRY);
        if (ArmNextPc() == CART_ENTRY)
            return;

        PC_REG16(0x004) |= 2; // HBlank
        if (line < 160) {
            PcRenderLine(line);
            ArmRunTimedDma(2);
        }
        if (PC_REG16(0x004) & 0x10)
            RaiseIrq(2);
        ArmRun(INSTRUCTIONS_HBLANK, CART_ENTRY);
        if (ArmNextPc() == CART_ENTRY)
            return;
    }
}

// Back to the state the game starts from without the animation.
static void ResetMachine(void) {
    memset(PC_EWRAM, 0, PC_EWRAM_SIZE);
    memset(PC_IWRAM, 0, PC_IWRAM_SIZE);
    memset(PC_PLTT, 0, PC_PLTT_SIZE);
    memset(PC_VRAM, 0, PC_VRAM_SIZE);
    memset(PC_OAM, 0, PC_OAM_SIZE);
    PcResetIo();
}

void PcBootIntro(void) {
#if UINTPTR_MAX > 0xFFFFFFFF
    // The 64-bit build keeps most of EWRAM inaccessible (memory.c), which the
    // BIOS clears; the animation is for the 32-bit builds.
    return;
#else
    static uint8_t bios[PC_BIOS_SIZE];
    int headless = PcGetRenderer() == NULL;
    const char* shots = getenv("TMC_BIOS_SHOTS");
    int shotEvery = shots != NULL ? atoi(shots) : 0;
    double pendingSamples = 0;
#ifdef PC_BIOS_INTRO_SKIPPABLE
    uint16_t prevKeys = 0;
#endif
    int frame;

    if ((headless && getenv("TMC_BIOS") == NULL) || !LoadBios(bios))
        return;

    ResetMachine();
    PC_REG16(0x130) = 0x3FF; // KEYINPUT: nothing pressed
    ArmReset(bios);
    for (frame = 1; frame <= MAX_FRAMES; frame++) {
        RunFrame(&pendingSamples);
        if (ArmNextPc() == CART_ENTRY)
            break;
        if (shotEvery > 0 && frame % shotEvery == 0) {
            char path[64];
            snprintf(path, sizeof(path), "bios_%d.bmp", frame);
            PcSaveScreenshot(path);
        }
        if (!headless) {
            uint16_t keys;
            PcPresent();
            keys = PcPollInput();
            PcTakeMenuRequest(); // Esc does not open the settings menu here
#ifdef PC_BIOS_INTRO_SKIPPABLE
            // Like the game's logos: a new press of A or Start.
            if (keys & ~prevKeys & (PC_KEY_A | PC_KEY_START))
                break;
            prevKeys = keys;
#else
            (void)keys;
#endif
        }
    }
    if (frame > MAX_FRAMES)
        fprintf(stderr, "tmc: the BIOS did not start the game in %d frames\n", MAX_FRAMES);
    ResetMachine();
#endif
}
