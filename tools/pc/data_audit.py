#!/usr/bin/env python3
"""Check the 64-bit PC build's ROM data against the 32-bit build, by content.

The 32-bit PC build lays out data like the GBA. The 64-bit build must emit the
same data in the 64-bit layout of its C type: pointers widened to 8 bytes,
fields moved to their 64-bit offsets. For every symbol defined in assembly
(data/*.s, asm/src/*.s) that C declares with a type, this converts the 32-bit
build's bytes to the 64-bit layout and compares them with the 64-bit build's:

- plain fields must hold the same bytes;
- pointers must point at the same symbol and offset (the offset is mapped
  into the target's 64-bit layout when its type grew);
- 4-byte fields that hold an address (u32 dest fields) may differ only by
  pointing at the same symbol.

Unlike layout_audit.py, which compares sizes, this finds entries of the wrong
size inside a table, fields in the wrong place, and pointers to the wrong
target. Symbols without a C type that follow a typed array (the game indexes
past its end, as with gSpritePtrs and gMoreSpritePtrs) are checked as more of
its elements.

Types come from the executables' debug info, read with gdb's Python API.

Usage: data_audit.py <32-bit exe> <64-bit exe> [--all] [--check]

Prints one line per symbol with differences (the first few differences of
each with --all). With --check, exits with an error if there are any.
"""

import bisect
import json
import os
import struct
import subprocess
import sys
import tempfile

GDB_SCRIPT = r"""
import gdb, json

def has_ptr(t):
    t = t.strip_typedefs()
    if t.code in (gdb.TYPE_CODE_PTR, gdb.TYPE_CODE_FUNC):
        return t.code == gdb.TYPE_CODE_PTR
    if t.code == gdb.TYPE_CODE_ARRAY:
        return has_ptr(t.target())
    if t.code in (gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION):
        return any(has_ptr(f.type) for f in t.fields() if not f.is_base_class)
    return False

# Leaves of a type: [path, offset, size, kind]; kind "ptr" (an address),
# "union" (a union with a pointer: an address or a number) or "bytes".
# Structs are split into their fields, so fields can be matched by name.
def leaves(t, path, off, out):
    t = t.strip_typedefs()
    if t.code == gdb.TYPE_CODE_PTR:
        out.append([path, off, t.sizeof, "ptr"])
    elif t.code == gdb.TYPE_CODE_UNION and has_ptr(t):
        out.append([path, off, t.sizeof, "union"])
    elif t.code == gdb.TYPE_CODE_ARRAY and t.target().strip_typedefs().code in (gdb.TYPE_CODE_PTR, gdb.TYPE_CODE_STRUCT, gdb.TYPE_CODE_UNION, gdb.TYPE_CODE_ARRAY):
        e = t.target().strip_typedefs()
        for i in range(t.sizeof // e.sizeof if e.sizeof else 0):
            leaves(e, "%s[%d]" % (path, i), off + i * e.sizeof, out)
    elif t.code == gdb.TYPE_CODE_STRUCT:
        for f in t.fields():
            name = "%s.%s" % (path, f.name or "?")
            if f.bitsize:
                out.append([name, off + f.bitpos // 8, (f.bitpos % 8 + f.bitsize + 7) // 8, "bytes"])
            else:
                leaves(f.type, name, off + f.bitpos // 8, out)
    elif t.sizeof:
        out.append([path, off, t.sizeof, "bytes"])

result = {}
for name in open(NAMES).read().split():
    try:
        # Data defined in assembly only has declarations in the debug info.
        sym = gdb.lookup_global_symbol(name) or gdb.lookup_static_symbol(name)
        typ = sym.type if sym is not None else gdb.parse_and_eval(name).type
        t = typ.strip_typedefs()
    except Exception:
        continue
    if t.code == gdb.TYPE_CODE_FUNC:
        continue
    array = t.code == gdb.TYPE_CODE_ARRAY
    elem = t.target().strip_typedefs() if array else t
    out = []
    leaves(elem, "", 0, out)
    result[name] = {"type": str(typ), "size": t.sizeof, "elem": elem.sizeof, "array": array, "leaves": out}
json.dump(result, open(OUT, "w"))
"""


class Image:
    """An executable's loaded bytes and symbols."""

    def __init__(self, path):
        self.data = open(path, "rb").read()
        self.wide = self.data[4] == 2
        self.segments = []
        if self.wide:
            phoff, = struct.unpack_from("<Q", self.data, 0x20)
            phentsize, phnum = struct.unpack_from("<HH", self.data, 0x36)
            for i in range(phnum):
                p_type, _, p_offset, p_vaddr, _, p_filesz = struct.unpack_from("<IIQQQQ", self.data, phoff + i * phentsize)
                if p_type == 1:
                    self.segments.append((p_vaddr, p_offset, p_filesz))
        else:
            phoff, = struct.unpack_from("<I", self.data, 0x1C)
            phentsize, phnum = struct.unpack_from("<HH", self.data, 0x2A)
            for i in range(phnum):
                p_type, p_offset, p_vaddr, _, p_filesz = struct.unpack_from("<IIIII", self.data, phoff + i * phentsize)
                if p_type == 1:
                    self.segments.append((p_vaddr, p_offset, p_filesz))

        # name -> address, for names defined once; and sorted (address, name)
        # of all of them, to find what an address points into.
        counts, where, every = {}, {}, set()
        nm = subprocess.run(["nm", "--defined-only", path], capture_output=True, text=True, check=True)
        for line in nm.stdout.splitlines():
            parts = line.split()
            if len(parts) != 3 or parts[1] not in "tTdDrRbBA" or parts[2].startswith((".", "$")):
                continue
            name = parts[2]
            value = int(parts[0], 16)
            if parts[1] == "A" and not 0x02000000 <= value < 0x04000000:
                continue  # a constant, not an address
            counts[name] = counts.get(name, 0) + 1
            where[name] = value
            every.add(value)
        self.where = {n: a for n, a in where.items() if counts[n] == 1}
        self.sorted = sorted((a, n) for n, a in self.where.items())
        self.addrs = [a for a, _ in self.sorted]
        self.all_addrs = sorted(every)  # with names defined more than once

    def read(self, addr, size):
        for vaddr, offset, filesz in self.segments:
            if vaddr <= addr and addr + size <= vaddr + filesz:
                return self.data[offset + addr - vaddr : offset + addr - vaddr + size]
        return None

    def symbol_at(self, addr):
        """(name, offset) of the symbol an address points into, or None."""
        i = bisect.bisect_right(self.addrs, addr) - 1
        if i < 0:
            return None
        a, name = self.sorted[i]
        return name, addr - a

    def span(self, name):
        """Bytes from a symbol to the next one."""
        addr = self.where[name]
        i = bisect.bisect_right(self.all_addrs, addr)
        return self.all_addrs[i] - addr if i < len(self.all_addrs) else 0


def gdb_types(exe, names):
    with tempfile.TemporaryDirectory() as d:
        open(d + "/n", "w").write("\n".join(names))
        open(d + "/s.py", "w").write(f"NAMES={d + '/n'!r}\nOUT={d + '/o'!r}\n" + GDB_SCRIPT)
        r = subprocess.run(["gdb", "-q", "-batch", "-nx", "-x", d + "/s.py", exe], capture_output=True, text=True)
        if not os.path.exists(d + "/o"):
            sys.exit("gdb failed:\n" + r.stderr)
        return json.load(open(d + "/o"))


def defining_objects(map_path):
    """From a link map: symbol -> (object file that defines it, end of the
    input section it is in)."""
    owners = {}
    obj = end = None
    with open(map_path) as f:
        for line in f:
            parts = line.split()
            if len(parts) == 4 and parts[0].startswith(".") and parts[1].startswith("0x"):
                obj, end = parts[3], int(parts[1], 16) + int(parts[2], 16)
            elif len(parts) == 3 and parts[0].startswith("0x") and parts[1].startswith("0x"):
                obj, end = parts[2], int(parts[0], 16) + int(parts[1], 16)
            elif len(parts) == 2 and parts[0].startswith("0x") and obj is not None:
                owners[parts[1]] = (obj, end)
    return owners


class Audit:
    def __init__(self, exe32, exe64):
        self.i32, self.i64 = Image(exe32), Image(exe64)
        names = sorted(set(self.i32.where) & set(self.i64.where))
        self.t32 = gdb_types(exe32, names)
        self.t64 = gdb_types(exe64, names)
        self.owners = {n: o for n, (o, _) in defining_objects(exe64 + ".map").items()}
        self.ends32 = {n: e for n, (_, e) in defining_objects(exe32 + ".map").items()}

    def map_offset(self, name, delta):
        """An offset into a symbol in the 32-bit layout, in the 64-bit one."""
        t32, t64 = self.t32.get(name), self.t64.get(name)
        if t32 is None or t64 is None or t32["elem"] == t64["elem"] or not t32["elem"]:
            return delta
        index, rest = divmod(delta, t32["elem"])
        if not t32["array"] and index:
            return None
        l64 = {path: off for path, off, _, _ in t64["leaves"]}
        for path, off, size, _ in t32["leaves"]:
            if off <= rest < off + size and path in l64:
                return index * t64["elem"] + l64[path] + rest - off
        return None

    def map_address(self, value):
        """The 64-bit build's address for a 32-bit build's one, or None.
        Both builds link the program at 0x10000000; below it, only GBA RAM
        moves (the 64-bit build's .gba_ram)."""
        if value < 0x10000000 and not 0x02000000 <= value < 0x04000000:
            return None
        hit = self.i32.symbol_at(value)
        if hit is None:
            return None
        name, delta = hit
        if name not in self.i64.where or (delta and delta >= self.i32.span(name)):
            return None
        offset = self.map_offset(name, delta)
        return None if offset is None else self.i64.where[name] + offset

    def same_address(self, v32, v64):
        return v32 != 0 and self.map_address(v32) == v64

    def describe(self, image, value):
        hit = image.symbol_at(value)
        if hit is None or (hit[1] and hit[1] >= image.span(hit[0])):
            return f"0x{value:X}"
        return f"0x{value:X} ({hit[0]})" if hit[1] == 0 else f"0x{value:X} ({hit[0]}+0x{hit[1]:X})"

    def compare_flat(self, b32, b64, step):
        """Data without pointers in its type, which may still hold addresses
        in 32-bit words (scripts): the first difference, or None."""
        i = 0
        n = min(len(b32), len(b64))
        while i < n:
            if b32[i : i + step] == b64[i : i + step]:
                i += step
                continue
            # A 32-bit address at i, or one starting before it.
            for start in sorted({i, i - i % 4, i - 2} if step < 4 else {i}):
                if start < 0:
                    continue
                v32 = int.from_bytes(b32[start : start + 4], "little")
                v64 = int.from_bytes(b64[start : start + 4], "little")
                if start + 4 <= n and self.same_address(v32, v64):
                    i = start + 4
                    break
            else:
                return f"+0x{i:X}: {b32[i:i + 4].hex()} -> {b64[i:i + 4].hex()}"
        return None

    def compare_element(self, index, b32, b64, l32, l64, found):
        """Appends the element's differences to found; returns False if the
        element is past the array's end (a pointer field holds no address)."""
        problems = []
        by64 = {path: (off, size, kind) for path, off, size, kind in l64}
        seen = set()
        for path, off32, size32, kind in l32:
            if path not in by64:
                problems.append(f"[{index}]{path}: missing in the 64-bit type")
                continue
            seen.add(path)
            off64, size64, _ = by64[path]
            v32 = b32[off32 : off32 + size32]
            v64 = b64[off64 : off64 + size64]
            if kind in ("ptr", "union") or (size32 == 4 and size64 == 8):
                p32 = int.from_bytes(v32[:4], "little")
                p64 = int.from_bytes(v64, "little")
                expected = self.map_address(p32) if p32 else 0
                if expected is None:
                    if kind == "ptr" and not l32[0][0].startswith("."):
                        return False  # a table of pointers ends here
                    expected = p32  # a number
                if p64 != expected:
                    problems.append(f"[{index}]{path}: {self.describe(self.i32, p32)} -> {self.describe(self.i64, p64)}")
            elif size32 != size64:
                problems.append(f"[{index}]{path}: {size32} bytes -> {size64} bytes")
            elif v32 != v64:
                if size32 == 4 and self.same_address(int.from_bytes(v32, "little"), int.from_bytes(v64, "little")):
                    continue  # an address kept in a u32
                problems.append(f"[{index}]{path}: {v32.hex()} -> {v64.hex()}")
        # Fields only the 64-bit type has (padding for larger pointers).
        for path, (off64, size64, _) in by64.items():
            if path not in seen and any(b64[off64 : off64 + size64]):
                problems.append(f"[{index}]{path}: 64-bit-only field is not zero")
        found.extend(problems)
        return True

    def element_count(self, name, t32):
        """Elements of an array in the 32-bit build: up to the next symbol,
        or, for elements with pointers, past untyped symbols from the same
        object right after it."""
        a32 = self.i32.where[name]
        end = a32 + self.i32.span(name)
        if name in self.ends32:  # not past its object's data
            end = min(end, self.ends32[name])
        owner = self.owners.get(name)
        while any(kind != "bytes" for _, _, _, kind in t32["leaves"]):
            hit = self.i32.symbol_at(end)
            if hit is None or hit[1] or hit[0] in self.t32 or self.owners.get(hit[0]) != owner:
                break
            nxt = self.i32.span(hit[0])
            if not nxt or nxt % t32["elem"]:
                break
            # It must follow the array in the 64-bit build as well, not after
            # alignment padding: then it is a table of its own.
            t64 = self.t64[name]
            if self.i64.where.get(hit[0]) != self.i64.where[name] + (end - a32) // t32["elem"] * t64["elem"]:
                break
            end += nxt
        count = (end - a32) // t32["elem"]
        if t32["size"]:
            count = min(count, t32["size"] // t32["elem"])
        return count

    def run(self, show_all):
        failures = 0
        checked = 0
        for name in sorted(self.t32):
            owner = self.owners.get(name, "")
            if not owner.startswith(("data/", "asm/")) or name not in self.t64:
                continue
            t32, t64 = self.t32[name], self.t64[name]
            if not t32["elem"] or not t64["elem"]:
                continue
            def signature(t):
                return [(p, k, n if k == "bytes" else 0) for p, _, n, k in t["leaves"]]

            if signature(t32) != signature(t64) and t32["type"] != t64["type"]:
                print(f"{owner}: {name}: declared as {t32['type']} and {t64['type']}, not compared")
                continue
            count = self.element_count(name, t32) if t32["array"] else 1
            a32, a64 = self.i32.where[name], self.i64.where[name]
            b32 = self.i32.read(a32, count * t32["elem"])
            b64 = self.i64.read(a64, count * t64["elem"])
            if b32 is None or b64 is None:
                continue
            checked += 1
            problems = []
            flat = all(kind == "bytes" for _, _, _, kind in t32["leaves"]) and t32["elem"] == t64["elem"]
            if flat:
                problem = self.compare_flat(b32, b64, min(t32["elem"], 4) if t32["elem"] in (1, 2, 4) else 1)
                if problem:
                    problems.append(problem)
            else:
                for i in range(count):
                    e32 = b32[i * t32["elem"] :][: t32["elem"]]
                    e64 = b64[i * t64["elem"] :][: t64["elem"]]
                    if not self.compare_element(i, e32, e64, t32["leaves"], t64["leaves"], problems):
                        count = i
                        break
            if problems:
                failures += 1
                print(f"{owner}: {name} ({t64['type']}, {count} x {t64['elem']}): {len(problems)} differences: {problems[0]}")
                if show_all:
                    for p in problems[1:8]:
                        print(f"    {p}")
                    if len(problems) > 8:
                        print(f"    ... {len(problems) - 8} more")
        print(f"{checked} symbols checked, {failures} with differences")
        return failures


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    failures = Audit(*args).run("--all" in sys.argv)
    if "--check" in sys.argv and failures:
        sys.exit(1)


if __name__ == "__main__":
    main()
