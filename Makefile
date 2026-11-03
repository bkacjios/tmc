.PHONY: default all
default: build
all: eu jp usa demo_jp demo_usa

MAKEFLAGS += --no-print-directory

.PHONY: build eu jp usa demo_jp demo_usa custom
build: GAME_VERSION ?=USA
build: tools
	@$(MAKE) -f GBA.mk build GAME_VERSION=$(GAME_VERSION)

eu: GAME_VERSION=EU
jp: GAME_VERSION=JP
usa: GAME_VERSION=USA
demo_jp: GAME_VERSION=DEMO_JP
demo_usa: GAME_VERSION=DEMO_USA
eu jp usa demo_jp demo_usa: tools
	@$(MAKE) GAME_VERSION=$(GAME_VERSION)

custom: tools
	@$(MAKE) GAME_VERSION=USA CUSTOM=1

# Native PC build (see PC.mk). The object list comes from linker.ld, so it is
# generated in a first pass.
.PHONY: pc clean-pc
pc: GAME_VERSION ?=USA
pc: tools
	@$(MAKE) -f PC.mk build/pc_$(GAME_VERSION)/objects.txt GAME_VERSION=$(GAME_VERSION)
	@$(MAKE) -f PC.mk build GAME_VERSION=$(GAME_VERSION)

# 64-bit build (work in progress), in build/pc64_*.
.PHONY: pc64
pc64: GAME_VERSION ?=USA
pc64: tools
	@$(MAKE) -f PC.mk build/pc64_$(GAME_VERSION)/objects.txt GAME_VERSION=$(GAME_VERSION) PC_BITS=64
	@$(MAKE) -f PC.mk build GAME_VERSION=$(GAME_VERSION) PC_BITS=64

# 32-bit Windows build (MinGW-w64 cross-compile, static), in build/pcwin_*.
.PHONY: pc-win
pc-win: GAME_VERSION ?=USA
pc-win: tools
	@$(MAKE) -f PC.mk deps GAME_VERSION=$(GAME_VERSION) PC_OS=windows
	@$(MAKE) -f PC.mk build/pcwin_$(GAME_VERSION)/objects.txt GAME_VERSION=$(GAME_VERSION) PC_OS=windows
	@$(MAKE) -f PC.mk build GAME_VERSION=$(GAME_VERSION) PC_OS=windows

# PocketGo S30 build (ARMv7 cross-compile against the pocketgo-s30 sysroot,
# S30_WORK), in build/pcs30_*. See docs/pc-s30.md.
.PHONY: pc-s30
pc-s30: GAME_VERSION ?=USA
pc-s30: tools
	@$(MAKE) -f PC.mk deps GAME_VERSION=$(GAME_VERSION) PC_OS=s30
	@$(MAKE) -f PC.mk build/pcs30_$(GAME_VERSION)/objects.txt GAME_VERSION=$(GAME_VERSION) PC_OS=s30
	@$(MAKE) -f PC.mk build GAME_VERSION=$(GAME_VERSION) PC_OS=s30

clean-pc:
	@$(MAKE) -f PC.mk clean

.PHONY: extract_assets
extract_assets: tools
	@$(MAKE) -f GBA.mk extract_assets

# On Windows (MSYS2), the tools are built natively into tools/windows, so that
# a checkout shared with Linux keeps both sets (TOOLS_DIR in Toolchain.mk).
ifneq ($(filter MINGW% MSYS% CYGWIN%,$(shell uname -s)),)
TOOLS_DIR := tools/windows
TOOLS_BUILD := tools/cmake-build-windows
# MSYS2's MINGW32 compiler is 32-bit, where the fetched nlohmann_json warns.
TOOLS_CMAKE_FLAGS := -DTOOLS_WARNINGS_AS_ERRORS=OFF
else
TOOLS_DIR := tools
TOOLS_BUILD := tools/cmake-build
endif

.PHONY: tools
tools: $(TOOLS_DIR)/bin

# CMAKE_POLICY_VERSION_MINIMUM: CMake 4 refuses the fetched nlohmann_json,
# which asks for compatibility with versions before 3.5.
$(TOOLS_DIR)/bin:
	mkdir -p $(TOOLS_BUILD)
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5 $(TOOLS_CMAKE_FLAGS) -DCMAKE_INSTALL_PREFIX=$(TOOLS_DIR) -S tools -B $(TOOLS_BUILD)
	cmake --build $(TOOLS_BUILD) -j --target install

.PHONY: clean clean-tools
clean:
	@$(MAKE) -f GBA.mk clean

clean-tools:
	rm -rf $(TOOLS_DIR)/bin
	rm -rf $(TOOLS_BUILD)
