"""Helpers for reading Game Boy Color graphics from oracles-disasm.

VRAM is modelled as tiles keyed by (bank, slot), where slot = (address - $8000) / 16,
as loaded by the game's graphics headers (m_GfxHeader name, $address+bank).
"""

import os
import re
import struct
import zlib


def read_indexed_png(path):
    """Returns (width, height, pixels) of an indexed PNG as palette indices."""
    data = open(path, "rb").read()
    pos = 8
    idat = b""
    while pos < len(data):
        length, kind = struct.unpack(">I4s", data[pos : pos + 8])
        body = data[pos + 8 : pos + 8 + length]
        if kind == b"IHDR":
            width, height, depth, color_type, _, _, interlace = struct.unpack(">IIBBBBB", body)
            if color_type != 3 or interlace:
                raise ValueError("%s: only non-interlaced indexed PNGs are supported" % path)
        elif kind == b"IDAT":
            idat += body
        pos += 12 + length
    raw = zlib.decompress(idat)

    stride = (width * depth + 7) // 8
    bpp = max(1, depth // 8)
    rows = []
    prev = bytearray(stride)
    i = 0
    for _ in range(height):
        filt = raw[i]
        line = bytearray(raw[i + 1 : i + 1 + stride])
        i += 1 + stride
        for x in range(stride):
            a = line[x - bpp] if x >= bpp else 0
            b = prev[x]
            c = prev[x - bpp] if x >= bpp else 0
            if filt == 1:
                line[x] = (line[x] + a) & 0xFF
            elif filt == 2:
                line[x] = (line[x] + b) & 0xFF
            elif filt == 3:
                line[x] = (line[x] + (a + b) // 2) & 0xFF
            elif filt == 4:
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                pred = a if pa <= pb and pa <= pc else b if pb <= pc else c
                line[x] = (line[x] + pred) & 0xFF
        rows.append(line)
        prev = line

    per_byte = 8 // depth
    mask = (1 << depth) - 1
    pixels = []
    for line in rows:
        for x in range(width):
            byte = line[x // per_byte]
            shift = 8 - depth * (x % per_byte + 1)
            pixels.append((byte >> shift) & mask)
    return width, height, pixels


def find_file(disasm, game, name, ext):
    for top in ("gfx_compressible", "gfx"):
        for sub in (game, "common"):
            path = os.path.join(disasm, top, sub, "%s.%s" % (name, ext))
            if os.path.exists(path):
                return path
    raise FileNotFoundError("%s.%s not found in %s" % (name, ext, disasm))


def header_entries(path, start_macro, name):
    """Returns [(file name, destination)] of a graphics header block."""
    text = open(path).read()
    m = re.search(r"%s \$\w+, %s\n(.*?)m_GfxHeaderEnd" % (start_macro, name), text, re.S)
    if m is None:
        raise KeyError("%s not found in %s" % (name, path))
    return [(n, int(d, 16)) for n, d in re.findall(r"m_GfxHeader (\w+), \$([0-9a-fA-F]+)", m.group(1))]


class Vram:
    def __init__(self):
        self.tiles = {}  # (bank, slot) -> 8 rows of 8 color indices
        self.data = [bytearray(0x2000), bytearray(0x2000)]  # raw bytes for maps

    def load(self, disasm, game, entries):
        for name, dest in entries:
            bank, addr = dest & 1, dest & ~0xF
            if name.startswith(("gfx_", "spr_")):
                w, h, px = read_indexed_png(find_file(disasm, game, name, "png"))
                slot = (addr - 0x8000) // 16
                for ty in range(h // 8):
                    for tx in range(w // 8):
                        self.tiles[(bank, slot)] = [px[(ty * 8 + y) * w + tx * 8 : (ty * 8 + y) * w + tx * 8 + 8] for y in range(8)]
                        slot += 1
            elif name.startswith(("map_", "flg_")):
                data = open(find_file(disasm, game, name, "bin"), "rb").read()
                self.data[bank][addr - 0x8000 : addr - 0x8000 + len(data)] = data

    def bg_tile(self, index, attr):
        """Tile for a BG map entry, with the signed $9000 addressing (LCDC bit 4 clear)."""
        signed = index - 256 if index >= 128 else index
        return self.tiles.get(((attr >> 3) & 1, (0x9000 + signed * 16 - 0x8000) // 16))


def palette_rgb5(disasm, game, label, count):
    """Reads count m_RGB16 colors after a label in data/<game>/paletteData.s, as 5-bit RGB."""
    text = open(os.path.join(disasm, "data", game, "paletteData.s")).read()
    text = text[text.index(label + ":") :]
    found = re.findall(r"m_RGB16 \$(\w+) \$(\w+) \$(\w+)", text)[:count]
    return [tuple(int(c, 16) for c in rgb) for rgb in found]


def palette_colors(disasm, game, label, count):
    """Like palette_rgb5, as 8-bit RGB."""
    return [tuple(c * 255 // 31 for c in rgb) for rgb in palette_rgb5(disasm, game, label, count)]


def draw_tile(image, width, x0, y0, tile, attr, colors):
    """Draws one 8x8 tile with GBC attributes into a flat list of RGB tuples."""
    for y in range(8):
        for x in range(8):
            sx = 7 - x if attr & 0x20 else x
            sy = 7 - y if attr & 0x40 else y
            color = tile[sy][sx] if tile else 0
            image[(y0 + y) * width + x0 + x] = colors[(attr & 7) * 4 + color]


def write_bmp(path, width, height, pixels):
    row_size = (width * 3 + 3) & ~3
    body = bytearray()
    for y in range(height - 1, -1, -1):  # bottom-up
        row = bytearray()
        for x in range(width):
            r, g, b = pixels[y * width + x]
            row += bytes((b, g, r))
        row += b"\0" * (row_size - len(row))
        body += row
    header = struct.pack("<2sIHHI", b"BM", 54 + len(body), 0, 0, 54)
    info = struct.pack("<IiiHHIIiiII", 40, width, height, 1, 24, 0, len(body), 2835, 2835, 0, 0)
    with open(path, "wb") as f:
        f.write(header + info + body)
