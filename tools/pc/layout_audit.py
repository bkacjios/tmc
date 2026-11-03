#!/usr/bin/env python3
"""Compare the layout of the game's variables in the 32- and 64-bit PC builds.

Types with pointers are larger in the 64-bit build. That matters for:

- RAM variables. The 64-bit build allocates them in .gba_ram (ram_commons.py,
  gen_linker.py), with their GBA size for those no C file declares with a
  size: arrays of types that grew need a size in their declaration.
- ROM data defined in assembly (data/*.s, asm/src/*.s): the assembly has to
  emit the 64-bit layout of its C type (see the ptr macro in asm/macros.inc).
  Checked by size: for an array, the data up to the next symbol must hold as
  many elements of the 64-bit type as it does of the 32-bit one; a single
  object must fit.

Types come from the executables' debug info, read with gdb's Python API.

Usage: layout_audit.py <32-bit exe> <64-bit exe> [--check]

With --check, exits with an error while anything is reported.
"""

import json
import os
import subprocess
import sys
import tempfile

GDB_SCRIPT = r"""
import gdb, json
names = open(NAMES).read().split()
out = {}
for name in names:
    sym = gdb.lookup_global_symbol(name) or gdb.lookup_static_symbol(name)
    try:
        t = sym.type if sym is not None else gdb.parse_and_eval(name).type
        info = {"type": str(t), "size": t.sizeof}
        base = t.strip_typedefs()
        if base.code == gdb.TYPE_CODE_ARRAY:
            info["element"] = base.target().sizeof
        out[name] = info
    except Exception:
        pass
json.dump(out, open(OUT, "w"))
"""

RAM_START, RAM_END = 0x02000000, 0x04000000


def is_ram(addr):
    return RAM_START <= addr < RAM_END


def data_symbols(exe):
    """Global data symbols: name -> address."""
    syms = {}
    nm = subprocess.run(["nm", "--defined-only", exe], capture_output=True, text=True, check=True)
    for line in nm.stdout.splitlines():
        parts = line.split()
        if len(parts) != 3 or parts[1] not in ("d", "D", "b", "B", "r", "R", "A", "C", "t", "T"):
            continue
        name = parts[2]
        if name.startswith((".", "_")) or "." in name:
            continue
        syms[name] = int(parts[0], 16)
    return syms


def symbol_types(exe, names):
    with tempfile.TemporaryDirectory() as tmp:
        names_path = os.path.join(tmp, "names")
        out_path = os.path.join(tmp, "out.json")
        script_path = os.path.join(tmp, "dump.py")
        with open(names_path, "w") as f:
            f.write("\n".join(names))
        with open(script_path, "w") as f:
            f.write(f"NAMES = {names_path!r}\nOUT = {out_path!r}\n" + GDB_SCRIPT)
        subprocess.run(["gdb", "-q", "-batch", "-nx", "-x", script_path, exe], capture_output=True, check=True)
        with open(out_path) as f:
            return json.load(f)


def symbol_sizes(exe):
    """Symbol -> size, for symbols that have one."""
    sizes = {}
    nm = subprocess.run(["nm", "-S", "--defined-only", exe], capture_output=True, text=True, check=True)
    for line in nm.stdout.splitlines():
        parts = line.split()
        if len(parts) == 4:
            sizes[parts[3]] = int(parts[1], 16)
    return sizes


def spans(syms):
    """Symbol -> bytes up to the next symbol's address."""
    addrs = sorted(set(syms.values()))
    nxt = {a: b for a, b in zip(addrs, addrs[1:])}
    return {name: nxt[addr] - addr for name, addr in syms.items() if addr in nxt}


def defining_objects(exe):
    """Symbol -> object file that defines it, from the link map: an input
    section line (" .rodata  0x... 0x... data/foo.o") is followed by the
    symbols in it ("  0x...  name")."""
    owners = {}
    obj = None
    with open(exe + ".map") as f:
        for line in f:
            parts = line.split()
            if len(parts) == 4 and parts[0].startswith(".") and parts[1].startswith("0x"):
                obj = parts[3]
            elif len(parts) == 2 and parts[0].startswith("0x") and obj is not None:
                owners[parts[1]] = obj
    return owners


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    check = "--check" in sys.argv
    exe32, exe64 = args

    syms32 = data_symbols(exe32)
    syms64 = data_symbols(exe64)
    names = sorted(syms32)
    types32 = symbol_types(exe32, names)
    types64 = symbol_types(exe64, names)
    owners = defining_objects(exe32)

    # RAM variables: the 64-bit build allocates each one (.gba_ram) as large
    # as its largest declaration, or its GBA size when it has no sized one.
    # That must hold as many elements of its 64-bit type as fit in its room
    # on the GBA (up to the next variable).
    sizes64 = symbol_sizes(exe64)
    ram32 = sorted(set(addr for addr in syms32.values() if is_ram(addr)))
    fixed = []
    for name, addr in syms32.items():
        if not is_ram(addr) or name not in types32 or name not in types64 or name not in sizes64:
            continue
        t32, t64 = types32[name], types64[name]
        region_end = (addr & ~0xFFFFFF) + (0x40000 if addr < 0x03000000 else 0x8000)
        room = min(next((a for a in ram32 if a > addr), region_end), region_end) - addr
        if t64["size"] == 0 and t32.get("element") and t64.get("element"):  # declared without a size
            need = room // t32["element"] * t64["element"]
        else:
            need = t64["size"]
        if sizes64[name] < need:
            fixed.append((addr, name, f"allocated {sizes64[name]} bytes, needs {need}", t64["type"]))

    # ROM data from assembly: what the 64-bit build emitted must match its C
    # type. Each symbol's span (up to the next symbol) gives the element count
    # in the 32-bit build; the 64-bit span must hold as many 64-bit elements.
    span32, span64 = spans(syms32), spans(syms64)
    rom = []
    for name, addr in syms32.items():
        if is_ram(addr) or name not in types32 or name not in types64 or name not in span64:
            continue
        owner = owners.get(name, "")
        t32, t64 = types32[name], types64[name]
        if not owner.startswith(("data/", "asm/")) or (t32["size"] == t64["size"] and t32.get("element") == t64.get("element")):
            continue
        if "element" not in t64:  # one object, maybe followed by other data
            if span64[name] < t64["size"]:
                rom.append((owner, name, f"{t64['size']} bytes expected, emitted {span64[name]}", t64["type"]))
            continue
        e32, e64 = t32["element"], t64["element"]
        if e32 == 0:
            continue
        count, rest = divmod(span32[name], e32)
        expected = count * e64 + rest
        if not expected <= span64[name] < expected + 8:  # up to 7 bytes of alignment
            rom.append((owner, name, f"{count} x {e32} + {rest} bytes -> expected {expected}, emitted {span64[name]}", t64["type"]))

    print(f"RAM variables allocated too small (declare them with a size): {len(fixed)}")
    for addr, name, change, typ in sorted(fixed):
        print(f"  {addr:08X} {name}: {change} ({typ})")
    print(f"Assembly-defined data not laid out for its 64-bit C type: {len(rom)}")
    for owner, name, change, typ in sorted(rom):
        print(f"  {owner}: {name}: {change} ({typ})")

    if check and (fixed or rom):
        sys.exit(1)


if __name__ == "__main__":
    main()
