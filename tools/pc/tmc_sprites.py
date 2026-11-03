"""Reads sprites from the USA ROM (baserom.gba): the sprite table, animations,
frame tiles, OAM piece lists and palettes, and draws frames the way the game
does (DrawFrameObjects in src/platform/pc/asm_intr.c).
"""

import struct
import zlib

ROM_BASE = 0x08000000
SPRITE_PTRS = 0x080029B4  # gSpritePtrs: animations, frames, tiles, pad
FRAME_OBJ_LISTS = 0x082F3D74  # gFrameObjLists
OBJ_SIZES = 0x080B2BE8  # x and y anchor, width, height by shape and size
GFX_AND_PALETTES = 0x085A2E80  # gGlobalGfxAndPalettes
OBJ_PALETTES = 0x08133368  # gUnk_08133368: palettes of objPaletteIds 0x16 and up


class Rom:
    def __init__(self, path):
        self.data = open(path, "rb").read()
        if self.data[0xAC:0xB0] != b"BZME":
            raise SystemExit("%s is not the USA ROM (BZME)" % path)

    def u8(self, addr):
        return self.data[addr - ROM_BASE]

    def u16(self, addr):
        return struct.unpack_from("<H", self.data, addr - ROM_BASE)[0]

    def u32(self, addr):
        return struct.unpack_from("<I", self.data, addr - ROM_BASE)[0]

    def bytes(self, addr, n):
        return self.data[addr - ROM_BASE : addr - ROM_BASE + n]

    def is_ptr(self, value):
        return ROM_BASE <= value < ROM_BASE + len(self.data)


def bgr555(color):
    return ((color & 31) * 255 // 31, ((color >> 5) & 31) * 255 // 31, ((color >> 10) & 31) * 255 // 31, 255)


def obj_palette(rom, palette_id):
    """The 16 colors that LoadObjPalette (color.c) loads for an objPaletteId;
    color 0 is transparent. 0-4 are the fixed palettes of palette group 11,
    pal_0 to pal_4."""
    if palette_id < 0x16:
        offset = palette_id * 32
    else:
        offset = rom.u32(OBJ_PALETTES + (palette_id - 0x16) * 4) & 0xFFFFFF
    colors = [bgr555(rom.u16(GFX_AND_PALETTES + offset + i * 2)) for i in range(16)]
    colors[0] = (0, 0, 0, 0)
    return colors


class Sprite:
    def __init__(self, rom, index):
        self.rom = rom
        self.index = index
        base = SPRITE_PTRS + index * 16
        self.animations_ptr = rom.u32(base)
        self.frames_ptr = rom.u32(base + 4)
        self.tiles_ptr = rom.u32(base + 8)

    def animation_count(self):
        if not self.animations_ptr:
            return 0
        n = 0
        while n < 256 and self.rom.is_ptr(self.rom.u32(self.animations_ptr + n * 4)):
            n += 1
        return n

    def keyframes(self, anim):
        """The keyframes of an animation as (frame index, duration, flip bits
        (0x40 x, 0x80 y), frame byte), and the keyframe it continues with after
        the last one (InitializeAnimation and FrameZero, asm_08003FC4.c)."""
        addr = self.rom.u32(self.animations_ptr + anim * 4)
        keys = []
        while len(keys) < 256:
            frame, duration, settings, flags = self.rom.bytes(addr, 4)
            keys.append((frame, duration, settings & 0xC0, flags))
            if flags & 0x80:
                return keys, len(keys) - self.rom.u8(addr + 4)
            addr += 4
        return keys, len(keys) - 1

    def frame_info(self, frame):
        """(tile count, byte 1) of a frame. Byte 1 is nonzero for frames that
        draw an attached item in front of the entity (sub_08078E84)."""
        count, unk = self.rom.bytes(self.frames_ptr + frame * 4, 2)
        return count, unk

    def draw(self, frame, colors, fixed):
        """Draws a frame, unflipped, as {(x, y): RGBA} relative to the entity's
        position, or None if it has no tiles. colors is the entity's palette;
        fixed maps palette slots to colors for pieces with their own palette."""
        if not self.frames_ptr or frame == 0xFF:
            return None
        count, _, first = struct.unpack("<BBH", self.rom.bytes(self.frames_ptr + frame * 4, 4))
        if count == 0:
            return None
        tiles = self.rom.bytes(self.tiles_ptr + first * 32, count * 32)
        lists = FRAME_OBJ_LISTS + self.rom.u32(FRAME_OBJ_LISTS + self.index * 4)
        addr = FRAME_OBJ_LISTS + self.rom.u32(lists + frame * 4)
        pixels = {}
        # Earlier pieces are drawn on top, as in OAM.
        for i in reversed(range(self.rom.u8(addr))):
            px, py, flags, lo, hi = struct.unpack("<bbBBB", self.rom.bytes(addr + 1 + i * 5, 5))
            ax, ay, width, height = self.rom.bytes(OBJ_SIZES + ((flags & 0xF0) >> 2), 4)
            hflip, vflip = flags & 4, flags & 8
            tile = (lo | hi << 8) & 0x3FF
            # The piece's palette is relative to the entity's, or its own (flag 1).
            palette = fixed.get(hi >> 4, colors) if flags & 1 or hi >> 4 else colors
            for y in range(height):
                for x in range(width):
                    i = (tile + (y // 8) * (width // 8) + x // 8) * 32 + (y % 8) * 4 + (x % 8) // 2
                    if i >= len(tiles):
                        continue
                    index = tiles[i] >> 4 if x & 1 else tiles[i] & 15
                    if index:
                        sx = width - 1 - x if hflip else x
                        sy = height - 1 - y if vflip else y
                        pixels[px - ax + sx, py - ay + sy] = palette[index]
        return pixels


def encode_png(rows):
    """An 8-bit RGBA PNG of rows of RGBA tuples."""
    height, width = len(rows), len(rows[0])
    raw = b"".join(b"\0" + bytes(c for pixel in row for c in pixel) for row in rows)

    def chunk(kind, data):
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data))

    header = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", header) + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")
