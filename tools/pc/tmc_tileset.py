#!/usr/bin/env python3
"""Render a Minish Cap room from a TMC_DUMP_BG dump (see src/platform/pc/oracle.c).

  tiles <dump> <out.bmp>   every tile index used in the bottom layer, 16 per row in
                           order of index; prints "<row>,<col> index=<n> uses=<count>
                           collision=<c> act=<a> type=<t>" per tile
  room <dump> <out.bmp>    the whole room, bottom layer then top layer on top
"""

import sys
from collections import Counter

from gbc_gfx import write_bmp
from tmc_dump import Dump


def draw(pixels, width, x0, y0, tile):
    for i, p in enumerate(tile):
        if p is not None:
            pixels[(y0 + i // 16) * width + x0 + i % 16] = p


def main():
    mode, path, out = sys.argv[1:]
    dump = Dump(path)
    bottom = dump.layers[0]

    if mode == "tiles":
        used = Counter()
        info = {}
        for y in range(dump.height):
            for x in range(dump.width):
                index = bottom.map[y * 64 + x]
                if index < 2048:
                    used[index] += 1
                    info[index] = (bottom.collision[y * 64 + x], bottom.act_tiles[y * 64 + x])
        indices = sorted(used)
        cell, cols = 18, 16
        w, h = cols * cell, (len(indices) + cols - 1) // cols * cell
        pixels = [(255, 0, 255)] * (w * h)
        for n, index in enumerate(indices):
            draw(pixels, w, (n % cols) * cell, (n // cols) * cell, dump.tile_pixels(0, index))
            print("%d,%d index=%d uses=%d collision=%d act=%d type=%d" % (
                n // cols, n % cols, index, used[index], info[index][0], info[index][1], bottom.tile_types[index]))
        write_bmp(out, w, h, pixels)
    elif mode == "room":
        w, h = dump.width * 16, dump.height * 16
        pixels = [dump.backdrop()] * (w * h)
        cache = {}
        for layer_index in (0, 1):
            layer = dump.layers[layer_index]
            for y in range(dump.height):
                for x in range(dump.width):
                    index = layer.map[y * 64 + x]
                    if index >= 2048:
                        continue
                    key = (layer_index, index)
                    if key not in cache:
                        cache[key] = dump.tile_pixels(layer_index, index)
                    draw(pixels, w, x * 16, y * 16, cache[key])
        write_bmp(out, w, h, pixels)


if __name__ == "__main__":
    main()
