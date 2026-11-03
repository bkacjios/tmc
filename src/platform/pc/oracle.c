// Oracle of Ages proof of concept: Lynna City with Oracle's own graphics,
// converted from oracles-disasm (tools/pc/oracle_map.py) and scaled up by 3/2,
// played in place of a Minish Cap room. The scaled tiles replace the room's BG
// tiles in character blocks 0-1 (BG2, the bottom layer, uses both; BG1, the top
// layer, is left empty), BG palettes 1-8 and the bottom layer's tile table.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "area.h"
#include "common.h"
#include "fade.h"
#include "main.h"
#include "map.h"
#include "room.h"
#include "player.h"
#include "transitions.h"
#include "pc.h"

// Generated from oracles-disasm by tools/pc/oracle_map.py. Weak, so the build
// works without oracles-disasm; Oracle mode is then unavailable.
extern const u8 gPcOracleArea __attribute__((weak));
extern const u8 gPcOracleRoom __attribute__((weak));
extern const u16 gPcOracleWidth __attribute__((weak));
extern const u16 gPcOracleHeight __attribute__((weak));
extern const u16 gPcOracleSpawnX __attribute__((weak));
extern const u16 gPcOracleSpawnY __attribute__((weak));
extern const u8 gPcOracleTiles[] __attribute__((weak));
extern const u16 gPcOraclePalettes[] __attribute__((weak));
extern const u16 gPcOracleNumScaledTiles __attribute__((weak));
extern const u32 gPcOracleScaledTiles[] __attribute__((weak));
extern const u16 gPcOracleNumMetatiles __attribute__((weak));
extern const u16 gPcOracleSubTiles[] __attribute__((weak));
extern const u16 gPcOracleMap[] __attribute__((weak));
extern const u8 gPcOracleCollision[] __attribute__((weak));
extern const u8 gPcOracleActTiles[] __attribute__((weak));
extern const u8 gPcOracleAnimTiles[] __attribute__((weak));
extern const u16 gPcOracleAnimHeaders[] __attribute__((weak));
extern const u8 gPcOracleAnims[] __attribute__((weak));
extern const u8 gPcOracleNumAnims __attribute__((weak));

#define ORACLE_TILES_SIZE 0x4000         // 512 4bpp source tiles
#define ORACLE_SCALED_TILES_SIZE 0x8000  // 1024 4bpp tiles, character blocks 0-1
#define ORACLE_BLANK_TILE 512            // block 1's first tile, for the empty top layer
#define ORACLE_FIRST_PALETTE 1
#define ORACLE_PALETTES 8


int gPcOracleMode;

static const Transition sNoExits[] = { { .warp_type = WARP_TYPE_END_OF_LIST } };

// The source tiles as currently animated, and the scaled tiles made from them,
// which are copied to VRAM when they change.
static u8 sTiles[ORACLE_TILES_SIZE];
static u8 sScaledTiles[ORACLE_SCALED_TILES_SIZE];

#define MAX_ANIMS 8
static struct {
    const u8* steps;
    const u8* current;
    u8 timer;
} sAnims[MAX_ANIMS];

int PcOracleAvailable(void) {
    return gPcOracleMap != NULL;
}

// TMC_GAME=ages|minishcap picks the game without the menu (also when headless).
int PcPreselectedGame(void) {
    const char* game = getenv("TMC_GAME");
    if (game == NULL)
        return -1;
    return strcmp(game, "ages") == 0 ? 1 : 0;
}

// Whether the Oracle map replaces this room.
bool32 PcOracleOverride(u32 area, u32 room) {
    return gPcOracleMode && PcOracleAvailable() && area == gPcOracleArea && room == gPcOracleRoom;
}

// Called when the game starts from the file select: spawn in the Oracle map.
// TMC_SPAWN=area,room,x,y spawns anywhere instead (for testing and dumps).
void PcOracleSpawn(PlayerRoomStatus* status) {
    const char* spawn = getenv("TMC_SPAWN");
    unsigned area, room, x, y;
    if (spawn != NULL && sscanf(spawn, "%u,%u,%u,%u", &area, &room, &x, &y) == 4) {
        status->area_next = area;
        status->room_next = room;
        status->start_pos_x = x;
        status->start_pos_y = y;
        status->layer = 1;
        status->spawn_type = PL_SPAWN_DEFAULT;
        return;
    }
    if (!gPcOracleMode || !PcOracleAvailable())
        return;
    status->area_next = gPcOracleArea;
    status->room_next = gPcOracleRoom;
    status->start_pos_x = gPcOracleSpawnX;
    status->start_pos_y = gPcOracleSpawnY;
    status->layer = 1;
    status->spawn_type = PL_SPAWN_DEFAULT;
}

// Replaces the room's size and exits, and drops its entities and scripts
// (from InitRoomResInfo).
void PcOracleRoomInfo(RoomResInfo* info, u32 area, u32 room) {
    if (!PcOracleOverride(area, room))
        return;
    info->pixel_width = gPcOracleWidth * 16;
    info->pixel_height = gPcOracleHeight * 16;
    info->exits = sNoExits;
    info->properties = NULL;
    info->bg_anim = NULL; // tile animations would overwrite the Oracle tiles
}

// Rebuilds the scaled tiles from the source tiles (see tools/pc/oracle_map.py).
static void ScaleTiles(void) {
    u32 i;
    memset(sScaledTiles, 0, sizeof(sScaledTiles));
    for (i = 0; i < gPcOracleNumScaledTiles * 64u; i++) {
        u32 recipe = gPcOracleScaledTiles[i];
        u32 pixel, color;
        if (i / 64 == ORACLE_BLANK_TILE)
            continue;
        pixel = (recipe >> 2) & 0x3F;
        color = (sTiles[(recipe >> 8) * 32 + pixel / 2] >> (4 * (pixel & 1))) & 0xF;
        if (color != 0)
            color += 4 * (recipe & 3);
        sScaledTiles[i / 2] |= color << (4 * (i & 1));
    }
}

static void InitAnimations(void) {
    const u8* p = gPcOracleAnims;
    s32 i;

    memcpy(sTiles, gPcOracleTiles, ORACLE_TILES_SIZE);
    for (i = 0; i < gPcOracleNumAnims && i < MAX_ANIMS; i++) {
        sAnims[i].steps = sAnims[i].current = p;
        sAnims[i].timer = 0;
        while (*p != 0)
            p += 2;
        p++;
    }
    ScaleTiles();
}

// Steps Oracle's tile animations: each step shows a graphics header's tiles,
// then waits its number of frames.
static void UpdateAnimations(void) {
    bool32 changed = FALSE;
    s32 i;
    for (i = 0; i < gPcOracleNumAnims && i < MAX_ANIMS; i++) {
        const u16* header;
        if (sAnims[i].steps == NULL)
            continue; // the tile set is not loaded yet
        if (sAnims[i].timer != 0 && --sAnims[i].timer != 0)
            continue;
        if (sAnims[i].current[0] == 0)
            sAnims[i].current = sAnims[i].steps; // loop
        header = &gPcOracleAnimHeaders[sAnims[i].current[1] * 3];
        memcpy(&sTiles[header[0] * 32], &gPcOracleAnimTiles[header[2] * 32], header[1] * 32);
        sAnims[i].timer = sAnims[i].current[0];
        sAnims[i].current += 2;
        changed = TRUE;
    }
    if (changed)
        ScaleTiles();
}

// Loads the Oracle tile table over the room's (from LoadRoomTileSet). The top
// layer's tiles all become the first, empty tile of its character block 1.
void PcOracleLoadTileSet(void) {
    if (!PcOracleOverride(gRoomControls.area, gRoomControls.room))
        return;
    MemCopy(gPcOracleSubTiles, gMapBottom.subTiles, gPcOracleNumMetatiles * 4 * sizeof(u16));
    MemClear(gMapBottom.tileTypes, gPcOracleNumMetatiles * sizeof(u16));
    MemClear(gMapTop.subTiles, sizeof(gMapTop.subTiles));
    InitAnimations();
}

// The game uploads the room's graphics and palettes over several frames after
// loading it, so the Oracle tiles (with their animations) and palettes are put
// back every frame (from PcFrame, before the frame is drawn).
void PcOracleFrame(void) {
    u16* palettes = &gPaletteBuffer[ORACLE_FIRST_PALETTE * 16];
    s32 i;

    if (!PcOracleOverride(gRoomControls.area, gRoomControls.room))
        return;
    UpdateAnimations();
    if (memcmp((void*)VRAM, sScaledTiles, ORACLE_SCALED_TILES_SIZE) != 0)
        memcpy((void*)VRAM, sScaledTiles, ORACLE_SCALED_TILES_SIZE);
    if (memcmp(palettes, gPcOraclePalettes, ORACLE_PALETTES * 16 * sizeof(u16)) != 0) {
        memcpy(palettes, gPcOraclePalettes, ORACLE_PALETTES * 16 * sizeof(u16));
        for (i = 0; i < ORACLE_PALETTES; i++)
            USE_PALETTE(ORACLE_FIRST_PALETTE + i);
    }
}

// Replaces the map the room just loaded, still in compact form (from LoadRoomGfx).
void PcOracleLoadMap(void) {
    if (!PcOracleOverride(gRoomControls.area, gRoomControls.room))
        return;
    MemCopy(gPcOracleMap, gMapBottom.mapData, gPcOracleWidth * gPcOracleHeight * 2);
    MemClear(gMapTop.mapData, sizeof(gMapTop.mapData));
}

// Collision and act tiles (water) from the Oracle map (from LoadRoomGfx).
void PcOracleApplyCollision(void) {
    u32 x, y;
    if (!PcOracleOverride(gRoomControls.area, gRoomControls.room))
        return;
    for (y = 0; y < gPcOracleHeight; y++) {
        for (x = 0; x < gPcOracleWidth; x++) {
            gMapBottom.collisionData[y * 64 + x] = gPcOracleCollision[y * gPcOracleWidth + x];
            gMapBottom.actTiles[y * 64 + x] = gPcOracleActTiles[y * gPcOracleWidth + x];
        }
    }
    MemClear(gMapTop.collisionData, sizeof(gMapTop.collisionData));
}

static void DumpLayer(FILE* f, MapLayer* layer) {
    fwrite(layer->subTiles, sizeof(layer->subTiles), 1, f);
    fwrite(layer->tileTypes, sizeof(layer->tileTypes), 1, f);
    fwrite(layer->mapData, sizeof(layer->mapData), 1, f);
    fwrite(layer->collisionData, sizeof(layer->collisionData), 1, f);
    fwrite(layer->actTiles, sizeof(layer->actTiles), 1, f);
    fwrite(&layer->bgSettings->control, 2, 1, f);
}

// TMC_DUMP_BG=path writes the current room's map state at exit, for the tools
// in tools/pc: for the bottom then the top layer the subtiles, tile types, map,
// collision, act tiles and BG control; then BG palettes, BG VRAM and the room size.
void PcDumpBgState(void) {
    const char* path = getenv("TMC_DUMP_BG");
    FILE* f;
    if (path == NULL || (f = fopen(path, "wb")) == NULL)
        return;
    DumpLayer(f, &gMapBottom);
    DumpLayer(f, &gMapTop);
    fwrite(PC_PLTT, 0x200, 1, f);
    fwrite(PC_VRAM, 0x10000, 1, f);
    fwrite(&gRoomControls.width, 2, 1, f);
    fwrite(&gRoomControls.height, 2, 1, f);
    fclose(f);
}
