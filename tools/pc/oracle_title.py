#!/usr/bin/env python3
"""Compose an Oracle of Ages/Seasons title screen from oracles-disasm data.

Reproduces what the game's GFXH_TITLESCREEN graphics header and PALH_03 palette
header load: tile sheets and the background map (tile indices in VRAM bank 0,
attributes in bank 1), drawn with the GBC's signed tile addressing at $9000
(LCDC $C7). Sprites ("Press Start" etc.) are left out.

Usage: oracle_title.py <oracles-disasm dir> <ages|seasons> <out.bmp>
"""

import os
import sys

from gbc_gfx import Vram, draw_tile, header_entries, palette_colors, write_bmp

WIDTH, HEIGHT = 160, 144


def main():
    disasm, game, out = sys.argv[1:]

    vram = Vram()
    vram.load(disasm, game, header_entries(os.path.join(disasm, "data", game, "gfxHeaders.s"), "m_GfxHeaderStart", "GFXH_TITLESCREEN"))
    colors = palette_colors(disasm, game, "paletteData4018", 32)

    pixels = [(0, 0, 0)] * (WIDTH * HEIGHT)
    for ty in range(HEIGHT // 8):
        for tx in range(WIDTH // 8):
            offset = 0x1800 + ty * 32 + tx  # map at $9800
            index, attr = vram.data[0][offset], vram.data[1][offset]
            draw_tile(pixels, WIDTH, tx * 8, ty * 8, vram.bg_tile(index, attr), attr, colors)
    write_bmp(out, WIDTH, HEIGHT, pixels)


if __name__ == "__main__":
    main()
