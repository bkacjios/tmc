"""Reader for TMC_DUMP_BG dumps (see PcDumpBgState in src/platform/pc/oracle.c)."""

import struct

TILESET_SIZE = 2048
MAP_TILES = 64 * 64


class Layer:
    pass


class Dump:
    def __init__(self, path):
        data = open(path, "rb").read()
        self.pos = 0

        def take(n):
            chunk = data[self.pos : self.pos + n]
            self.pos += n
            return chunk

        self.layers = []
        for _ in range(2):
            layer = Layer()
            layer.sub_tiles = struct.unpack("<%dH" % (TILESET_SIZE * 4), take(TILESET_SIZE * 8))
            layer.tile_types = struct.unpack("<%dH" % TILESET_SIZE, take(TILESET_SIZE * 2))
            layer.map = struct.unpack("<%dH" % MAP_TILES, take(MAP_TILES * 2))
            layer.collision = take(MAP_TILES)
            layer.act_tiles = take(MAP_TILES)
            (layer.bgcnt,) = struct.unpack("<H", take(2))
            self.layers.append(layer)
        self.palette = struct.unpack("<256H", take(0x200))
        self.vram = take(0x10000)
        self.width, self.height = (v // 16 for v in struct.unpack("<2H", take(4)))

    def tile_pixels(self, layer_index, index):
        """16x16 RGB pixels of a tile index, None for transparent pixels."""
        layer = self.layers[layer_index]
        chars = ((layer.bgcnt >> 2) & 3) * 0x4000
        pixels = [None] * 256
        for q in range(4):
            entry = layer.sub_tiles[index * 4 + q]
            tile, pal = entry & 0x3FF, entry >> 12
            for y in range(8):
                for x in range(8):
                    sx = 7 - x if entry & 0x400 else x
                    sy = 7 - y if entry & 0x800 else y
                    byte = self.vram[(chars + tile * 32 + sy * 4 + sx // 2) & 0xFFFF]
                    color = byte >> 4 if sx & 1 else byte & 0xF
                    if color:
                        c = self.palette[pal * 16 + color]
                        pixels[((q >> 1) * 8 + y) * 16 + (q & 1) * 8 + x] = (
                            (c & 31) * 255 // 31,
                            ((c >> 5) & 31) * 255 // 31,
                            ((c >> 10) & 31) * 255 // 31,
                        )
        return pixels

    def backdrop(self):
        c = self.palette[0]
        return ((c & 31) * 255 // 31, ((c >> 5) & 31) * 255 // 31, ((c >> 10) & 31) * 255 // 31)
