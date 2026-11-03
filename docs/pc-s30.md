# PocketGo S30 build

`make pc-s30` cross-compiles the 32-bit PC port for the PocketGo S30
(Allwinner A33: ARMv7 hard-float, Alpine Linux with musl), into
`build/pcs30_USA/tmc`. It needs the pocketgo-s30 build tree
(github.com/bkacjios/pocketgo-s30-cfw): its Alpine sysroot and `armv7-clang`
wrapper, found at `S30_WORK` (default `../../s30-build`, as a submodule in
`ports/tmc`).

Host requirements: `arm-none-eabi-gcc`, cmake, ninja, pkg-config and
`qemu-arm-static` (the menu art is made by running the built game headless).

## How it is built

- Game code: `arm-none-eabi-gcc` (it needs `-fno-toplevel-reorder`, which clang
  lacks), against the device's musl headers, with `-fno-short-enums`. ARM code
  only, no Thumb.
- Platform layer (`src/platform/pc`) and the link: the tree's clang wrapper and
  lld, linked at 0x10000000 above the GBA address ranges, as on Linux.
- Data: the GBA assembly is assembled as it is with `arm-none-eabi-as`.
- SDL3: built from the release source as a static library
  (`build/deps/armv7-s30`): KMSDRM video with OpenGL ES, ALSA audio, evdev
  input through udev. The system libraries are loaded at runtime.

## Differences from the PC build (`PC_S30`)

- Video: the panel is portrait (320x480), mounted landscape. Everything is
  drawn to a 480x320 view (2x), turned a quarter turn counterclockwise onto the
  panel when presented (`PcRenderPresent`). Always fullscreen.
- Input: the face buttons are in Nintendo's layout, so A is the east button and
  B the south one; the settings menu shows the S30's button names. The settings
  menu opens from the OPTIONS button of the pause menu's Quest screen.
- Open bus (`openbus.c`): reads through NULL pointers are emulated by decoding
  the faulting ARM load or store (LDR/STR, halfword and signed forms, LDRD/STRD,
  LDM/STM).
- `BIOS_INTRO_SKIPPABLE` and `LOGOS_SKIPPABLE` are on by default.

## Device

Copy `tmc` (strip it with `llvm-strip`), `baserom.gba`, `gba_bios.bin` and
`menu/` to `/mnt/sd/home/ports/tmc` on the card. Launcher entry,
`/mnt/sd/apps/The Minish Cap.sh`:

```sh
#!/bin/sh
cd /mnt/sd/home/ports/tmc && exec ./tmc
```

Saves and settings are kept in `~/.local/share/tmc`. The system menu's Exit
sends SIGTERM, which SDL turns into a quit event: the game exits and saves.
