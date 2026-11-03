#!/usr/bin/env python3
"""Generate the 64-bit PC layout of ROM data that is included as binary.

Some ROM data is still .incbin'd from the original ROM, so it has the GBA's
layout and GBA addresses in it. When its C type has pointers, the 64-bit PC
build needs it laid out for that type, with symbols for the addresses. This
reads each symbol's bytes from the GBA ROM, its C type from the 32- and 64-bit
PC builds' debug info, and writes data/pc64/<symbol>.inc: assembly in the
64-bit layout. The data file includes it under .ifdef PLATFORM_PC64.

Only the USA version is generated.

Usage: gen_pc64_data.py <symbol>...   (needs tmc.gba, tmc.elf, build/pc_USA/tmc,
                                        build/pc64_USA/tmc and build/pc_USA/linker.ld.i)
"""

import bisect
import json
import os
import re
import subprocess
import sys
import tempfile

ROM_BASE = 0x08000000
OUT_DIR = "data/pc64"

# Flattens a type into leaves: (offset32, offset64, size32, size64, kind),
# kind "bytes" (copied) or "ptr" (an address, or a number in a union with one).
GDB_SCRIPT = r"""
import gdb, json

def has_ptr(t):
    t = t.strip_typedefs()
    if t.code == gdb.TYPE_CODE_PTR:
        return True
    if t.code == gdb.TYPE_CODE_ARRAY:
        return has_ptr(t.target())
    if t.code in (gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION):
        return any(has_ptr(f.type) for f in t.fields())
    return False

def leaves(t, off, out):
    t = t.strip_typedefs()
    if t.code == gdb.TYPE_CODE_PTR or (t.code == gdb.TYPE_CODE_UNION and has_ptr(t)):
        out.append([off, t.sizeof, "ptr"])
    elif t.code == gdb.TYPE_CODE_ARRAY and has_ptr(t.target()):
        e = t.target().strip_typedefs()
        for i in range(t.sizeof // e.sizeof):
            leaves(e, off + i * e.sizeof, out)
    elif t.code == gdb.TYPE_CODE_STRUCT and has_ptr(t):
        for f in t.fields():
            if f.bitsize:
                out.append([off + f.bitpos // 8, (f.bitpos % 8 + f.bitsize + 7) // 8, "bytes"])
            else:
                leaves(f.type, off + f.bitpos // 8, out)
    else:
        out.append([off, t.sizeof, "bytes"])

result = {}
for name in open(NAMES).read().split():
    sym = gdb.lookup_global_symbol(name) or gdb.lookup_static_symbol(name)
    t = sym.type.strip_typedefs()
    elem = t.target() if t.code == gdb.TYPE_CODE_ARRAY else t
    out = []
    leaves(elem, 0, out)
    result[name] = {"elem": elem.strip_typedefs().sizeof, "leaves": out,
                    "array": t.code == gdb.TYPE_CODE_ARRAY, "type": str(sym.type)}
json.dump(result, open(OUT, "w"))
"""


def gdb_layouts(exe, names):
    with tempfile.TemporaryDirectory() as d:
        open(d + "/n", "w").write("\n".join(names))
        open(d + "/s.py", "w").write(f"NAMES={d + '/n'!r}\nOUT={d + '/o'!r}\n" + GDB_SCRIPT)
        r = subprocess.run(["gdb", "-q", "-batch", "-nx", "-x", d + "/s.py", exe], capture_output=True, text=True)
        if not os.path.exists(d + "/o"):
            sys.exit("gdb failed:\n" + r.stderr)
        return json.load(open(d + "/o"))


def gba_symbols():
    """Sorted (address, name) of the GBA build's ROM and RAM symbols."""
    syms = []
    for line in subprocess.run(["nm", "-n", "tmc.elf"], capture_output=True, text=True).stdout.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in "tTdDrRbB" and not parts[2].startswith((".", "$")):
            addr = int(parts[0], 16)
            if parts[1] in "tT":
                addr &= ~1
            syms.append((addr, parts[2]))
    region = None
    for line in open("build/pc_USA/linker.ld.i"):
        if re.match(r"\s*ewram\b.*\{", line):
            region = 0x02000000
        elif re.match(r"\s*iwram\b.*\{", line):
            region = 0x03000000
        elif re.match(r"\s*\}\s*>", line):
            region = None
        m = re.match(r"\s*\.\s*=\s*(0x[0-9A-Fa-f]+)\s*;\s*(\w+)\s*=\s*\.\s*;", line)
        if m and region:
            syms.append((region + int(m.group(1), 16), m.group(2)))
    syms.sort()
    return syms


def symbol_expr(value, syms, addrs):
    """An assembly expression for a GBA address, or the number itself."""
    thumb = value & 1 and 0x08000000 <= value < 0x09000000
    target = value & ~1 if thumb else value
    if not (0x02000000 <= target < 0x02040000 or 0x03000000 <= target < 0x03008000 or ROM_BASE <= target < 0x09000000):
        return str(value)
    i = bisect.bisect_right(addrs, target) - 1
    if i < 0:
        return str(value)
    addr, name = syms[i]
    off = target - addr
    if off > 0x10000:
        return str(value)
    expr = name if off == 0 else f"{name}+0x{off:X}"

    return expr


def main():
    names = sys.argv[1:]
    rom = open("tmc.gba", "rb").read()
    syms = gba_symbols()
    addrs = [a for a, _ in syms]
    where = {n: a for a, n in syms}
    lay32 = gdb_layouts("build/pc_USA/tmc", names)
    lay64 = gdb_layouts("build/pc64_USA/tmc", names)
    os.makedirs(OUT_DIR, exist_ok=True)
    for name in names:
        l32, l64 = lay32[name], lay64[name]
        start = where[name]
        if l32["array"]:
            nxt = addrs[bisect.bisect_right(addrs, start)]
            count = (nxt - start) // l32["elem"]
        else:
            count = 1
        lines = [f"# {name} ({l64['type']}), {count} element(s), for the 64-bit PC build.",
                 "# Generated by tools/pc/gen_pc64_data.py from the USA ROM. Do not edit."]
        for e in range(count):
            src = rom[start - ROM_BASE + e * l32["elem"]:][: l32["elem"]]
            out = bytearray(l64["elem"])
            relocs = {}
            for (o32, s32, kind), (o64, s64, _) in zip(l32["leaves"], l64["leaves"]):
                if kind == "bytes":
                    out[o64:o64 + s32] = src[o32:o32 + s32]
                else:
                    value = int.from_bytes(src[o32:o32 + 4], "little")
                    relocs[o64] = symbol_expr(value, syms, addrs)
            pos = 0
            while pos < len(out):
                if pos in relocs:
                    lines.append(f"\t.quad {relocs[pos]}")
                    pos += 8
                    continue
                end = pos
                while end < len(out) and end not in relocs:
                    end += 1
                for k in range(pos, end, 16):
                    lines.append("\t.byte " + ", ".join(f"0x{b:02X}" for b in out[k:min(end, k + 16)]))
                pos = end
        with open(f"{OUT_DIR}/{name}.inc", "w") as f:
            f.write("\n".join(lines) + "\n")
        print(f"{name}: {count} x {l64['elem']} bytes")


if __name__ == "__main__":
    main()
