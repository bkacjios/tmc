# Windows build (32-bit)

`make pc-win` cross-compiles the 32-bit PC build for Windows with MinGW-w64
and writes `build/pcwin_USA/tmc.exe`, a statically linked executable.

## Requirements (on the Linux build machine)

- MinGW-w64 for i686: `i686-w64-mingw32-gcc`, `-as`, `-objcopy`
  (Arch: `mingw-w64-gcc`, `mingw-w64-binutils`).
- `cmake`, `ninja` and `curl`, to build SDL3 the first time.
- `wine`, to run the exe once at build time for the menu art
  (`menu/minishcap.bmp`), and for testing.

## Building on Windows (MSYS2)

`make pc-win` also runs natively, from the MSYS2 **MINGW32** shell:

- Packages: `pacman -S --needed make python mingw-w64-i686-gcc
  mingw-w64-i686-cmake mingw-w64-i686-ninja mingw-w64-i686-pkgconf
  mingw-w64-i686-libpng python-pycparser` (libpng for `gbagfx`, pycparser
  for `extract_include_enum.py`).
  `EMBED_ROM=1` needs GCC 15 or later (C23 `#embed`).
- devkitARM, for the ARM preprocessor that `extract_include_enum.py` runs:
  `export DEVKITARM=/c/devkitPro/devkitARM`.
- The assets extracted from the ROM (`build/<version>/assets`), as for Linux.

When `uname -s` names MSYS2 (`MINGW*`, `MSYS*`), Make:

- Builds the tools (`scaninc`, `preproc`, ...) natively into `tools/windows`
  (build dir `tools/cmake-build-windows`), so that a checkout shared with
  Linux keeps its `tools/bin` (`TOOLS_DIR`, `Makefile` and `Toolchain.mk`).
- Runs `title.exe` directly instead of under wine.
- Uses the binutils without the `i686-w64-mingw32-` prefix if the prefixed
  ones are missing, as in MSYS2 (`MINGW_BINUTILS`, `PC.mk`).

SDL3 is not installed system-wide. `make pc-win` first runs `PC.mk deps`,
which downloads the SDL3 release source (`SDL_VERSION` in `PC.mk`, 3.4.18) and
builds it as a static library into `build/deps/i686-w64-mingw32`. Later builds
reuse it.

## What the exe needs at runtime

- Only system DLLs and the Universal CRT (`api-ms-win-crt-*`), which ships with
  Windows 10 and later. Check with
  `i686-w64-mingw32-objdump -p build/pcwin_USA/tmc.exe | grep "DLL Name"`.
- The ROM, like the Linux build: `$TMC_ROM`, `baserom.gba` in the working
  directory, or `baserom.gba` two directories above the exe (`src/platform/pc/rom.c`).
- Optional: `menu/*.bmp` next to the exe (game select art, `gameselect.c`).
- Optional: `gba_bios.bin`, found like the ROM (or `$TMC_BIOS`). With it, the GBA
  startup animation plays first: the BIOS runs in an ARM interpreter
  (`arm.c`, `bootintro.c`). Build with `BIOS_INTRO_SKIPPABLE=1` to let A or
  Start skip it, like the logos.
- Build with `LOGOS_SKIPPABLE=1` to let A or Start skip the Nintendo/Capcom
  logos on the first boot. The game itself only allows it after a soft reset
  (`GetAdvanceState` in `title.c`). Make does not notice a changed option:
  rebuild the affected object (`title.o`, `bootintro.o`) when switching.
- Build with `EMBED_ROM=1` to put `baserom.gba`, and `gba_bios.bin` if it is in
  the repository root, into the exe (`embed.c`, with C23 `#embed`). It then
  needs neither file and ignores `$TMC_ROM`/`$TMC_BIOS`. The exe grows by
  16 MiB. Such an exe contains Nintendo's ROM and BIOS: it is for personal use,
  not for distribution. Like the other options, Make does not notice a
  changed `EMBED_ROM`: clean or remove `embed.o` when switching. It also works
  for the Linux builds.
- The save (`tmc.sav`) and settings (`tmc.cfg`) go in `%APPDATA%\tmc\`
  (`~/.local/share/tmc/` on Linux; `PcUserFile`, `config.c`). Headless runs
  keep using the working directory. A file left in the working directory by an
  older build is copied over on first use.
- It is a GUI-subsystem program (`-mwindows` from SDL), so it opens no console.
  Errors meant for the player (`PcError`/`PcFatal`, `video.c`), such as a
  missing ROM or a failed save, are shown in a message box on all platforms,
  and printed to stderr. Headless runs only print them. Debug and test
  messages only go to stderr.
- The exe is about 40 MB because of debug info. `i686-w64-mingw32-strip` makes a
  smaller copy to send.

## How the port works

- `PC.mk`, `PC_OS=windows` (32-bit only): MinGW toolchain, build dir
  `build/pcwin_<version>`, `tmc.exe`, static link, image base 0x10000000 with
  ASLR off (`--disable-dynamicbase`). Like the Linux build's
  `-Ttext-segment`, this keeps the program above the GBA regions that are mapped at
  runtime (0x02000000-0x09000000).
- `-mno-ms-bitfields`: MinGW lays out bitfields like MSVC by default, which
  breaks the game's structs (for example, `Entity` grows from 0x68 to 0x74 bytes).
  The Makefile does not track flags, so after changing `CFLAGS`, clean
  (`make -f PC.mk clean PC_OS=windows`). Otherwise stale objects keep the old
  layout.
- Symbol names: on i686, Windows names C symbols with a leading underscore. The
  assembled data (`data/*.o`, `asm/src/*.data.o`) gets one via
  `objcopy --prefix-symbols=_` (`ASM_FIXUP`). `gen_linker.py --pe` writes the
  RAM and other script symbols with one. Platform functions that the game calls
  under another name use `PC_SYMBOL(name)` (`pc.h`) instead of
  `__asm__("name")`.
- `gen_linker.py --pe`: C read-only data is in `.rdata` on PE, and the
  `.tmc_rom` output section starts on the section alignment (a page). An
  unaligned section loads at the wrong place.
- Icon: `tools/pc/make_icon.py` draws the title screen logo (its sprites, on
  a transparent background) from a `TMC_DUMP_HW` dump at frame 420, into
  `icon.ico` (256, 48, 32 and 16 px). Since that needs the game to run, it is
  linked twice: `title.exe` without the icon (run under wine for the dump and
  the menu art), then `tmc.exe` with `icon.o` (windres). SDL uses the exe's
  icon for the window too.
- `openbus.c`: the NULL-page read emulation works on Windows too, through a
  vectored exception handler. The decoder accesses registers through
  `Machine`/`Reg()`, which map to `CONTEXT` on Windows and `mcontext_t` on Linux.

## Testing

`TMC_DUMP_HW=path` (new, `main.c`) writes EWRAM, IWRAM, I/O, palette, VRAM and
OAM when the run reaches its frame limit (`TMC_FRAMES`). In the 32-bit builds,
RAM variables are at their GBA addresses, so two builds' dumps can be compared
byte for byte. Only pointers into the executables should differ.

Verified on 2026-10-17 under wine: with the scripted input from `docs/pc64.md`
(title screen, file select, name entry), screenshots every 100 frames up to
frame 6000, the save file and the hardware/RAM dump match the Linux 32-bit
build. The menu art is identical too.

Windowed (not headless) under wine: the title screen renders correctly, with
sound on and no errors. This needed the GBA memory regions to be mapped before
`PcVideoInit()` (`main.c`): under wine, Direct3D setup took memory at
0x02000000 first, and startup failed with `tmc: cannot map EWRAM at 02000000`.
Headless runs skip video setup, which is why they worked.

Verified on 2026-10-17 under wine: an `EMBED_ROM=1` exe copied alone into an
empty directory runs, matches the Linux build (screenshot and dump at frame
1500), and plays the BIOS intro.

A first test on real Windows exited at once with no window. A probable cause,
fixed since: `openbus.c`'s vectored exception handler aborted on every access
violation, including those that system DLLs and graphics drivers raise and
handle themselves. It now passes faults it cannot emulate on, and a filter
reports real crashes with `PcFatal`. Not confirmed on real Windows yet.

Not tested yet: gamepads.

On real Windows 11 (64-bit, build 26300), the exe failed with
`cannot map EWRAM at 02000000`: WoW64 maps large sections low while a 32-bit
process starts, before any of its code runs (in a plain `SysWOW64\cmd.exe`, one
of 32 MiB at 0x00A80000-0x02A80000). A process created suspended has none of
them yet. So when `main` finds a GBA range taken, `PcEnsureGbaRanges`
(`memory.c`) starts the exe again suspended, reserves the ranges in it, resumes
it and exits with its exit code. The child (`TMC_GBA_RESERVED` set) commits
inside the reservations; `rom.c` releases the ROM one just before mapping the
file view there.
