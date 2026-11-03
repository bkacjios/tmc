# ========
# compiler
# ========

ifndef TOOLCHAIN_PATH
ifneq (,$(shell which arm-none-eabi-gcc))
TOOLCHAIN_PATH :=
else ifdef DEVKITARM
TOOLCHAIN_PATH := $(DEVKITARM)/bin/
else
$(error arm-none-eabi-gcc not found, please install (devkitPro supported))
endif
endif

# ensure trailing slash
ifneq ($(TOOLCHAIN_PATH),)
override TOOLCHAIN_PATH:=$(TOOLCHAIN_PATH)/
endif

CC := $(TOOLCHAIN_PATH)arm-none-eabi-gcc
CPP := $(CC) -E
CXX := $(TOOLCHAIN_PATH)arm-none-eabi-g++
AS := $(TOOLCHAIN_PATH)arm-none-eabi-as
LD := $(TOOLCHAIN_PATH)arm-none-eabi-ld
OBJCOPY := $(TOOLCHAIN_PATH)arm-none-eabi-objcopy

# ============
# custom tools
# ============

AGBCC_PATH ?= $(shell pwd)/tools/agbcc

CC1 := $(AGBCC_PATH)/bin/agbcc
SHA1 := $(shell { command -v sha1sum || command -v shasum; } 2>/dev/null) -c

# Non-empty when make runs on Windows (MSYS2).
HOST_WINDOWS := $(filter MINGW% MSYS% CYGWIN%,$(shell uname -s))

# Built by the top-level Makefile (target tools). On Windows (MSYS2), they go
# in tools/windows, so that a checkout shared with Linux keeps both sets.
ifneq ($(HOST_WINDOWS),)
TOOLS_DIR := tools/windows
else
TOOLS_DIR := tools
endif

SCANINC := $(TOOLS_DIR)/bin/scaninc
PREPROC := $(TOOLS_DIR)/bin/preproc
ASSET_PROCESSOR := $(TOOLS_DIR)/bin/asset_processor
ENUM_PROCESSOR := tools/extract_include_enum.py
FIX := $(TOOLS_DIR)/bin/gbafix
TMC_STRINGS := $(TOOLS_DIR)/bin/tmc_strings
