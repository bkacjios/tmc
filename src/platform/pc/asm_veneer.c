// C version of the Thumb code in asm/src/veneer.s. The ARM veneers themselves
// are in asm_intr.c.

#include "global.h"
#include "map.h"

extern u8 gUpdateVisibleTiles;
extern u16 gMapDataTopSpecial[];
extern u16 gMapDataBottomSpecial[];
extern u16 gBG1Buffer[];
extern u16 gBG2Buffer[];
extern MapLayer gMapBottom;
extern MapLayer gMapTop;
extern u8* const gMapDataPtrs[];
extern u8* const gActTilePtrs[];
extern u8* const gCollisionDataPtrs[];
extern const u8 gMapTileTypeToActTile[];
extern const u8 gMapSpecialTileToActTile[];
extern const u8 gMapTileTypeToCollisionData[];
extern const u8 gMapSpecialTileToCollisionData[];

extern void ram_sub_080B197C(u8*, u8*);
extern void sub_0807D280(u8*, u8*);
extern void sub_0807D46C(u8*, u8*);
extern void sub_0807D6D8(u8*, u8*);
extern void RegisterInteractTile(u32 tileIndex, u32 tilePos, u32 layer);
extern void UnregisterInteractTile(u32 tilePos, s32 layer);

// Copies gMapData*Special to the BG buffers depending on gUpdateVisibleTiles.
void UpdateScrollVram(void) {
    static void (*const sUpdaters[])(u8*, u8*) = {
        NULL, ram_sub_080B197C, sub_0807D280, sub_0807D46C, sub_0807D6D8,
    };
    void (*update)(u8*, u8*);

    if (gUpdateVisibleTiles == 0)
        return;
    update = sUpdaters[gUpdateVisibleTiles];
    if (gMapBottom.bgSettings != NULL)
        update((u8*)gMapDataBottomSpecial, (u8*)gBG1Buffer + 0x40);
    if (gMapTop.bgSettings != NULL)
        update((u8*)gMapDataTopSpecial, (u8*)gBG2Buffer + 0x40);
}

void SetCollisionData(u32 collisionData, u32 tilePos, u32 layer) {
    gCollisionDataPtrs[layer][tilePos] = collisionData;
}

void SetActTileAtTilePos(u32 actTile, u32 tilePos, u32 layer) {
    gActTilePtrs[layer][tilePos] = actTile;
}

u32 GetTileIndex(u32 tilePos, u32 layer) {
    return ((u16*)gMapDataPtrs[layer * 2])[tilePos];
}

void SetTile(u32 tileIndex, u32 tilePos, u32 layer) {
    u16* mapData = (u16*)gMapDataPtrs[layer * 2];
    u32 oldTile = mapData[tilePos];

    mapData[tilePos] = tileIndex;
    if (tileIndex >= 0x4000) {
        u32 special = tileIndex - 0x4000;
        SetActTileAtTilePos(gMapSpecialTileToActTile[special], tilePos, layer);
        SetCollisionData(gMapSpecialTileToCollisionData[special], tilePos, layer);
        UnregisterInteractTile(tilePos, layer);
        RegisterInteractTile(oldTile, tilePos, layer);
    } else {
        u32 tileType = ((u16*)gMapDataPtrs[layer * 2 + 1])[tileIndex];
        SetActTileAtTilePos(gMapTileTypeToActTile[tileType], tilePos, layer);
        SetCollisionData(gMapTileTypeToCollisionData[tileType], tilePos, layer);
        UnregisterInteractTile(tilePos, layer);
    }
}

void CloneTile(u32 index, u32 tilePos, u32 layer) {
    static u16* const sTileIndices[] = {
        gMapBottom.tileIndices,
        gMapBottom.tileIndices,
        gMapTop.tileIndices,
        gMapBottom.tileIndices,
    };
    SetTile(sTileIndices[layer][index], tilePos, layer);
}
