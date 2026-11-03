// Entry point and frame loop of the PC build.
//
// The game runs its own main loop (AgbMain) and waits for each frame in
// VBlankIntrWait. That is where the platform layer does a frame's work: run the
// VBlank interrupt, draw the 160 lines (with HBlank DMA in between), show the
// frame and read the input.
//
// Testing options (environment variables):
//   TMC_HEADLESS=1           no window
//   TMC_FRAMES=n             exit after n frames
//   TMC_INPUT=frame:keys,... scripted button presses (see ScriptedKeys); the
//                            key "menu" opens the settings menu like Esc
//   TMC_SCREENSHOTS=n,m,...  save frame n as shot_n.bmp
//   TMC_MUTE=1               no sound
//   TMC_GAME=ages|minishcap  skip the game select menu
//   TMC_DUMP_BG=path         dump the room's BG tileset state at exit (see oracle.c)
//   TMC_BIOS=path            gba_bios.bin for the startup animation (bootintro.c)
//   TMC_DUMP_HW=path         dump EWRAM, IWRAM, I/O, palette, VRAM and OAM at exit
//                            (to compare builds, e.g. Linux and Windows)

#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"
#include "global.h"
#if UINTPTR_MAX > 0xFFFFFFFF
#include <sys/mman.h>
#include <ucontext.h>
#endif
#include "main.h"
#include "save.h"

extern void AgbMain(void);

#define INTR_FLAG_VBLANK_BIT 1
#define SCRIPTED_MENU_KEY (1 << 10)

int gPcFrameLimit = -1;
static jmp_buf sResetJump;
static long sFrame;

// Scripted input for testing: TMC_INPUT="frame:keys,frame:keys,..." holds the
// keys (names joined with '+') for 4 frames starting at that frame.
static const char* sInputScript;
static const char* sScreenshots;
static int sHeadless;

static uint16_t KeyMask(const char* name, size_t len) {
    static const char* const names[] = { "a", "b", "select", "start", "right", "left", "up", "down", "r", "l", "menu" };
    size_t i;
    for (i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        if (strlen(names[i]) == len && strncmp(names[i], name, len) == 0)
            return 1 << i;
    }
    PcFatal("unknown key '%.*s' in TMC_INPUT", (int)len, name);
}

static uint16_t ScriptedKeys(long frame) {
    const char* p = sInputScript;
    uint16_t keys = 0;
    while (p != NULL && *p != '\0') {
        long start = strtol(p, (char**)&p, 10);
        if (*p++ != ':')
            break;
        while (*p != '\0' && *p != ',') {
            size_t len = strcspn(p, "+,");
            if (frame >= start && frame < start + 4)
                keys |= KeyMask(p, len);
            p += len;
            if (*p == '+')
                p++;
        }
        if (*p == ',')
            p++;
    }
    return keys;
}

static void RaiseInterrupt(uint16_t flag) {
    void (*handler)(void);

    PC_REG16(0x202) |= flag; // IF
    if (!(PC_REG16(0x208) & 1) || !(PC_REG16(0x200) & flag)) // IME, IE
        return;
    handler = *(void (**)(void))(PC_IWRAM + 0x7FFC); // INTR_VECTOR
    if (handler != NULL)
        handler();
    *(uint16_t*)(PC_IWRAM + 0x7FF8) |= flag; // INTR_CHECK
}

// Writes the GBA hardware memory, in GBA address order, to $TMC_DUMP_HW.
static void DumpHardware(void) {
    const char* path = getenv("TMC_DUMP_HW");
    FILE* f;
    if (path == NULL || (f = fopen(path, "wb")) == NULL)
        return;
    fwrite(PC_EWRAM, PC_EWRAM_SIZE, 1, f);
    fwrite(PC_IWRAM, PC_IWRAM_SIZE, 1, f);
    fwrite(PC_IO, PC_IO_SIZE, 1, f);
    fwrite(PC_PLTT, PC_PLTT_SIZE, 1, f);
    fwrite(PC_VRAM, PC_VRAM_SIZE, 1, f);
    fwrite(PC_OAM, PC_OAM_SIZE, 1, f);
    fclose(f);
}

static int IsScreenshotFrame(long frame) {
    const char* p = sScreenshots;
    while (p != NULL && *p != '\0') {
        if (strtol(p, (char**)&p, 10) == frame)
            return 1;
        if (*p == ',')
            p++;
        else
            break;
    }
    return 0;
}

// Called when the game waits for VBlank. On the GBA, the screen is drawn while
// the game runs a frame's logic, and the game applies its scroll, tilemap and
// VRAM updates right after VBlank. So draw the frame first, with what the game
// set up since the last VBlank, then start the next VBlank.
void PcFrame(void) {
    uint16_t keys;
    int line;

    PcOracleFrame();
    PC_REG16(0x004) &= ~1; // DISPSTAT: not in VBlank
    for (line = 0; line < 160; line++) {
        uint16_t dispstat;
        PC_REG16(0x006) = line;
        dispstat = PC_REG16(0x004);
        if (line == (dispstat >> 8)) { // VCOUNT match (the game runs the sound mixer here)
            PC_REG16(0x004) = dispstat | 4;
            if (dispstat & 0x20)
                RaiseInterrupt(4);
        } else {
            PC_REG16(0x004) = dispstat & ~4;
        }
        PcRenderLine(line);
        PcRunTimedDma(2); // HBlank
    }
    sFrame++;
    if (!sHeadless)
        PcPresent();
    if (IsScreenshotFrame(sFrame)) {
        char path[64];
        snprintf(path, sizeof(path), "shot_%ld.bmp", sFrame);
        if (!PcSaveScreenshot(path))
            fprintf(stderr, "tmc: cannot write %s\n", path);
    }
    if (gPcFrameLimit >= 0 && sFrame >= gPcFrameLimit) {
        PcDumpBgState();
        DumpHardware();
        printf("tmc: ran %ld frames\n", sFrame);
        exit(0);
    }

    keys = ScriptedKeys(sFrame);
    if (!sHeadless)
        keys |= PcPollInput();
    if ((keys & SCRIPTED_MENU_KEY) || PcTakeMenuRequest()) {
        // In a game, the menu also has the save file's options.
        if (gMain.task == TASK_GAME)
            PcSettingsMenu(&gSave.msg_speed, &gSave.brightness);
        else
            PcSettingsMenu(NULL, NULL);
        keys = 0;
    }
    PC_REG16(0x130) = 0x3FF & ~keys; // KEYINPUT is active low

    PC_REG16(0x006) = 160; // VCOUNT: start of VBlank
    PC_REG16(0x004) |= 1;  // DISPSTAT: in VBlank
    PcRunTimedDma(1);
    RaiseInterrupt(INTR_FLAG_VBLANK_BIT);
    PcSaveFlush();
}

void VBlankIntrWait(void) {
    PcFrame();
}

void SoftReset(u32 resetFlags) {
    (void)resetFlags;
    longjmp(sResetJump, 1);
}

static void RunGame(void) {
    setjmp(sResetJump);
    AgbMain();
}

// The game keeps addresses in 32-bit registers and variables (DMA sources and
// destinations, for one), so in the 64-bit build its memory must be below
// 4 GiB. Code and data are linked there; this runs the game on a stack there
// too, on the main thread (where SDL wants its calls).
static void RunGameOnLowStack(void) {
#if UINTPTR_MAX > 0xFFFFFFFF
    enum { STACK_SIZE = 8 << 20 };
    static ucontext_t sMainContext, sGameContext;
    void* stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT | MAP_STACK,
                       -1, 0);
    if (stack == MAP_FAILED)
        PcFatal("cannot map the game's stack");
    getcontext(&sGameContext);
    sGameContext.uc_stack.ss_sp = stack;
    sGameContext.uc_stack.ss_size = STACK_SIZE;
    sGameContext.uc_link = &sMainContext;
    makecontext(&sGameContext, RunGame, 0);
    swapcontext(&sMainContext, &sGameContext);
#else
    RunGame();
#endif
}

int main(int argc, char** argv) {
    const char* frames = getenv("TMC_FRAMES");
    (void)argc;
    (void)argv;

    if (frames != NULL)
        gPcFrameLimit = atoi(frames);
    sInputScript = getenv("TMC_INPUT");
    sScreenshots = getenv("TMC_SCREENSHOTS");
    sHeadless = getenv("TMC_HEADLESS") != NULL;
    // Claim the GBA address ranges first: the graphics drivers SDL loads may
    // allocate memory there (Direct3D does under wine).
    PcEnsureGbaRanges();
    PcMapMemory();
    PcMapRom();
    PcLoadConfig();
    if (!sHeadless)
        PcVideoInit();
    gPcAudioEnabled = !sHeadless && getenv("TMC_MUTE") == NULL;
    PcBootIntro();
    PcInstallOpenBus();
    atexit(PcSaveFlush);
    RunGameOnLowStack();
    return 0;
}
