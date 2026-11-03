#!/usr/bin/env python3
"""Convert Oracle of Ages' Lynna City to Minish Cap map data, with Oracle's art.

Converts the Lynna City tileset (tileset 00: GFXH_TILESET_OVERWORLD_PRESENT,
UNIQUE_GFXH_LYNNA_CITY_1, BG palettes 48e0, 4928 and PALH_TILESET_LYNNA_CITY)
from Game Boy Color to GBA format. The map is scaled up by SCALE_NUM/SCALE_DEN
(nearest neighbour), so a 16px Oracle metatile covers 3x3 GBA tiles, and cropped
to MAX_MAP_SIZE. Writes a C file with:

- gPcOracleTiles: the 512 GBC tiles in 4bpp. GBC VRAM bank b, tile slot s
  ($8800-$97ff, slots 128-383) becomes tile b * 256 + s - 128. GBC color c
  becomes c + 1, so every pixel is opaque. These are the scaled tiles' source.
- gPcOracleScaledTiles: per scaled tile (BG character blocks 0-1), per pixel the
  source pixel as (source tile << 8 | pixel << 2 | slot): the pixel's color is
  the source color plus 4 * slot, the place of its GBC palette in the scaled
  tile's GBA palette. Tile ORACLE_BLANK_TILE (block 1's first tile) stays empty
  for the unused top layer. Rebuilt by oracle.c whenever an animation changes
  source tiles.
- gPcOraclePalettes: GBA BG palettes 1-8, each up to 3 of the 8 GBC BG palettes,
  so that every scaled tile's GBC palettes are in one GBA palette.
- gPcOracleSubTiles: per metatile, the 4 GBA screen entries.
- gPcOracleMap: the metatile index of each map tile; gPcOracleCollision: its
  collision (Oracle and Minish Cap use the same quadrant bits below 0x10);
  gPcOracleActTiles: Minish Cap act tiles (deep water).
- Tile animations (water, waterfalls) of animation group 00, on the source
  tiles: gPcOracleAnimTiles holds the frames' tiles; gPcOracleAnimHeaders is
  (first source tile, tile count, offset in gPcOracleAnimTiles in tiles) per
  graphics header; each animation in gPcOracleAnims is a list of (frames,
  header) steps ending with 0.

Used by src/platform/pc/oracle.c.

Usage: oracle_map.py <oracles-disasm dir> <out.c>
"""

import os
import re
import sys

from gbc_gfx import Vram, header_entries, palette_rgb5, read_indexed_png
from oracle_lynna_tiles import (
    ANIMATION_GROUP,
    MAX_MAP_SIZE,
    SCALE_DEN,
    SCALE_NUM,
    SCREENS_X,
    SCREENS_Y,
    TARGET_AREA,
    TARGET_ROOM,
    SOLID_SPECIAL_COLLISIONS,
    WATER_METATILES,
)

GAME = "ages"
SCREEN_W, SCREEN_H = 10, 8
FIRST_PALETTE = 1  # first GBA BG palette
SOLID = 0xF
ACT_TILE_WATER = 16  # Minish Cap deep water
SCALED_TILES = 1024  # BG character blocks 0-1
BLANK_TILE = 512  # first tile of block 1, used by the (empty) top layer
TILESET_SIZE = 2048  # Minish Cap metatiles per layer


def gbc_palettes(disasm):
    """The 8 BG palettes Lynna City uses, as 32 5-bit RGB colors."""
    return (
        palette_rgb5(disasm, GAME, "paletteData48e0", 4)
        + palette_rgb5(disasm, GAME, "paletteData4928", 4)
        + palette_rgb5(disasm, GAME, "paletteData4a30", 24)
    )


def gba_tile(vram, bank, slot):
    """4bpp GBA tile bytes for a GBC tile."""
    pixels = vram.tiles.get((bank, slot))
    out = bytearray(32)
    for y in range(8):
        for x in range(8):
            color = (pixels[y][x] if pixels else 0) + 1
            out[y * 4 + x // 2] |= color << (4 * (x & 1))
    return out


def gbc_to_gba_tile(pixels):
    out = bytearray(32)
    for y in range(8):
        for x in range(8):
            out[y * 4 + x // 2] |= (pixels[y][x] + 1) << (4 * (x & 1))
    return out


def gba_tile_number(dest):
    """GBA tile number for a GBC VRAM destination ($addr + bank)."""
    return (dest & 1) * 256 + (dest & ~0xF) // 16 - 0x800 - 128


def tile_animations(disasm, group):
    """Returns (tiles, headers, animations) for an animation group (see module doc)."""
    data = os.path.join(disasm, "data", GAME)
    groups = open(os.path.join(data, "animationGroups.s")).read()
    body = re.search(r"animationGroup%02x:.*?\n(.*?)(?=\nanimationGroup|\Z)" % group, groups, re.S).group(1)
    names = re.findall(r"\.dw (\w+)", body)

    anim_text = open(os.path.join(data, "animationData.s")).read()
    header_list = re.findall(
        r"m_GfxHeaderAnim (\w+), \$([0-9a-fA-F]+), \$([0-9a-fA-F]+), \$([0-9a-fA-F]+)",
        open(os.path.join(data, "animationGfxHeaders.s")).read(),
    )

    sources = {}
    tiles = bytearray()
    headers = []
    header_slot = {}  # header index -> index in headers
    animations = []
    for name in names:
        body = re.search(r"^%s:.*?\n(.*?)m_AnimationLoop" % name, anim_text, re.S | re.M).group(1)
        steps = []
        for delay, header in re.findall(r"\.db \$([0-9a-fA-F]+) \$([0-9a-fA-F]+)", body):
            header = int(header, 16)
            if header not in header_slot:
                source, dest, count, offset = header_list[header]
                if source not in sources:
                    w, h, px = read_indexed_png(os.path.join(disasm, "gfx", GAME, source + ".png"))
                    sources[source] = [
                        [px[(ty * 8 + y) * w + tx * 8 : (ty * 8 + y) * w + tx * 8 + 8] for y in range(8)]
                        for ty in range(h // 8)
                        for tx in range(w // 8)
                    ]
                first, count = int(offset, 16) // 16, int(count, 16)
                header_slot[header] = len(headers)
                headers.append((gba_tile_number(int(dest, 16)), count, len(tiles) // 32))
                for t in sources[source][first : first + count]:
                    tiles += gbc_to_gba_tile(t)
            steps.append((int(delay, 16), header_slot[header]))
        animations.append(steps)
    return tiles, headers, animations


def main():
    disasm, out = sys.argv[1:]
    data = os.path.join(disasm, "data", GAME)

    vram = Vram()
    vram.load(disasm, GAME, header_entries(os.path.join(data, "gfxHeaders.s"), "m_GfxHeaderStart", "GFXH_TILESET_OVERWORLD_PRESENT"))
    vram.load(disasm, GAME, header_entries(os.path.join(data, "uniqueGfxHeaders.s"), "m_UniqueGfxHeaderStart", "UNIQUE_GFXH_LYNNA_CITY_1"))

    tiles = bytearray()
    for bank in (0, 1):
        for slot in range(128, 384):
            tiles += gba_tile(vram, bank, slot)

    # Per metatile, its 4 source tiles as (tile, horizontal flip, vertical flip, GBC palette).
    mappings = open(os.path.join(disasm, "tileset_layouts", GAME, "tilesetMappings00.bin"), "rb").read()
    source_tiles = []
    for n in range(256):
        entry = mappings[n * 8 : n * 8 + 8]
        quadrants = []
        for i in range(4):
            index, attr = entry[i], entry[4 + i]
            slot = 256 + (index - 256 if index >= 128 else index)  # signed $9000 addressing
            quadrants.append((((attr >> 3) & 1) * 256 + slot - 128, attr & 0x20, attr & 0x40, attr & 7))
        source_tiles.append(quadrants)

    collisions = open(os.path.join(disasm, "tileset_layouts", GAME, "tilesetCollisions00.bin"), "rb").read()

    def collision(metatile):
        value = collisions[metatile]
        if metatile in WATER_METATILES:
            return 0
        if value < 0x10:
            return value
        return SOLID if value in SOLID_SPECIAL_COLLISIONS else 0

    src_width, src_height = len(SCREENS_X) * SCREEN_W, len(SCREENS_Y) * SCREEN_H
    src_map = [0] * (src_width * src_height)
    for sy, screen_y in enumerate(SCREENS_Y):
        for sx, screen_x in enumerate(SCREENS_X):
            path = os.path.join(disasm, "rooms", GAME, "small", "room00%02x.bin" % (screen_y * 16 + screen_x))
            for i, n in enumerate(open(path, "rb").read()):
                src_map[(sy * SCREEN_H + i // SCREEN_W) * src_width + sx * SCREEN_W + i % SCREEN_W] = n

    width = min(src_width * SCALE_NUM // SCALE_DEN, MAX_MAP_SIZE)
    height = min(src_height * SCALE_NUM // SCALE_DEN, MAX_MAP_SIZE)

    def source(x, y):
        """Source metatile and its pixel for a pixel of the scaled map."""
        x, y = x * SCALE_DEN // SCALE_NUM, y * SCALE_DEN // SCALE_NUM
        return src_map[(y // 16) * src_width + x // 16], x % 16, y % 16

    def source_pixel(x, y):
        """(source tile, pixel in it, GBC palette) for a pixel of the scaled map."""
        metatile, px, py = source(x, y)
        tile, hflip, vflip, palette = source_tiles[metatile][(py // 8) * 2 + px // 8]
        px, py = 7 - px % 8 if hflip else px % 8, 7 - py % 8 if vflip else py % 8
        return tile, py * 8 + px, palette

    # The scaled map's GBA tiles, as source pixels.
    pixel_tiles = {}
    for ty in range(height * 2):
        for tx in range(width * 2):
            pixel_tiles[tx, ty] = [source_pixel(tx * 8 + x, ty * 8 + y) for y in range(8) for x in range(8)]

    # Pack the GBC palettes into GBA palettes, each holding all of some tiles' GBC palettes.
    groups = []
    for needed in sorted({frozenset(p for _, _, p in t) for t in pixel_tiles.values()}, key=len, reverse=True):
        group = next((g for g in groups if needed <= set(g) or len(set(g) | needed) <= 3), None)
        if group is None:
            group = []
            groups.append(group)
        group += sorted(needed - set(group))
    assert len(groups) <= 8, "%d GBA palettes needed" % len(groups)
    gbc = gbc_palettes(disasm)
    palettes = []
    for group in groups + [[]] * (8 - len(groups)):
        colors = [c for p in group for c in gbc[p * 4 : p * 4 + 4]]
        palettes += [0] + [r | (g << 5) | (b << 10) for r, g, b in colors] + [0] * (15 - len(colors))

    # Unique scaled tiles (also flipped), and per metatile its 4 screen entries.
    scaled_tiles = []
    tile_entries = {}  # tile recipe -> screen entry

    def screen_entry(tile):
        group = next(g for g, members in enumerate(groups) if {p for _, _, p in tile} <= set(members))
        recipe = tuple(t << 8 | pixel << 2 | groups[group].index(p) for t, pixel, p in tile)
        if recipe not in tile_entries:
            if len(scaled_tiles) == BLANK_TILE:
                scaled_tiles.append((0,) * 64)
            number = len(scaled_tiles)
            assert number < SCALED_TILES, "too many scaled tiles"
            scaled_tiles.append(recipe)
            rows = [recipe[y * 8 : y * 8 + 8] for y in range(8)]
            for flip, flipped in (
                (0, rows),
                (0x400, [r[::-1] for r in rows]),
                (0x800, rows[::-1]),
                (0xC00, [r[::-1] for r in rows[::-1]]),
            ):
                tile_entries.setdefault(tuple(v for r in flipped for v in r), number | flip)
        return tile_entries[recipe] | (FIRST_PALETTE + group) << 12

    metatile_numbers = {}
    sub_tiles = []
    metatiles = []
    for y in range(height):
        for x in range(width):
            entries = tuple(screen_entry(pixel_tiles[x * 2 + q % 2, y * 2 + q // 2]) for q in range(4))
            if entries not in metatile_numbers:
                metatile_numbers[entries] = len(metatile_numbers)
                sub_tiles += entries
            metatiles.append(metatile_numbers[entries])
    assert len(metatile_numbers) <= TILESET_SIZE, "too many metatiles"

    # Collision of each quadrant, and act tiles, from the source at its middle.
    solid = []
    act_tiles = []
    for y in range(height):
        for x in range(width):
            value = 0
            for q in range(4):
                metatile, px, py = source(x * 16 + (q % 2) * 8 + 4, y * 16 + (q // 2) * 8 + 4)
                if collision(metatile) & (8 >> ((py // 8) * 2 + px // 8)):
                    value |= 8 >> q
            solid.append(value)
            act_tiles.append(ACT_TILE_WATER if source(x * 16 + 8, y * 16 + 8)[0] in WATER_METATILES else 0)

    # Spawn on the walkable tile nearest the middle.
    cx, cy = width // 2, height // 2
    spawn = min(
        (
            (x, y)
            for y in range(1, height - 1)
            for x in range(1, width - 1)
            if solid[y * width + x] == 0 and act_tiles[y * width + x] == 0
        ),
        key=lambda p: (p[0] - cx) ** 2 + (p[1] - cy) ** 2,
    )

    def array(values, per_row):
        return ",\n".join(
            "    " + ", ".join(str(v) for v in values[i : i + per_row]) for i in range(0, len(values), per_row)
        )

    with open(out, "w") as f:
        f.write("// Generated by tools/pc/oracle_map.py from oracles-disasm. Do not edit.\n\n")
        f.write("const unsigned char gPcOracleArea = %d;\n" % TARGET_AREA)
        f.write("const unsigned char gPcOracleRoom = %d;\n" % TARGET_ROOM)
        f.write("const unsigned short gPcOracleWidth = %d;\n" % width)
        f.write("const unsigned short gPcOracleHeight = %d;\n" % height)
        f.write("const unsigned short gPcOracleSpawnX = %d;\n" % (spawn[0] * 16 + 8))
        f.write("const unsigned short gPcOracleSpawnY = %d;\n" % (spawn[1] * 16 + 8))
        f.write("const unsigned char gPcOracleTiles[] = {\n%s\n};\n" % array(tiles, 32))
        f.write("const unsigned short gPcOracleNumScaledTiles = %d;\n" % len(scaled_tiles))
        f.write("const unsigned int gPcOracleScaledTiles[] = {\n%s\n};\n" % array([v for t in scaled_tiles for v in t], 8))
        f.write("const unsigned short gPcOracleNumMetatiles = %d;\n" % len(metatile_numbers))
        f.write("const unsigned short gPcOraclePalettes[] = {\n%s\n};\n" % array(palettes, 16))
        f.write("const unsigned short gPcOracleSubTiles[] = {\n%s\n};\n" % array(sub_tiles, 4))
        f.write("const unsigned short gPcOracleMap[] = {\n%s\n};\n" % array(metatiles, width))
        f.write("const unsigned char gPcOracleCollision[] = {\n%s\n};\n" % array(solid, width))
        f.write("const unsigned char gPcOracleActTiles[] = {\n%s\n};\n" % array(act_tiles, width))
        anim_tiles, anim_headers, animations = tile_animations(disasm, ANIMATION_GROUP)
        f.write("const unsigned char gPcOracleAnimTiles[] = {\n%s\n};\n" % array(anim_tiles, 32))
        f.write("const unsigned short gPcOracleAnimHeaders[] = {\n%s\n};\n" % array([v for h in anim_headers for v in h], 3))
        steps = []
        for animation in animations:
            steps += [v for step in animation for v in step] + [0]
        f.write("const unsigned char gPcOracleAnims[] = {\n%s\n};\n" % array(steps + [0], 16))
        f.write("const unsigned char gPcOracleNumAnims = %d;\n" % len(animations))


if __name__ == "__main__":
    main()
