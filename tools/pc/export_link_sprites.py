"""Exports Link's sprites, with his swords and items, from the USA ROM into a
sprite atlas (PNG) and a text file that describes its layout and animations.

usage: export_link_sprites.py baserom.gba include/player.h out_dir

Writes out_dir/link_sprites.png and out_dir/link_sprites.txt; the format is
described at the top of the text file.

Every image is drawn unflipped in a cell of the same size, with the entity's
position (the point the game draws it at) at the same place in every cell.
Items are separate entities the game draws at Link's position, so their cells
line up with his. The animations come from the ROM; their names come from the
PlayerAnimation enum (include/player.h). Animations without a name there are
left out. The rules for the items' frames and flips follow their code in
src/playerItem/.
"""

import re
import sys

from tmc_sprites import Rom, Sprite, encode_png, obj_palette

LINK_PALETTE = 0x16  # GetPlayerPalette: the normal palette, not hurt or charmed
FIXED_PALETTE_SLOTS = 5  # palette group 11 loads pal_0 to pal_4 at OBJ slots 0-4
COLUMNS = 32
DIRECTIONS = "NESW"


def link_animations(rom, player_h):
    """(name, sprite, animation, direction or None) for each named animation.
    Animations below the direction threshold come in groups of 4 (north,
    east, south, west), as UpdatePlayerAnimation picks them (playerUtils.c)."""
    names = {int(v, 16): n[5:] for n, v in re.findall(r"\b(ANIM_\w+) = 0x([0-9a-fA-F]+)", open(player_h).read())}
    out = []
    for value, name in sorted(names.items()):
        sprite, anim = value >> 8, value & 0xFF
        threshold = 0x18 if sprite == 12 else 0x34 if sprite == 7 else 0xB8
        count = Sprite(rom, sprite).animation_count()
        grouped = anim % 4 == 0 and anim < threshold and not any(value + k in names for k in (1, 2, 3))
        if grouped and anim + 3 < count:
            out += [("%s_%s" % (name, DIRECTIONS[k]), sprite, anim + k, k) for k in range(4)]
        elif anim < count:
            out.append((name, sprite, anim, None))
    return out


def is_sword_animation(name):
    return not name.startswith("GET_") and any(
        word in name for word in ("SWORD", "SPINATTACK", "GREATSPIN", "ROLLATTACK", "DOWN_THRUST")
    )


def flip_like_link(link_flip, direction):
    return link_flip


def flip_against_link(link_flip, direction):
    # Sword, dash sword, Pacci cane, boomerang: flipped again facing east or west.
    return link_flip ^ (direction is not None and direction & 1)


def bow_flip(link_flip, direction):
    # gUnk_080B3E40 (playerItemBow.c): only facing east.
    return int(direction == 1)


def sword_frame(name, sprite, frame, flags):
    if name.startswith("DASH") and sprite == 2:
        return frame + 0x2E  # PlayerItemDashSword
    if sprite in (1, 4) and is_sword_animation(name) and frame >= 0x6A:
        return frame - 0x6A  # sub_080A78B8
    return None


def shield_frame(offset):
    def rule(name, sprite, frame, flags):
        if sprite in (1, 4) and 0 <= frame - 0x25 < 0x3F:
            return frame - 0x25 + offset  # PlayerItemShield
        return None

    return rule


def lantern_frame(name, sprite, frame, flags):
    if name.startswith("LANTERN") and sprite == 6 and frame < 0x37:
        return frame
    return None


def bow_frame(name, sprite, frame, flags):
    return frame if name.startswith("BOW_") else None


def boomerang_frame(name, sprite, frame, flags):
    if name.startswith("BOOMERANG") and not flags & 1:
        return (flags >> 4) + (6 if "MAGIC" in name else 0)
    return None


def cane_frame(name, sprite, frame, flags):
    if name.startswith("CANE") and flags & 0xF != 0xF:
        return (frame + 0x6F) & 0xFF
    return None


# Layer name, item sprite, OBJ palette slot, frame rule, flip rule. Palettes
# and sprites come from gPlayerItemDefinitions (playerItemDefinitions.c).
SWORDS = (("smith", 14, 1), ("green", 15, 2), ("red", 15, 0), ("blue", 15, 1), ("four", 16, 4))
LAYERS = [("sword_%s" % n, s, p, sword_frame, flip_against_link) for n, s, p in SWORDS] + [
    ("shield", 17, 1, shield_frame(0), flip_like_link),
    ("mirror_shield", 17, 1, shield_frame(0x27), flip_like_link),
    ("lantern", 18, 0, lantern_frame, flip_like_link),
    ("bow", 21, 1, bow_frame, bow_flip),
    ("boomerang", 22, 0, boomerang_frame, flip_against_link),
    ("pacci_cane", 20, 4, cane_frame, flip_against_link),
]
# The Gust Jar plays animations of its own (PlayerItemGustJar), so they are
# exported as animations of its layer.
GUST_JAR = ("gust_jar", 23, 1)


class Atlas:
    def __init__(self, rom):
        self.rom = rom
        self.fixed = {i: obj_palette(rom, i) for i in range(FIXED_PALETTE_SLOTS)}
        self.cells = {}  # (layer, sprite, frame) -> cell number
        self.images = []  # (layer, sprite, frame, pixels)

    def cell(self, layer, sprite, frame, palette):
        key = (layer, sprite, frame)
        if key not in self.cells:
            colors = self.fixed[palette] if palette < FIXED_PALETTE_SLOTS else obj_palette(self.rom, palette)
            pixels = Sprite(self.rom, sprite).draw(frame, colors, self.fixed)
            self.cells[key] = None
            if pixels:
                self.cells[key] = len(self.images)
                self.images.append((layer, sprite, frame, pixels))
        return self.cells[key]

    def layout(self):
        """Orders the cells by layer and places them in rows. Returns the cell
        size, the origin in a cell and each cell's (column, row)."""
        xs = [x for *_, p in self.images for x, _ in p]
        ys = [y for *_, p in self.images for _, y in p]
        half = max(-min(xs), max(xs) + 1)
        width, height = 2 * half, max(ys) + 1 - min(ys)
        order = sorted(range(len(self.images)), key=lambda i: (self.layer_order(self.images[i][0]), i))
        places, row, col, last = {}, 0, 0, None
        for i in order:
            layer = self.images[i][0]
            if last is not None and (layer != last or col == COLUMNS):
                row, col = row + 1, 0
            places[i] = (col, row)
            col, last = col + 1, layer
        return (width, height), (half, -min(ys)), places

    @staticmethod
    def layer_order(layer):
        return 0 if layer == "body" else 1 + [l[0] for l in LAYERS + [GUST_JAR]].index(layer)

    def render(self, size, origin, places):
        rows = max(r for _, r in places.values()) + 1
        canvas = [[(0, 0, 0, 0)] * (COLUMNS * size[0]) for _ in range(rows * size[1])]
        for i, (_, _, _, pixels) in enumerate(self.images):
            col, row = places[i]
            for (x, y), color in pixels.items():
                canvas[row * size[1] + origin[1] + y][col * size[0] + origin[0] + x] = color
        return canvas


HEADER = """\
# Link's sprites from The Legend of Zelda: The Minish Cap (USA), exported
# from baserom.gba by tools/pc/export_link_sprites.py.
#
# link_sprites.png is a grid of cells, all the same size. Each cell holds one
# frame of Link (layer "body") or of an item he holds (the other layers),
# drawn unflipped. The "origin" is Link's position: the point the game draws
# him and his items at. It is at the same place in every cell, so drawing a
# body cell and item cells at the same position puts them together as in the
# game.
#
# Flipped frames: mirror the whole cell horizontally. The origin is at the
# exact horizontal center of the cell, so it stays in place.
#
# Durations are in game frames (the GBA runs at 59.73 frames per second).
#
# Lines:
#   cell_size <width> <height>
#   origin <x> <y>                     the origin, from a cell's top left corner
#   columns <n>                        cells per row of the image
#   layer <name> <first row> <rows>    rows of the image that hold that layer
#   cell <number> <layer> <column> <row> <sprite> <frame>
#       a cell; sprite and frame are the game's sprite and frame indexes
#   animation <name> <sprite> <index> <keyframes> <loop>
#       one animation, followed by its keyframes. Names end in _N, _E, _S
#       or _W when the game picks the animation by Link's direction. After
#       the last keyframe, the animation continues at keyframe <loop>
#       (counted from 0); when that is the last one, it holds it.
#   key <duration> <body cell> <flip> <flags> [<layer>=<cell>,<front|back>,<flip> ...]
#       a keyframe. <flip> is "x" to mirror the cell or "-". <body cell> is
#       "-" when Link is not drawn. <flags> is the game's frame byte (hex),
#       which the game uses to time events such as a sword's hit. Then, for
#       each item layer that has a frame here, its cell, whether it is drawn
#       in front of Link or behind him, and its flip. An item is only shown
#       when Link holds it: show the layer of the item in use.
#
# The Gust Jar animates on its own, alongside Link's GUSTJAR_* animations:
# its animations are GUST_JAR_ITEM_<n>, whose keys hold its cell in place of
# the body cell (numbered as in the game; 0-3 and 8-11 are by direction,
# N E S W). The game draws it in front of Link on frames that draw items in
# front. While it sucks, it plays frame lists of its own (gUnk_08132714)
# that are not exported.
#
# Swords: the five sword layers are the same frames in the colors of the
# Smith's, White (green), White (red), White (blue) and Four Sword.
# The palettes are the normal ones: the game changes Link's palette when he
# is hurt or charmed.
"""


def main():
    rom_path, player_h, out_dir = sys.argv[1:]
    rom = Rom(rom_path)
    atlas = Atlas(rom)
    animations = []
    for name, sprite_index, anim, direction in link_animations(rom, player_h):
        sprite = Sprite(rom, sprite_index)
        keys, loop = sprite.keyframes(anim)
        link_flip = 1 if direction == 3 else 0  # UpdatePlayerAnimation flips Link facing west
        out = []
        for frame, duration, settings, flags in keys:
            body = atlas.cell("body", sprite_index, frame, LINK_PALETTE) if frame != 0xFF else None
            flip = link_flip ^ bool(settings & 0x40)
            items = []
            if frame != 0xFF and body is not None:
                front = sprite.frame_info(frame)[1] != 0
                for layer, item_sprite, palette, frame_rule, flip_rule in LAYERS:
                    item_frame = frame_rule(name, sprite_index, frame, flags)
                    if item_frame is None or not 0 <= item_frame < 0xFF:
                        continue
                    cell = atlas.cell(layer, item_sprite, item_frame, palette)
                    if cell is not None:
                        items.append((layer, cell, front, flip_rule(link_flip, direction)))
            out.append((duration, body, flip, flags, items))
        animations.append((name, sprite_index, anim, out, loop))

    layer, sprite_index, palette = GUST_JAR
    sprite = Sprite(rom, sprite_index)
    for anim in range(sprite.animation_count()):
        keys, loop = sprite.keyframes(anim)
        out = [
            (duration, atlas.cell(layer, sprite_index, frame, palette) if frame != 0xFF else None, settings & 0x40, flags, [])
            for frame, duration, settings, flags in keys
        ]
        animations.append(("GUST_JAR_ITEM_%d" % anim, sprite_index, anim, out, loop))

    size, origin, places = atlas.layout()
    with open("%s/link_sprites.png" % out_dir, "wb") as f:
        f.write(encode_png(atlas.render(size, origin, places)))

    lines = [HEADER, "cell_size %d %d" % size, "origin %d %d" % origin, "columns %d" % COLUMNS, ""]
    for layer in ["body"] + [l[0] for l in LAYERS] + [GUST_JAR[0]]:
        rows = sorted({places[i][1] for i, image in enumerate(atlas.images) if image[0] == layer})
        if rows:
            lines.append("layer %s %d %d" % (layer, rows[0], len(rows)))
    lines.append("")
    for i in sorted(places, key=lambda i: (places[i][1], places[i][0])):
        layer, sprite_index, frame, _ = atlas.images[i]
        lines.append("cell %d %s %d %d %d %d" % (i, layer, places[i][0], places[i][1], sprite_index, frame))
    for name, sprite_index, anim, keys, loop in animations:
        lines += ["", "animation %s %d %d %d %d" % (name, sprite_index, anim, len(keys), loop)]
        for duration, body, flip, flags, items in keys:
            line = "key %d %s %s %02x" % (duration, "-" if body is None else body, "x" if flip else "-", flags)
            for layer, cell, front, item_flip in items:
                line += " %s=%d,%s,%s" % (layer, cell, "front" if front else "back", "x" if item_flip else "-")
            lines.append(line)
    with open("%s/link_sprites.txt" % out_dir, "w") as f:
        f.write("\n".join(lines) + "\n")
    print("%d cells of %dx%d, %d animations" % (len(atlas.images), size[0], size[1], len(animations)))


if __name__ == "__main__":
    main()
