#ifndef PLATFORM_PC_H
#define PLATFORM_PC_H

// Internal interface of the PC platform layer. Game code does not include this.

#include <stdint.h>
#include <stdio.h>

// The assembler name of the C symbol `name`, for asm labels: a function
// defined under another C name but called by the game as `name`. Windows
// (x86) prefixes C symbols with an underscore.
#define PC_STRINGIFY_(x) #x
#define PC_STRINGIFY(x) PC_STRINGIFY_(x)
#define PC_SYMBOL(name) __asm__(PC_STRINGIFY(__USER_LABEL_PREFIX__) #name)

#define PC_EWRAM ((uint8_t*)0x02000000)
#define PC_IWRAM ((uint8_t*)0x03000000)
#define PC_IO ((uint8_t*)0x04000000)
#define PC_PLTT ((uint8_t*)0x05000000)
#define PC_VRAM ((uint8_t*)0x06000000)
#define PC_OAM ((uint8_t*)0x07000000)

#define PC_EWRAM_SIZE 0x40000
#define PC_IWRAM_SIZE 0x8000
#define PC_IO_SIZE 0x400
#define PC_PLTT_SIZE 0x400
#define PC_VRAM_SIZE 0x18000
#define PC_OAM_SIZE 0x400

#define PC_REG16(offset) (*(volatile uint16_t*)(PC_IO + (offset)))
#define PC_REG32(offset) (*(volatile uint32_t*)(PC_IO + (offset)))

// rom.c
#define PC_ROM_SIZE 0x1000000
void PcMapRom(void);
int PcFindFile(const char* env, const char* name, char* path, size_t size);

// embed.c: the ROM and BIOS built into the executable, or NULL.
#define PC_BIOS_SIZE 0x4000
extern const uint8_t* const gPcEmbeddedRom;  // PC_ROM_SIZE bytes
extern const uint8_t* const gPcEmbeddedBios; // PC_BIOS_SIZE bytes

// memory.c
// Windows: if the GBA address ranges are already taken, runs the game in a
// child process that has them reserved, and exits with its exit code.
void PcEnsureGbaRanges(void);
void PcMapMemory(void);
// Windows: frees the reservation the parent made at addr (PcEnsureGbaRanges),
// so that a file view can be mapped there. Does nothing otherwise.
void PcReleaseReservation(void* addr);
// Maps a read-only copy of size bytes from src at addr. Returns 0 on failure.
int PcMapReadOnlyCopy(void* addr, const void* src, size_t size);
void PcResetIo(void);

// dma.c: runs the timed (VBlank/HBlank) transfers that are enabled.
void PcRunTimedDma(uint32_t timing);

// main.c
void PcFrame(void);
extern int gPcFrameLimit;

// ppu.c
void PcRenderLine(int line);

// video.c
void PcVideoInit(void);
// Report a problem to the player in a message box (and on stderr). PcFatal
// then exits. Headless runs only print it.
__attribute__((format(printf, 1, 2))) void PcError(const char* fmt, ...);
__attribute__((noreturn, format(printf, 1, 2))) void PcFatal(const char* fmt, ...);
void PcPresent(void);
void PcPresentPixels(const uint32_t* pixels); // 240x160 ARGB
int PcSaveScreenshot(const char* path);
int PcSavePixels(const char* path, const uint32_t* pixels);
void PcApplyVideoOptions(void); // fullscreen, filtering and scaling from gPcOptions
struct SDL_Renderer* PcGetRenderer(void); // NULL when headless
void PcRenderPresent(void);               // instead of SDL_RenderPresent
void PcRestoreGameView(void);

// config.c: tmc.cfg, with the controls and these options.
#define PC_MAX_VOLUME 10
typedef struct {
    int volume;       // 0 to PC_MAX_VOLUME
    int fullscreen;
    int smooth;       // linear filtering instead of sharp pixels
    int integerScale; // whole multiples of 240x160 only
} PcOptions;
extern PcOptions gPcOptions;
void PcLoadConfig(void);
void PcSaveConfig(void);
// The path of a file kept between runs (tmc.sav, tmc.cfg), in the user's data
// directory.
const char* PcUserFile(const char* name);

// input.c
#define PC_NUM_BUTTONS 10 // GBA buttons, in KEYINPUT bit order
enum {
    PC_KEY_A = 1 << 0,
    PC_KEY_B = 1 << 1,
    PC_KEY_SELECT = 1 << 2,
    PC_KEY_START = 1 << 3,
    PC_KEY_RIGHT = 1 << 4,
    PC_KEY_LEFT = 1 << 5,
    PC_KEY_UP = 1 << 6,
    PC_KEY_DOWN = 1 << 7,
    PC_KEY_R = 1 << 8,
    PC_KEY_L = 1 << 9,
};
#define PC_PAD_NONE (-1)
#define PC_PAD_LEFT_TRIGGER 100
#define PC_PAD_RIGHT_TRIGGER 101
typedef struct {
    int key; // SDL_Scancode
    int pad; // SDL_GamepadButton or PC_PAD_*
} PcBinding;
extern PcBinding gPcBindings[PC_NUM_BUTTONS];
extern int gPcFastForward;
void PcDefaultBindings(void);
void PcParseBinding(const char* name, char* value);
void PcWriteBindings(FILE* f);
uint16_t PcPollInput(void);
uint16_t PcPollMenuKeys(void);
int PcPollRebind(int button, int* conflict);
int PcTakeMenuRequest(void);
void PcResetBindings(void);
const char* PcKeyName(int button);
const char* PcPadName(int button);

// settings.c
void PcSettingsMenu(uint8_t* msgSpeed, uint8_t* brightness);
void PcDrawOptionsButton(void);

// gameselect.c
void PcGameSelect(void);

// oracle.c
extern int gPcOracleMode;
int PcPreselectedGame(void);
int PcOracleAvailable(void);
void PcDumpBgState(void);
void PcOracleFrame(void);

// arm.c: ARM7TDMI interpreter for the BIOS (bootintro.c)
void ArmReset(const uint8_t* bios);
void ArmRun(int count, uint32_t stopAt);
uint32_t ArmNextPc(void);
void ArmRunTimedDma(int timing);
int ArmFifoSamples(int8_t* out, int count);

// bootintro.c
void PcBootIntro(void);

// audio.c
extern int gPcAudioEnabled;
void PcAudioSubmit(const int8_t* directSound, int count, int frequency);
void PcApplyVolume(void); // from gPcOptions

// openbus.c
void PcInstallOpenBus(void);

// save.c
void PcSaveFlush(void);

#endif // PLATFORM_PC_H
