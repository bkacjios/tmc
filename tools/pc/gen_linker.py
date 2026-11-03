#!/usr/bin/env python3
"""Generate the PC link script and object list from the preprocessed linker.ld.

The GBA linker script places RAM variables at fixed EWRAM/IWRAM offsets and
lays out ROM data in a fixed order. For the PC build:

- RAM symbols become absolute addresses (0x02000000 / 0x03000000 + offset).
  The platform layer maps memory at those addresses before the game runs.
- ROM data (data/*.o and the .rodata of C files) keeps its relative order in a
  single .tmc_rom output section, since some tables rely on adjacency.
- C code is left to the host's default link script.
- Handwritten ARM assembly (asm/) and agbcc's libc are dropped; the platform
  layer replaces them.

With --ram-commons=<prefix> (64-bit build), RAM variables don't get their GBA
addresses, except the ones in EWRAM's first 0xC0 bytes (see FIXED_EWRAM):
their types with pointers are larger there. Instead:
- <prefix>.txt lists them; their declarations in C become common definitions
  (ram_commons.py), sized by their 64-bit types.
- <prefix>.s defines each as a common too, with its size on the GBA (up to the
  next variable), for those C declares without a size or not at all. The
  linker gives each the largest size.
- PLATFORM_RAM variables are left out of both: the platform layer defines
  them in .gba_ram.
- All of them go to a .gba_ram section between __gba_ram_start and
  __gba_ram_end (the platform layer clears it for RegisterRamReset).

With --pe, writes the script for a 32-bit Windows (PE) build.

Usage: gen_linker.py [--ram-commons=<prefix>] [--pe] <preprocessed linker.ld> <out script> <out object list> [excluded objects...]
"""

import re
import sys

REGIONS = {"ewram": 0x02000000, "iwram": 0x03000000}
REGION_SIZES = {"ewram": 0x40000, "iwram": 0x8000}
FIXED_EWRAM = 0xC0
# Defined together by the platform layer (src/platform/pc/memory.c), since the
# game walks them as one array.
PLATFORM_RAM = {"gPlayerEntity", "gAuxPlayerEntities", "gEntities"}

SECTION_START = re.compile(r"^\s*(ewram|iwram|rom)\b.*\{\s*$")
SECTION_END = re.compile(r"^\s*\}\s*>\s*(ewram|iwram|rom)\s*$")
RAM_SYMBOL = re.compile(r"^\s*\.\s*=\s*(0x[0-9A-Fa-f]+)\s*;\s*(\w+)\s*=\s*\.\s*;")
GLOBAL_ASSIGN = re.compile(r"^\s*(\w+)\s*=\s*([^;]+);")
OBJECT_ENTRY = re.compile(r"^\s*((?:\w|/)+\.o)\((\.\w+)\)\s*;")
ALIGN = re.compile(r"^\s*\.\s*=\s*ALIGN\((\d+)\)\s*;")


def is_ram_function(name):
    return name.startswith("ram_") and not name.startswith(("ram_gUnk_", "ram_0x"))


def main():
    args = sys.argv[1:]
    ram_commons = None
    pe = False
    while args and args[0].startswith("--"):
        opt = args.pop(0)
        if opt.startswith("--ram-commons="):
            ram_commons = opt.split("=", 1)[1]
        elif opt == "--pe":
            pe = True
        else:
            sys.exit(f"unknown option {opt}")
    # PE (Windows) names C symbols with a leading underscore on x86; the
    # assembled data gets one too (objcopy --prefix-symbols in PC.mk).
    prefix = "_" if pe else ""
    rodata = ".rdata .rdata$* .rodata .rodata.*" if pe else ".rodata .rodata.*"
    src, out_script, out_objs, *excluded = args
    excluded = set(excluded)
    ram = []  # (address, name, region)

    symbols = []
    rom = []
    objects = []
    section = None

    for line in open(src):
        m = SECTION_START.match(line)
        if m:
            section = m.group(1)
            continue
        if SECTION_END.match(line):
            section = None
            continue

        if section in REGIONS:
            m = RAM_SYMBOL.match(line)
            # ram_* are ARM functions copied to IWRAM on the GBA; on PC they
            # are C functions in src/platform/pc. The ram_ data tables keep
            # their IWRAM address: the PC build copies them there as well.
            if m and not is_ram_function(m.group(2)):
                addr = REGIONS[section] + int(m.group(1), 16)
                # EWRAM's first 0xC0 bytes keep their addresses in any case:
                # the first 0x30 survive soft resets (gSaveHeader,
                # gUnk_02000010), and gMenu, gIntroState and gChooseFileState
                # share 0x80-0xAF. All of them fit there in the 64-bit build.
                if ram_commons is None or addr < REGIONS["ewram"] + FIXED_EWRAM:
                    symbols.append(f"{prefix}{m.group(2)} = 0x{addr:08X};")
                else:
                    ram.append((addr, m.group(2), section))
        elif section == "rom":
            m = OBJECT_ENTRY.match(line)
            if m:
                obj = m.group(1)
                if obj.startswith("asm/") or obj in excluded:
                    continue
                if obj not in objects:
                    objects.append(obj)
                if obj.startswith("data/"):
                    rom.append(f"        {obj}(.text {rodata})")
                elif m.group(2) == ".rodata":
                    rom.append(f"        {obj}({rodata})")
                continue
            m = ALIGN.match(line)
            if m:
                rom.append(f"        . = ALIGN({m.group(1)});")
        elif section is None:
            m = GLOBAL_ASSIGN.match(line)
            if m and m.group(1) not in ("ENTRY",):
                value = m.group(2).strip()
                if pe and re.fullmatch(r"[A-Za-z_]\w*", value):
                    value = prefix + value
                symbols.append(f"{prefix}{m.group(1)} = {value};")

    with open(out_script, "w") as f:
        f.write("/* Generated by tools/pc/gen_linker.py from linker.ld. Do not edit. */\n\n")
        f.write("\n".join(symbols))
        # PE sections must start on the section alignment (a page).
        align = "__section_alignment__" if pe else "4"
        f.write(f"\n\nSECTIONS {{\n    .tmc_rom ALIGN({align}) : ALIGN(4) {{\n")
        f.write("\n".join(rom))
        f.write(f"\n    }}\n}}\nINSERT AFTER {'.rdata' if pe else '.rodata'};\n")
        if ram_commons is not None:
            f.write("\nSECTIONS {\n    .gba_ram (NOLOAD) : ALIGN(64) {\n")
            f.write("        __gba_ram_start = .;\n        *(.gba_ram)\n        *(COMMON)\n        __gba_ram_end = .;\n")
            f.write("    }\n}\nINSERT AFTER .bss;\n")

    with open(out_objs, "w") as f:
        f.write("\n".join(objects) + "\n")

    if ram_commons is not None:
        write_ram_commons(ram_commons, ram)


def write_ram_commons(prefix, ram):
    addresses = sorted(set(addr for addr, _, _ in ram))
    with open(prefix + ".txt", "w") as f:
        f.write("\n".join(name for _, name, _ in ram if name not in PLATFORM_RAM) + "\n")
    with open(prefix + ".s", "w") as f:
        f.write("# Generated by tools/pc/gen_linker.py from linker.ld. Do not edit.\n")
        for addr, name, region in sorted(ram):
            if name in PLATFORM_RAM:
                continue
            end = REGIONS[region] + REGION_SIZES[region]
            size = next((a for a in addresses if a > addr and a <= end), end) - addr
            f.write(f"\t.comm {name}, {size}, 16\n")


if __name__ == "__main__":
    main()
