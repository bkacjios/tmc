"""Oracle of Ages Lynna City settings for tools/pc/oracle_map.py."""

# Lynna City: present overworld screens x 5-9, y 3-6.
SCREENS_X = range(5, 10)
SCREENS_Y = range(3, 7)

# The map is scaled up by 3/2 so it matches Minish Cap's sizes, then cropped to
# Minish Cap's 64x64 tile maximum (which cuts off most of the eastern forest).
SCALE_NUM, SCALE_DEN = 3, 2
MAX_MAP_SIZE = 64

# Tile animation group of tileset 00 (data/ages/tilesets.s).
ANIMATION_GROUP = 0x00

# Minish Cap room the map replaces (Hyrule Field, South Hyrule Field: 63x43 tiles).
TARGET_AREA = 3
TARGET_ROOM = 1

# Deep water metatiles of tileset 00. They get Minish Cap's deep water act tile,
# so the player swims (with flippers) or drowns there like in Minish Cap.
# Waterfalls (0xFE, 0xFF) keep their solid collision.
WATER_METATILES = set(range(0xE0, 0xE4)) | {0xE9, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD}

# Special collision values (>= 0x10) that are solid; the others (bridges, stairs)
# are walkable.
SOLID_SPECIAL_COLLISIONS = {0x10}  # holes
