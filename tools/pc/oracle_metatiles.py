#!/usr/bin/env python3
"""Render Oracle of Ages overworld tileset data, for building tile mappings.

  sheet <disasm> <out.bmp>               the 256 metatiles of tileset 00 (Lynna City),
                                         16 per row, metatile n at (n % 16, n / 16)
  map <disasm> <x> <y> <w> <h> <out.bmp> present overworld screens x..x+w-1, y..y+h-1
"""

import os
import sys

from gbc_gfx import Vram, draw_tile, header_entries, palette_colors, write_bmp

GAME = "ages"
SCREEN_W, SCREEN_H = 10, 8  # metatiles per overworld screen


def load_tileset(disasm):
    """Returns (vram, colors, mappings) for tileset 00 (Lynna City, present)."""
    data = os.path.join(disasm, "data", GAME)
    vram = Vram()
    vram.load(disasm, GAME, header_entries(os.path.join(data, "gfxHeaders.s"), "m_GfxHeaderStart", "GFXH_TILESET_OVERWORLD_PRESENT"))
    vram.load(disasm, GAME, header_entries(os.path.join(data, "uniqueGfxHeaders.s"), "m_UniqueGfxHeaderStart", "UNIQUE_GFXH_LYNNA_CITY_1"))
    # PALH_TILESET_LYNNA_CITY sets BG palettes 2-7; 0-1 are left grey here.
    grey = [(255, 255, 255), (170, 170, 170), (85, 85, 85), (0, 0, 0)]
    colors = grey * 2 + palette_colors(disasm, GAME, "paletteData4a30", 24)
    mappings = open(os.path.join(disasm, "tileset_layouts", GAME, "tilesetMappings00.bin"), "rb").read()
    return vram, colors, mappings


def draw_metatile(pixels, width, x0, y0, vram, colors, mappings, n):
    entry = mappings[n * 8 : n * 8 + 8]
    for i in range(4):
        index, attr = entry[i], entry[4 + i]
        draw_tile(pixels, width, x0 + (i & 1) * 8, y0 + (i >> 1) * 8, vram.bg_tile(index, attr), attr, colors)


def read_screen(disasm, x, y):
    return open(os.path.join(disasm, "rooms", GAME, "small", "room00%02x.bin" % (y * 16 + x)), "rb").read()


def main():
    mode, disasm = sys.argv[1], sys.argv[2]
    vram, colors, mappings = load_tileset(disasm)

    if mode == "sheet":
        out = sys.argv[3]
        cell = 18  # 16 px metatile plus a 2 px gap
        width = height = 16 * cell
        pixels = [(255, 0, 255)] * (width * height)
        for n in range(256):
            draw_metatile(pixels, width, (n % 16) * cell, (n // 16) * cell, vram, colors, mappings, n)
        write_bmp(out, width, height, pixels)
    elif mode == "map":
        x0, y0, w, h = (int(v) for v in sys.argv[3:7])
        out = sys.argv[7]
        width, height = w * SCREEN_W * 16, h * SCREEN_H * 16
        pixels = [(0, 0, 0)] * (width * height)
        for sy in range(h):
            for sx in range(w):
                screen = read_screen(disasm, x0 + sx, y0 + sy)
                for i, n in enumerate(screen):
                    px = (sx * SCREEN_W + i % SCREEN_W) * 16
                    py = (sy * SCREEN_H + i // SCREEN_W) * 16
                    draw_metatile(pixels, width, px, py, vram, colors, mappings, n)
        write_bmp(out, width, height, pixels)


if __name__ == "__main__":
    main()
