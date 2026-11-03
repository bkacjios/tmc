.PHONY: default
default:
	@echo do not use this directly

# Native PC build. Compiles the game's C code and ROM data for the host and
# links it against the platform layer in src/platform/pc, which stands in for
# GBA hardware.
#
# PC_BITS=32 (the default) builds for 32-bit x86, where pointers are 4 bytes
# like on the GBA. PC_BITS=64 builds for x86_64; code and data are still
# linked below 4 GiB, so 32-bit addresses in the ROM data stay valid.
#
# PC_OS=windows cross-compiles the 32-bit build for Windows with MinGW-w64
# (i686-w64-mingw32), into a statically linked tmc.exe. SDL3 is built from
# source as a static library for it (build/deps, see below).
#
# PC_OS=s30 cross-compiles the 32-bit build for the PocketGo S30 (Allwinner
# A33: ARMv7 hard-float, Alpine Linux with musl), against the sysroot of the
# pocketgo-s30 build tree ($(S30_WORK)). Game code is compiled with
# arm-none-eabi-gcc (it needs -fno-toplevel-reorder, which clang lacks), the
# platform layer and the link with the tree's clang wrapper. SDL3 is built
# from source as a static library (KMSDRM video, ALSA audio).

include Toolchain.mk

GAME_VERSION ?= USA
PC_BITS ?= 32
PC_OS ?= linux
ifeq ($(PC_OS), windows)
ifneq ($(PC_BITS), 32)
$(error PC_OS=windows needs PC_BITS=32)
endif
BUILD_DIR = build/pcwin_$(GAME_VERSION)
else ifeq ($(PC_OS), s30)
ifneq ($(PC_BITS), 32)
$(error PC_OS=s30 needs PC_BITS=32)
endif
BUILD_DIR = build/pcs30_$(GAME_VERSION)
else ifneq ($(PC_OS), linux)
$(error PC_OS must be linux, windows or s30)
else ifeq ($(PC_BITS), 64)
BUILD_DIR = build/pc64_$(GAME_VERSION)
else ifeq ($(PC_BITS), 32)
BUILD_DIR = build/pc_$(GAME_VERSION)
else
$(error PC_BITS must be 32 or 64)
endif
# Assets are extracted from the baserom by the GBA build and shared.
ASSET_DIR = build/$(GAME_VERSION)/assets

REVISION := 0

ifeq ($(GAME_VERSION), EU)
BUILD_NAME    := tmc_eu
GAME_LANGUAGE := ENGLISH
else ifeq ($(GAME_VERSION), JP)
BUILD_NAME    := tmc_jp
GAME_LANGUAGE := JAPANESE
else ifeq ($(GAME_VERSION), USA)
BUILD_NAME    := tmc
GAME_LANGUAGE := ENGLISH
else ifeq ($(GAME_VERSION), DEMO_USA)
BUILD_NAME    := tmc_demo_usa
GAME_LANGUAGE := ENGLISH
else ifeq ($(GAME_VERSION), DEMO_JP)
BUILD_NAME    := tmc_demo_jp
GAME_LANGUAGE := JAPANESE
else
$(error unknown version $(GAME_VERSION))
endif

# Compiler and assembler flags for the target CPU.
ARCH_FLAGS := -m$(PC_BITS)
AS_ARCH := --$(PC_BITS)
# GNU as for x86 needs the GBA assembly adjusted (see ASM_TO_X86).
ASM_FILTER = $(ASM_TO_X86)

ifeq ($(PC_OS), windows)
EXE = $(BUILD_DIR)/$(BUILD_NAME).exe
MINGW := i686-w64-mingw32
# MSYS2's MINGW32 environment has the binutils without the target prefix.
MINGW_BINUTILS := $(if $(shell which $(MINGW)-objcopy 2>/dev/null),$(MINGW)-)
HOST_CC := $(MINGW)-gcc
HOST_AS := $(MINGW_BINUTILS)as
WINDRES := $(MINGW_BINUTILS)windres
# MinGW names C symbols with a leading underscore on x86. The assembled data
# defines and refers to them without one, so it gets one added.
ASM_FIXUP = && $(MINGW_BINUTILS)objcopy --prefix-symbols=_ $@
SDL_PREFIX := $(CURDIR)/build/deps/$(MINGW)
SDL_LIB := $(SDL_PREFIX)/lib/libSDL3.a
PKG_CONFIG := PKG_CONFIG_LIBDIR=$(SDL_PREFIX)/lib/pkgconfig pkg-config --static
# GCC lays out bitfields like MSVC on Windows by default; the game's structs
# need the GCC (and agbcc) layout.
PC_CFLAGS := -malign-data=abi -mno-ms-bitfields
# A fixed image base, below 4 GiB and above the GBA regions mapped at runtime.
PC_LDFLAGS := -static -Wl,--image-base=0x10000000 -Wl,--disable-dynamicbase
# Runs the built executable (for the menu art): directly when building on
# Windows (MSYS2), else under wine.
ifneq ($(HOST_WINDOWS),)
RUN_EXE :=
else
RUN_EXE := wine
endif
else ifeq ($(PC_OS), s30)
EXE = $(BUILD_DIR)/$(BUILD_NAME)
S30_WORK ?= ../../s30-build
S30_SYSROOT := $(abspath $(S30_WORK)/sysroot)
S30_CLANG := $(abspath $(S30_WORK)/bin/armv7-clang)
HOST_CC := $(TOOLCHAIN_PATH)arm-none-eabi-gcc
HOST_AS := $(TOOLCHAIN_PATH)arm-none-eabi-as
# The platform layer needs a Linux target for the system and SDL headers.
PLATFORM_CC := $(S30_CLANG)
LINK_CC := $(S30_CLANG)
# ARM code only (no Thumb), for openbus.c's instruction decoder.
ARCH_FLAGS := -marm -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
AS_ARCH := -march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard
# The GBA assembly is ARM assembly already.
ASM_FILTER := cat
# The device's musl headers come before the bare-metal (newlib) ones, and
# enums are int-sized as on Linux.
PC_CPPFLAGS := -isystem$(S30_SYSROOT)/usr/include
PC_CFLAGS := -fno-short-enums
SDL_TARGET := armv7-s30
SDL_PREFIX := $(CURDIR)/build/deps/$(SDL_TARGET)
SDL_LIB := $(SDL_PREFIX)/lib/libSDL3.a
PKG_CONFIG := PKG_CONFIG_LIBDIR=$(SDL_PREFIX)/lib/pkgconfig pkg-config --static
# Linked above the GBA address space, as on Linux.
PC_LDFLAGS := -no-pie -Wl,--image-base=0x10000000
# Runs the built executable (for the menu art) under QEMU.
RUN_EXE := qemu-arm-static -L $(S30_SYSROOT)
# The BIOS animation and the logos can be skipped (see below).
BIOS_INTRO_SKIPPABLE ?= 1
LOGOS_SKIPPABLE ?= 1
else
EXE = $(BUILD_DIR)/$(BUILD_NAME)
HOST_CC  ?= gcc
HOST_AS  ?= as
ifeq ($(PC_BITS), 32)
PKG_CONFIG ?= PKG_CONFIG_PATH=/usr/lib32/pkgconfig pkg-config
else
PKG_CONFIG ?= pkg-config
endif
# The executable is linked above the GBA address space: the original ROM is
# mapped at 0x08000000 at runtime, since some data still holds ROM addresses.
PC_LDFLAGS := -no-pie -Wl,-Ttext-segment=0x10000000
PC_CFLAGS := -malign-data=abi
endif
PLATFORM_CC ?= $(HOST_CC)
LINK_CC ?= $(HOST_CC)

.SUFFIXES:
.SECONDARY:
.DELETE_ON_ERROR:
.SECONDEXPANSION:

.PHONY: build clean deps
build: $(EXE) $$(MENU_ART)

# Libraries built from source (Windows and S30: SDL3).
deps: $(SDL_LIB)

clean:
	rm -rf $(BUILD_DIR)

$(ASSET_DIR): build/$(GAME_VERSION)/extracted_assets_$(GAME_VERSION)
build/$(GAME_VERSION)/extracted_assets_$(GAME_VERSION):
	@$(MAKE) -f GBA.mk $@ GAME_VERSION=$(GAME_VERSION)

# ===============
# build ASM files
# ===============

ASINCLUDE := -I $(ASSET_DIR) -I $(BUILD_DIR)/enum_include
ASFLAGS := $(AS_ARCH) --defsym PLATFORM_PC=1 $(if $(filter 64,$(PC_BITS)),--defsym PLATFORM_PC64=1) --defsym $(GAME_VERSION)=1 --defsym REVISION=$(REVISION) --defsym $(GAME_LANGUAGE)=1 $(ASINCLUDE)

ENUM_ASM_SRCS := $(wildcard include/*.h)
ENUM_ASM_HEADERS := $(patsubst include/%.h,$(BUILD_DIR)/enum_include/%.inc,$(ENUM_ASM_SRCS))

# GNU as for ARM and x86 differ in a few places: .align takes a power of two
# on ARM but a byte count on x86, ARM-only directives must go, and x86 splits
# macro arguments on spaces around operators.
ASM_TO_X86 := sed -E 's/^(\s*)\.align\b/\1.p2align/; /^\s*\.(syntax|thumb|arm|thumb_func|code)\b/d; s/ ([-+*]) /\1/g'

$(BUILD_DIR)/data/%.o: deps = $(shell $(SCANINC) -I . $(ASINCLUDE) data/$*.s)
$(BUILD_DIR)/data/%.o: data/%.s $$(deps) $(ENUM_ASM_HEADERS) | $(ASSET_DIR)
	@mkdir -p $(dir $@)
	$(PREPROC) $(BUILD_NAME) $< -- $(ASINCLUDE) | $(ASM_FILTER) | $(HOST_AS) $(ASFLAGS) -o $@ $(ASM_FIXUP)

# Handwritten ARM code is replaced by C in src/platform/pc, but the data tables
# defined in those files are still needed.
ASM_DATA_SRCS := $(wildcard asm/src/*.s)
ASM_DATA_OBJS := $(patsubst %.s,$(BUILD_DIR)/%.data.o,$(ASM_DATA_SRCS))

$(BUILD_DIR)/asm/src/%.data.o: asm/src/%.s tools/pc/asm_data.py $(ENUM_ASM_HEADERS) | $(ASSET_DIR)
	@mkdir -p $(dir $@)
	python3 tools/pc/asm_data.py < $< > $(BUILD_DIR)/asm/src/$*.data.s
	$(PREPROC) $(BUILD_NAME) $(BUILD_DIR)/asm/src/$*.data.s -- $(ASINCLUDE) | $(ASM_FILTER) | $(HOST_AS) $(ASFLAGS) -o $@ $(ASM_FIXUP)

$(BUILD_DIR)/enum_include/%.inc: include/%.h
	@mkdir -p $(dir $@)
	$(ENUM_PROCESSOR) $< $(CC) "-D__attribute__(x)=" "-D$(GAME_VERSION)" "-E" "-nostdinc" "-I$(AGBCC_PATH)" "-I$(AGBCC_PATH)/include" "-iquote include" > $@

# =============
# build C files
# =============

# The 64-bit build places RAM variables in .bss instead of at their GBA
# addresses, since its types with pointers are larger (see ram_commons.py).
ifeq ($(PC_BITS), 64)
RAM_SYMBOLS := $(BUILD_DIR)/ram_symbols.txt
RAM_SYMBOLS_OBJ := $(BUILD_DIR)/ram_symbols.o
GEN_LINKER_FLAGS += --ram-commons=$(BUILD_DIR)/ram_symbols
RAM_COMMONS := | python3 tools/pc/ram_commons.py $(RAM_SYMBOLS)
endif


# Static SDL3, built from the release source (Windows and S30).
SDL_VERSION := 3.4.18
SDL_SRC := build/deps/src/SDL3-$(SDL_VERSION)
ifeq ($(PC_OS), windows)
SDL_TARGET := $(MINGW)
SDL_CMAKE := cmake
SDL_CMAKE_FLAGS := -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=x86 \
	-DCMAKE_C_COMPILER=$(MINGW)-gcc -DCMAKE_CXX_COMPILER=$(MINGW)-g++ -DCMAKE_RC_COMPILER=$(WINDRES) \
	-DCMAKE_FIND_ROOT_PATH=/usr/$(MINGW)
else ifeq ($(PC_OS), s30)
# Console only: KMSDRM with OpenGL ES (the libraries are loaded at runtime),
# ALSA, and evdev input through udev.
SDL_CMAKE := PKG_CONFIG_LIBDIR=$(S30_SYSROOT)/usr/lib/pkgconfig:$(S30_SYSROOT)/usr/share/pkgconfig \
	PKG_CONFIG_SYSROOT_DIR=$(S30_SYSROOT) cmake
SDL_CMAKE_FLAGS := -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=armv7 \
	-DCMAKE_C_COMPILER=$(S30_CLANG) -DCMAKE_CXX_COMPILER=$(S30_CLANG)++ -DCMAKE_SYSROOT=$(S30_SYSROOT) \
	-DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
	-DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=ONLY \
	-DSDL_UNIX_CONSOLE_BUILD=ON -DSDL_X11=OFF -DSDL_WAYLAND=OFF -DSDL_VULKAN=OFF -DSDL_OPENGL=OFF \
	-DSDL_OPENGLES=ON -DSDL_KMSDRM=ON -DSDL_ALSA=ON -DSDL_PULSEAUDIO=OFF -DSDL_PIPEWIRE=OFF \
	-DSDL_JACK=OFF -DSDL_SNDIO=OFF -DSDL_DBUS=OFF -DSDL_IBUS=OFF -DSDL_LIBURING=OFF -DSDL_CAMERA=OFF
endif
ifneq ($(SDL_LIB),)
$(SDL_SRC):
	@mkdir -p build/deps/src
	cd build/deps/src && curl -fsSL -O https://github.com/libsdl-org/SDL/releases/download/release-$(SDL_VERSION)/SDL3-$(SDL_VERSION).tar.gz && tar xzf SDL3-$(SDL_VERSION).tar.gz
$(SDL_LIB): | $(SDL_SRC)
	$(SDL_CMAKE) -S $(SDL_SRC) -B build/deps/sdl3-build-$(SDL_TARGET) -G Ninja $(SDL_CMAKE_FLAGS) \
		-DCMAKE_BUILD_TYPE=Release -DSDL_SHARED=OFF -DSDL_STATIC=ON \
		-DSDL_TESTS=OFF -DSDL_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=$(SDL_PREFIX)
	cmake --build build/deps/sdl3-build-$(SDL_TARGET)
	cmake --install build/deps/sdl3-build-$(SDL_TARGET)
endif

SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl3)
SDL_LIBS   := $(shell $(PKG_CONFIG) --libs sdl3)

CINCLUDE := -I include -I build/$(GAME_VERSION)
CPPFLAGS := $(ARCH_FLAGS) $(PC_CPPFLAGS) -std=gnu11 $(CINCLUDE) -DPLATFORM_PC -DNON_MATCHING=1 -D$(GAME_VERSION) -DREVISION=$(REVISION) -D$(GAME_LANGUAGE)
ifeq ($(PC_OS), s30)
CPPFLAGS += -DPC_S30
endif
# LOGOS_SKIPPABLE=1: A or Start skips the Nintendo/Capcom logos on the first
# boot too, not only after a soft reset (title.c).
ifeq ($(LOGOS_SKIPPABLE), 1)
CPPFLAGS += -DPC_LOGOS_SKIPPABLE
endif
# -funsigned-char: char is unsigned on ARM.
# -fpermissive: the decompiled code relies on pre-C99 leniency.
PC_OPT ?= -O2
CFLAGS := $(ARCH_FLAGS) -std=gnu11 $(PC_OPT) -g -fno-toplevel-reorder -fno-merge-constants -fpermissive -funsigned-char -fwrapv -fno-strict-aliasing -fcommon -fno-pie -w $(PC_CFLAGS)

PLATFORM_SRCS := $(wildcard src/platform/pc/*.c)
PLATFORM_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(PLATFORM_SRCS))
$(PLATFORM_OBJS): CPPFLAGS += $(SDL_CFLAGS)
# BIOS_INTRO_SKIPPABLE=1: A or Start skips the BIOS startup animation (bootintro.c).
ifeq ($(BIOS_INTRO_SKIPPABLE), 1)
$(PLATFORM_OBJS): CPPFLAGS += -DPC_BIOS_INTRO_SKIPPABLE
endif
# -fno-common: the platform layer's variables stay out of the game's RAM
# (.gba_ram, made of common symbols, in the 64-bit build).
$(PLATFORM_OBJS): CFLAGS := $(filter-out -w,$(CFLAGS)) -Wall -Wno-multichar -fno-common
# The platform layer is compiled with PLATFORM_CC (clang for the S30), without
# the GCC-only flags.
ifneq ($(PLATFORM_CC), $(HOST_CC))
$(PLATFORM_OBJS): HOST_CC := $(PLATFORM_CC)
$(PLATFORM_OBJS): CFLAGS := $(filter-out -fno-toplevel-reorder -fpermissive -fno-merge-constants,$(CFLAGS))
endif

# EMBED_ROM=1: builds baserom.gba, and gba_bios.bin if it is there, into the
# executable (embed.c), so it runs without them. Such an executable contains
# Nintendo's ROM and BIOS: it is for personal use, not for distribution.
EMBED_OBJ := $(BUILD_DIR)/src/platform/pc/embed.o
ifeq ($(EMBED_ROM), 1)
EMBED_FILES := baserom.gba $(wildcard gba_bios.bin)
EMBED_CPPFLAGS := -DPC_EMBED_ROM='"$(CURDIR)/baserom.gba"' $(if $(wildcard gba_bios.bin),-DPC_EMBED_BIOS='"$(CURDIR)/gba_bios.bin"')
endif
# Compiled directly: the preprocessed file would hold the ROM as text.
$(EMBED_OBJ): src/platform/pc/embed.c src/platform/pc/pc.h $(EMBED_FILES)
	@mkdir -p $(dir $@)
	$(HOST_CC) $(CPPFLAGS) $(EMBED_CPPFLAGS) $(CFLAGS) -c $< -o $@

# Structs are laid out like agbcc does (see tools/pc/agbcc_layout.py).
$(BUILD_DIR)/src/%.o: deps = $(shell $(SCANINC) $(CINCLUDE) src/$*.c)
$(BUILD_DIR)/src/%.o: src/%.c $$(deps) tools/pc/agbcc_layout.py $(if $(RAM_SYMBOLS),tools/pc/ram_commons.py $(RAM_SYMBOLS)) | $(ASSET_DIR)
	@mkdir -p $(dir $@)
	$(HOST_CC) -E $(CPPFLAGS) $< | python3 tools/pc/agbcc_layout.py $(if $(filter src/platform/%,$<),,$(RAM_COMMONS)) > $(BUILD_DIR)/src/$*.i
	$(HOST_CC) $(CFLAGS) -x cpp-output -c $(BUILD_DIR)/src/$*.i -o $@

# ==============
# link
# ==============

# Objects replaced by the platform layer.
PC_EXCLUDED_OBJS := src/eeprom.o

$(BUILD_DIR)/linker.ld.i: linker.ld
	@mkdir -p $(BUILD_DIR)
	$(HOST_CC) -E -P -x c -D$(GAME_VERSION) -DREVISION=$(REVISION) -D$(GAME_LANGUAGE) $< -o $@

$(BUILD_DIR)/pc.ld $(BUILD_DIR)/objects.txt $(RAM_SYMBOLS) $(RAM_SYMBOLS_OBJ:.o=.s) &: $(BUILD_DIR)/linker.ld.i tools/pc/gen_linker.py
	python3 tools/pc/gen_linker.py $(GEN_LINKER_FLAGS) $(if $(filter windows,$(PC_OS)),--pe) $< $(BUILD_DIR)/pc.ld $(BUILD_DIR)/objects.txt $(PC_EXCLUDED_OBJS)

$(RAM_SYMBOLS_OBJ): $(RAM_SYMBOLS_OBJ:.o=.s)
	$(HOST_AS) --$(PC_BITS) -o $@ $<

ifeq ($(PC_OS), windows)
# The exe's icon is drawn from the title screen (tools/pc/make_icon.py), so the
# game is linked twice: first without it, as title.exe, which is run for the
# icon and the menu art, then with it.
TITLE_EXE := $(BUILD_DIR)/title.exe
ICON_OBJ := $(BUILD_DIR)/icon.o
else
TITLE_EXE = $(EXE)
endif

LINK_OBJS = $(BUILD_DIR)/pc.ld $(BUILD_DIR)/objects.txt $$(addprefix $(BUILD_DIR)/,$$(objs)) $(PLATFORM_OBJS) $(ASM_DATA_OBJS) $$(ORACLE_OBJS) $(RAM_SYMBOLS_OBJ)
LINK = cd $(BUILD_DIR) && $(LINK_CC) $(ARCH_FLAGS) $(PC_LDFLAGS) -o $(notdir $@) -Wl,-T,pc.ld -Wl,-Map,$(notdir $(basename $@)).map \
	$(objs) $(patsubst $(BUILD_DIR)/%,%,$(PLATFORM_OBJS) $(ASM_DATA_OBJS) $(ORACLE_OBJS) $(RAM_SYMBOLS_OBJ) $(filter $(ICON_OBJ),$^)) $(SDL_LIBS) -lm

$(EXE) $(TITLE_EXE): objs = $(shell cat $(BUILD_DIR)/objects.txt 2>/dev/null)
$(TITLE_EXE): $(LINK_OBJS)
	$(LINK)
ifeq ($(PC_OS), windows)
$(EXE): $(LINK_OBJS) $(ICON_OBJ)
	$(LINK)

$(BUILD_DIR)/icon.ico: $(BUILD_DIR)/title_hw.bin tools/pc/make_icon.py
	python3 tools/pc/make_icon.py $< $@

$(ICON_OBJ): $(BUILD_DIR)/icon.ico
	echo '1 ICON "icon.ico"' > $(BUILD_DIR)/icon.rc
	cd $(BUILD_DIR) && $(WINDRES) icon.rc -O coff -o $(notdir $@)
endif

# ==============
# game select menu art
# ==============

# Title screens shown in the game select menu (src/platform/pc/gameselect.c).
# Minish Cap's is a screenshot of its own title screen. The Oracle ones are
# composed from oracles-disasm, if it is there.
ORACLES_DISASM ?= ../oracles-disasm
MENU_DIR := $(BUILD_DIR)/menu
MENU_ART := $(MENU_DIR)/minishcap.bmp
ORACLE_ART := $(MENU_DIR)/ages.bmp $(MENU_DIR)/seasons.bmp
ORACLE_OBJS :=
ifneq ($(wildcard $(ORACLES_DISASM)/data/ages/gfxHeaders.s),)
MENU_ART += $(ORACLE_ART)
ORACLE_OBJS += $(BUILD_DIR)/oracle/lynna.o
endif

# Oracle of Ages proof of concept: Lynna City converted to Minish Cap tiles
# (src/platform/pc/oracle.c).
$(BUILD_DIR)/oracle/lynna.c: tools/pc/oracle_map.py tools/pc/oracle_lynna_tiles.py tools/pc/gbc_gfx.py
	@mkdir -p $(dir $@)
	python3 tools/pc/oracle_map.py $(ORACLES_DISASM) $@

$(BUILD_DIR)/oracle/lynna.o: $(BUILD_DIR)/oracle/lynna.c
	$(HOST_CC) $(ARCH_FLAGS) -c $< -o $@

$(ORACLE_ART): $(MENU_DIR)/%.bmp: tools/pc/oracle_title.py tools/pc/gbc_gfx.py
	@mkdir -p $(MENU_DIR)
	python3 tools/pc/oracle_title.py $(ORACLES_DISASM) $* $@

# Frame 420 is the finished title screen. The hardware dump is for the icon.
$(MENU_DIR)/minishcap.bmp $(BUILD_DIR)/title_hw.bin &: $(TITLE_EXE)
	@mkdir -p $(MENU_DIR)
	cd $(MENU_DIR) && TMC_HEADLESS=1 TMC_FRAMES=420 TMC_SCREENSHOTS=420 TMC_DUMP_HW=../title_hw.bin $(RUN_EXE) ../$(notdir $(TITLE_EXE)) > /dev/null && mv shot_420.bmp minishcap.bmp && rm -f tmc.sav
