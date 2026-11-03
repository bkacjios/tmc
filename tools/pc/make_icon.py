"""Makes the Windows icon (.ico) from the title screen logo.

Reads a TMC_DUMP_HW dump (src/platform/pc/main.c) taken on the finished title
screen (frame 420) and draws the "THE LEGEND OF ZELDA The Minish Cap" logo, its
sprites in palettes 6 and 7, on a transparent background. The sword (BG2), the
sword's pommel (a sprite in palette 8) and the backgrounds are left out.

usage: make_icon.py dump out.ico
"""

import math
import struct
import sys

from tmc_sprites import encode_png

EWRAM_SIZE = 0x40000
IWRAM_SIZE = 0x8000
IO_SIZE = 0x400
PLTT_SIZE = 0x400
VRAM_SIZE = 0x18000

LOGO_PALETTES = (6, 7)
ICON_SIZES = (256, 48, 32, 16)
MARGIN = 4  # transparent pixels around the logo, at the logo's own scale

# OBJ shapes (square, wide, tall) by size: width, height.
OBJ_SIZES = {
    0: ((8, 8), (16, 16), (32, 32), (64, 64)),
    1: ((16, 8), (32, 8), (32, 16), (64, 32)),
    2: ((8, 16), (8, 32), (16, 32), (32, 64)),
}


def rgba(color):
    return ((color & 31) * 255 // 31, ((color >> 5) & 31) * 255 // 31, ((color >> 10) & 31) * 255 // 31, 255)


def draw_logo(dump):
    """Returns the logo as rows of RGBA tuples, cropped to its pixels."""
    io = EWRAM_SIZE + IWRAM_SIZE
    pltt = io + IO_SIZE
    vram = pltt + PLTT_SIZE
    oam = vram + VRAM_SIZE
    (dispcnt,) = struct.unpack_from("<H", dump, io)
    assert dispcnt & 0x40, "sprite tiles are expected in 1D mapping"

    pixels = {}
    for i in reversed(range(128)):  # lower indices are drawn on top
        attr0, attr1, attr2 = struct.unpack_from("<3H", dump, oam + i * 8)
        palette = attr2 >> 12
        if (attr0 >> 8) & 3 != 0 or palette not in LOGO_PALETTES:
            continue  # hidden, affine or not part of the logo
        assert not attr0 & 0x2000, "logo sprites are expected to be 16 colors"
        width, height = OBJ_SIZES[attr0 >> 14][attr1 >> 14]
        x0, y0, tile = attr1 & 0x1FF, attr0 & 0xFF, attr2 & 0x3FF
        for y in range(height):
            for x in range(width):
                t = tile + (y // 8) * (width // 8) + x // 8
                byte = dump[vram + 0x10000 + t * 32 + (y % 8) * 4 + (x % 8) // 2]
                index = byte >> 4 if x & 1 else byte & 15
                if index:
                    sx = width - 1 - x if attr1 & 0x1000 else x
                    sy = height - 1 - y if attr1 & 0x2000 else y
                    (color,) = struct.unpack_from("<H", dump, pltt + 0x200 + (palette * 16 + index) * 2)
                    pixels[x0 + sx, y0 + sy] = rgba(color)

    assert pixels, "no logo sprites found"
    left = min(x for x, _ in pixels)
    top = min(y for _, y in pixels)
    width = max(x for x, _ in pixels) - left + 1
    height = max(y for _, y in pixels) - top + 1
    return [[pixels.get((left + x, top + y), (0, 0, 0, 0)) for x in range(width)] for y in range(height)]


def square(image):
    """Centers the image on a transparent square with a margin."""
    width, height = len(image[0]), len(image)
    side = max(width, height) + 2 * MARGIN
    left, top = (side - width) // 2, (side - height) // 2
    clear = (0, 0, 0, 0)
    out = [[clear] * side for _ in range(side)]
    for y in range(height):
        out[top + y][left : left + width] = image[y]
    return out


def resize(image, size):
    """Resamples a square image to size x size by averaging a grid of samples
    per pixel, with premultiplied alpha."""
    side = len(image)
    n = max(4, 2 * math.ceil(side / size))
    out = []
    for y in range(size):
        row = []
        for x in range(size):
            r = g = b = a = 0
            for sy in range(n):
                src_row = image[int((y + (sy + 0.5) / n) * side / size)]
                for sx in range(n):
                    pr, pg, pb, pa = src_row[int((x + (sx + 0.5) / n) * side / size)]
                    r += pr * pa
                    g += pg * pa
                    b += pb * pa
                    a += pa
            if a:
                row.append((r // a, g // a, b // a, a // (n * n)))
            else:
                row.append((0, 0, 0, 0))
        out.append(row)
    return out


def ico(images):
    """An .ico file with PNG-compressed entries (Windows Vista and later)."""
    header = struct.pack("<HHH", 0, 1, len(images))
    entries = b""
    data = b""
    offset = len(header) + 16 * len(images)
    for size, blob in images:
        entries += struct.pack("<BBBBHHII", size % 256, size % 256, 0, 0, 1, 32, len(blob), offset + len(data))
        data += blob
    return header + entries + data


def main():
    dump_path, out_path = sys.argv[1:]
    logo = square(draw_logo(open(dump_path, "rb").read()))
    images = [(size, encode_png(resize(logo, size))) for size in ICON_SIZES]
    with open(out_path, "wb") as f:
        f.write(ico(images))


if __name__ == "__main__":
    main()
