// C version of asm/src/intr.s (USA): the ARM routines the GBA copies to IWRAM,
// plus their Thumb veneers in asm/src/veneer.s.
//
// The sprite drawing routines pass state between each other in registers. That
// state lives in DrawRegs here, and the "OAM is full" bail-out that resets the
// stack pointer becomes a longjmp.

#include <setjmp.h>
#include <stdint.h>
#include <string.h>
#include "global.h"
#include "entity.h"
#include "enemy.h"
#include "color.h"
#include "room.h"
#include "vram.h"
#include "affine.h"

typedef struct {
    void* last;
    void* first;
    Entity* node;
    u8 flags;
} LinkedList2;

typedef struct {
    void* table;
    void* list_top;
    Entity* current_entity;
    void* restore_sp;
} UpdateContext;

typedef u32 (*CollisionHandler)(Entity* org, Entity* tgt, u32 direction, void* settings);

extern UpdateContext gUpdateContext;
extern LinkedList2* gUnk_02018EA0;
extern u8 gCollisionMtx[];
extern const CollisionHandler gCollisionHandlers[];
extern const u8 gUnk_0800464E[];
extern u16* const gUnk_08000F54[][4];
extern const u16 gUnk_08000360[];
extern const u16 gUnk_080B7A3E[];
extern u8* const gMapDataPtrs[];
extern u8* const gTileTypesPtrs[];
extern u8* const gUnk_08000258[];
extern u8* const gActTilePtrs[];
extern u8* const gCollisionDataPtrs[];
extern const u8 gMapTileTypeToActTile[];
extern const u8 gMapSpecialTileToActTile[];
extern u32 gFrameObjLists[];

// The interrupt handlers and the OAM tables after them (data/const/interrupts.s).
// The GBA copies them to IWRAM at 0x030067E4 and the tables point into that
// copy. The PC reads them in ROM instead: their entries are 32-bit addresses
// there, which IwramTables translates.
extern const u32 gUnk_080B2A70[];
#define IWRAM_TABLES 0x030067E4
#define IWRAM_TABLES_SIZE 0x268
#define IWRAM_SHADOW_FRAMES 0x030068CC // ram_0x80b2b58
#define IWRAM_SHADOW_FRAMES2 0x0300694C // ram_0x80b2bd8
#define IWRAM_OBJ_SIZES 0x0300695C // ram_0x80b2be8

static u8* IwramTables(u32 gbaAddr) {
    if (gbaAddr - IWRAM_TABLES < IWRAM_TABLES_SIZE)
        return (u8*)gUnk_080B2A70 + (gbaAddr - IWRAM_TABLES);
    return (u8*)(uptr)gbaAddr;
}

extern void sub_080044AE(Entity*, u32, u32);

// ---------------------------------------------------------------------------
// Tile lookups

#define ROOM_TILE_POS(roomX, roomY) ((((u32)(roomX) >> 4) & 0x3F) + ((((u32)(roomY) >> 4) & 0x3F) << 6))

// Returns the tile type at a tile position, or the special tile (>= 0x4000).
static u32 TileTypeFromTable(u8* const* table, u32 tilePos, u32 layer) {
    u32 tile = ((u16*)table[layer * 2])[tilePos];
    if (tile >= 0x4000)
        return tile;
    return ((u16*)table[layer * 2 + 1])[tile];
}

u32 ram_sub_080B1A04(u32 tilePos, u32 layer) {
    return TileTypeFromTable(gUnk_08000258, tilePos, layer);
}

u32 ram_sub_080B19FC(u32 tileX, u32 tileY, u32 layer) {
    return ram_sub_080B1A04(tileX + (tileY << 6), layer);
}

u32 ram_sub_080B19EC(u32 roomX, u32 roomY, u32 layer) {
    return ram_sub_080B1A04(ROOM_TILE_POS(roomX, roomY), layer);
}

u32 ram_sub_080B19D8(u32 worldX, u32 worldY, u32 layer) {
    return ram_sub_080B19EC(worldX - gRoomControls.origin_x, worldY - gRoomControls.origin_y, layer);
}

u32 ram_GetTileAtEntityPos(Entity* entity) {
    return ram_sub_080B19D8(entity->x.HALF_U.HI, entity->y.HALF_U.HI, entity->collisionLayer);
}

u32 ram_GetTileType(u32 tilePos, u32 layer) {
    return TileTypeFromTable(gMapDataPtrs, tilePos, layer);
}

u32 ram_GetTileTypeAtRoomTile(u32 tileX, u32 tileY, u32 layer) {
    return ram_GetTileType(tileX + (tileY << 6), layer);
}

u32 ram_GetTileTypeAtRoomCoords(u32 roomX, u32 roomY, u32 layer) {
    return ram_GetTileType(ROOM_TILE_POS(roomX, roomY), layer);
}

u32 ram_GetTileTypeAtWorldCoords(u32 worldX, u32 worldY, u32 layer) {
    return ram_GetTileTypeAtRoomCoords(worldX - gRoomControls.origin_x, worldY - gRoomControls.origin_y, layer);
}

u32 ram_GetTileTypeAtEntity(Entity* entity) {
    return ram_GetTileTypeAtWorldCoords(entity->x.HALF_U.HI, entity->y.HALF_U.HI, entity->collisionLayer);
}

u32 ram_GetTileTypeRelativeToEntity(Entity* entity, s32 xOffset, s32 yOffset) {
    return ram_GetTileTypeAtWorldCoords(entity->x.HALF_U.HI + xOffset, entity->y.HALF_U.HI + yOffset,
                                        entity->collisionLayer);
}

u32 ram_GetActTileAtTilePos(u32 tilePos, u32 layer) {
    return gActTilePtrs[layer][tilePos];
}

u32 ram_GetActTileAtRoomTile(u32 tileX, u32 tileY, u32 layer) {
    return ram_GetActTileAtTilePos(tileX + (tileY << 6), layer);
}

u32 ram_GetActTileAtRoomCoords(u32 roomX, u32 roomY, u32 layer) {
    return ram_GetActTileAtTilePos(ROOM_TILE_POS(roomX, roomY), layer);
}

u32 ram_GetActTileAtWorldCoords(u32 worldX, u32 worldY, u32 layer) {
    return ram_GetActTileAtRoomCoords(worldX - gRoomControls.origin_x, worldY - gRoomControls.origin_y, layer);
}

u32 ram_GetActTileAtEntity(Entity* entity) {
    return ram_GetActTileAtWorldCoords(entity->x.HALF_U.HI, entity->y.HALF_U.HI, entity->collisionLayer);
}

u32 ram_GetActTileRelativeToEntity(Entity* entity, s32 xOffset, s32 yOffset) {
    return ram_GetActTileAtWorldCoords(entity->x.HALF_U.HI + xOffset, entity->y.HALF_U.HI + yOffset,
                                       entity->collisionLayer);
}

u32 ram_GetCollisionDataAtTilePos(u32 tilePos, u32 layer) {
    return gCollisionDataPtrs[layer][tilePos];
}

u32 ram_GetCollisionDataAtRoomTile(u32 tileX, u32 tileY, u32 layer) {
    return ram_GetCollisionDataAtTilePos(tileX + (tileY << 6), layer);
}

u32 ram_GetCollisionDataAtRoomCoords(u32 roomX, u32 roomY, u32 layer) {
    return ram_GetCollisionDataAtTilePos(ROOM_TILE_POS(roomX, roomY), layer);
}

u32 ram_GetCollisionDataAtWorldCoords(u32 worldX, u32 worldY, u32 layer) {
    return ram_GetCollisionDataAtRoomCoords(worldX - gRoomControls.origin_x, worldY - gRoomControls.origin_y, layer);
}

u32 ram_GetCollisionDataAtEntity(Entity* entity) {
    return ram_GetCollisionDataAtWorldCoords(entity->x.HALF_U.HI, entity->y.HALF_U.HI, entity->collisionLayer);
}

u32 ram_GetCollisionDataRelativeTo(Entity* entity, s32 xOffset, s32 yOffset) {
    return ram_GetCollisionDataAtWorldCoords(entity->x.HALF_U.HI + xOffset, entity->y.HALF_U.HI + yOffset,
                                             entity->collisionLayer);
}

u32 ram_GetActTileForTileType(u32 tileType) {
    const u8* table = (tileType & 0x4000) ? gMapSpecialTileToActTile : gMapTileTypeToActTile;
    return table[tileType & 0x3FFF];
}

u32 ram_sub_080B1B68(u32 tileIndex, u32 layer) {
    if (tileIndex >> 14)
        return tileIndex;
    return ((u16*)gTileTypesPtrs[layer * 2])[tileIndex];
}

u32 ram_sub_080B1B84(u32 tilePos, u32 layer) {
    u32 tileType = ram_GetTileType(tilePos, layer);
    const u16* table = (tileType & 0x4000) ? gUnk_080B7A3E : gUnk_08000360;
    return table[tileType & 0x3FFF];
}

u32 ram_sub_080B1BA4(u32 tilePos, u32 layer, u32 mask) {
    return ram_sub_080B1B84(tilePos, layer) & mask;
}

// Act tile under an entity plus an offset.
static u32 sub_080B1BCC(Entity* entity, s32 xOffset, s32 yOffset) {
    u32 x = entity->x.HALF_U.HI + xOffset - gRoomControls.origin_x;
    u32 y = entity->y.HALF_U.HI + yOffset - gRoomControls.origin_y;
    return gActTilePtrs[entity->collisionLayer][ROOM_TILE_POS(x, y)];
}

// Thumb veneers (asm/src/veneer.s).
u32 GetTileAtEntityPos(Entity* e) { return ram_GetTileAtEntityPos(e); }
u32 sub_080B19D8(u32 x, u32 y, u32 l) { return ram_sub_080B19D8(x, y, l); }
u32 sub_080B19EC(u32 x, u32 y, u32 l) { return ram_sub_080B19EC(x, y, l); }
u32 sub_080B19FC(u32 x, u32 y, u32 l) { return ram_sub_080B19FC(x, y, l); }
u32 sub_080B1A04(u32 p, u32 l) { return ram_sub_080B1A04(p, l); }
u32 GetTileTypeRelativeToEntity(Entity* e, s32 x, s32 y) { return ram_GetTileTypeRelativeToEntity(e, x, y); }
u32 GetTileTypeAtEntity(Entity* e) { return ram_GetTileTypeAtEntity(e); }
u32 GetTileTypeAtWorldCoords(s32 x, s32 y, u32 l) { return ram_GetTileTypeAtWorldCoords(x, y, l); }
u32 GetTileTypeAtRoomCoords(u32 x, u32 y, u32 l) { return ram_GetTileTypeAtRoomCoords(x, y, l); }
u32 GetTileTypeAtRoomTile(u32 x, u32 y, u32 l) { return ram_GetTileTypeAtRoomTile(x, y, l); }
u32 GetTileTypeAtTilePos(u32 p, u32 l) { return ram_GetTileType(p, l); }
u32 GetActTileRelativeToEntity(Entity* e, s32 x, s32 y) { return ram_GetActTileRelativeToEntity(e, x, y); }
u32 GetActTileAtEntity(Entity* e) { return ram_GetActTileAtEntity(e); }
u32 GetActTileAtWorldCoords(u32 x, u32 y, u32 l) { return ram_GetActTileAtWorldCoords(x, y, l); }
u32 GetActTileAtRoomCoords(u32 x, u32 y, u32 l) { return ram_GetActTileAtRoomCoords(x, y, l); }
u32 GetActTileAtRoomTile(u32 x, u32 y, u32 l) { return ram_GetActTileAtRoomTile(x, y, l); }
u32 GetActTileAtTilePos(u16 p, u8 l) { return ram_GetActTileAtTilePos(p, l); }
u32 GetCollisionDataRelativeTo(Entity* e, s32 x, s32 y) { return ram_GetCollisionDataRelativeTo(e, x, y); }
u32 GetCollisionDataAtEntity(Entity* e) { return ram_GetCollisionDataAtEntity(e); }
u32 GetCollisionDataAtWorldCoords(u32 x, u32 y, u32 l) { return ram_GetCollisionDataAtWorldCoords(x, y, l); }
u32 GetCollisionDataAtRoomCoords(u32 x, u32 y, u32 l) { return ram_GetCollisionDataAtRoomCoords(x, y, l); }
u32 GetCollisionDataAtRoomTile(u32 x, u32 y, u32 l) { return ram_GetCollisionDataAtRoomTile(x, y, l); }
u32 GetCollisionDataAtTilePos(u32 p, u32 l) { return ram_GetCollisionDataAtTilePos(p, l); }
u32 GetActTileForTileType(u32 t) { return ram_GetActTileForTileType(t); }
u32 sub_080B1B68(u32 t, u32 l) { return ram_sub_080B1B68(t, l); }
u32 sub_080B1B84(u32 p, u32 l) { return ram_sub_080B1B84(p, l); }
u32 sub_080B1BA4(u32 p, u32 l, u32 m) { return ram_sub_080B1BA4(p, l, m); }

// ---------------------------------------------------------------------------
// Collision

void UpdateCollision(Entity* this) {
    if (this->flags & 0x80)
        gCollidableList[gCollidableCount++] = this;
}

u32 ram_CalcCollisionDirection(s32 x1, s32 y1, s32 x2, s32 y2) {
    u32 dir;
    u32 dx, dy, t, step;

    dx = x2 - x1;
    if ((s32)dx >= 0) {
        dir = 0;
    } else {
        dir = 0x20;
        dx = -dx;
    }
    dy = y1 - y2;
    if ((s32)dy < 0) {
        dy = -dy;
        dir += 0x10;
    }
    if (dx >= dy) {
        t = dy;
        dy = dx;
        dx = t;
        dir += 8;
    }
    dx <<= 3;
    step = dy << 1;
    if (dx >= dy) {
        dir++;
        dy += step;
    }
    if (dx >= dy) {
        dir++;
        dy += step;
    }
    if (dx >= dy) {
        dir++;
        dy += step;
    }
    if (dx >= dy)
        dir++;
    return gUnk_0800464E[dir];
}

// BBOX_REFLECT helpers: bounce the collision direction off an entity's direction.
static u32 ReflectDirection(u32 calcDir, u32 dir) {
    u32 t = (calcDir - ((dir - 8) & 0x1F)) & 0x1F;
    if (t < 0x11)
        return calcDir;
    t = (calcDir - dir - 8) & 0x1F;
    return (calcDir - (t << 1)) & 0x1F;
}

static u32 CalcCollision(Entity* this, Entity* other) {
    Hitbox* bbThis = this->hitbox;
    Hitbox* bbOther = other->hitbox;
    s32 dx, dy, dz;
    u32 w, h, d, solid, reflect, result, direction, handler;
    u8* settings;

    dx = this->x.HALF_U.HI - other->x.HALF_U.HI + bbThis->offset_x - bbOther->offset_x;
    w = bbThis->width + bbOther->width;
    if ((u32)(dx + w) > w * 2)
        return 0;
    dy = this->y.HALF_U.HI - other->y.HALF_U.HI + bbThis->offset_y - bbOther->offset_y;
    h = bbThis->height + bbOther->height;
    if ((u32)(dy + h) > h * 2)
        return 0;
    d = ((this->collisionFlags & 0x10) ? ((u8*)bbThis)[8] : 5) + ((other->collisionFlags & 0x10) ? ((u8*)bbOther)[8] : 5);
    dz = this->z.HALF.HI - other->z.HALF.HI;
    if ((u32)(d + dz) > d * 2)
        return 0;

    solid = ((this->collisionFlags & 0x20) >> 3) + ((other->collisionFlags & 0x20) >> 2);
    switch (solid) {
        case 4: // BBOX_SOLID this
            dx = this->x.HALF_U.HI - other->x.HALF_U.HI - bbOther->offset_x;
            dy = this->y.HALF_U.HI - other->y.HALF_U.HI - bbOther->offset_y;
            break;
        case 8: // BBOX_SOLID other
            dx = this->x.HALF_U.HI - other->x.HALF_U.HI + bbThis->offset_x;
            dy = this->y.HALF_U.HI - other->y.HALF_U.HI + bbThis->offset_y;
            break;
        case 12: // BBOX_SOLID both
            dx = this->x.HALF_U.HI - other->x.HALF_U.HI;
            dy = this->y.HALF_U.HI - other->y.HALF_U.HI;
            break;
    }
    direction = ram_CalcCollisionDirection(dx, dy, 0, 0);

    settings = gCollisionMtx + 12 * (other->hitType * 0x22 + this->hurtType);
    handler = settings[0] == 0xFF ? *(u16*)(settings + 2) : 0;
    result = gCollisionHandlers[handler](this, other, direction, settings);
    if (result == 0)
        return 0;
    if (result == 2)
        return 1;

    reflect = ((this->collisionFlags & 0x80) >> 5) + ((other->collisionFlags & 0x80) >> 4);
    switch (reflect) {
        case 4: // BBOX_REFLECT this
            direction = ReflectDirection(direction, this->direction);
            break;
        case 8: // BBOX_REFLECT other
            direction = ReflectDirection(direction, other->direction ^ 0x10);
            break;
        case 12: // BBOX_REFLECT both
            direction = this->direction;
            break;
    }
    other->knockbackDirection = direction;
    this->knockbackDirection = direction ^ 0x10;
    return 1;
}

void ram_CollideAll(void) {
    LinkedList2* start;
    LinkedList2* node;
    u32 count = gCollidableCount;

    if (count == 0)
        return;
    start = node = gUnk_02018EA0;
    do {
        Entity* this = node->node;
        if (this->flags & 0x80) {
            u32 mask = 1 << (this->collisionFlags & 7);
            u32 thisFlags = this->collisionFlags;
            u32 thisInvuln = (u8)this->iframes;
            s32 i;
            for (i = count - 1; i >= 0; i--) {
                Entity* other = gCollidableList[i];
                if (this == other)
                    continue;
                if (!(other->collisionMask & mask))
                    continue;
                if (!(other->flags & 0x80))
                    continue;
                if (!(this->collisionLayer & other->collisionLayer))
                    continue;
                if (!((other->collisionFlags | thisFlags) & 0x40) && ((u8)other->iframes | thisInvuln))
                    continue;
                if (!CalcCollision(this, other))
                    continue;
                thisFlags = this->collisionFlags;
                thisInvuln = (u8)this->iframes | this->confusedTime;
                if (!(other->contactFlags & 0x80)) {
                    other->contactedEntity = this;
                    other->contactFlags = this->hurtType | 0x80;
                }
                if (!(this->contactFlags & 0x80)) {
                    this->contactedEntity = other;
                    this->contactFlags = other->hurtType | 0x80;
                }
            }
        }
        node = (LinkedList2*)node->first;
    } while (node != start);
}

// Pushes two overlapping entities apart. Returns whether they overlapped.
u32 ram_sub_080B227C(Entity* tgt, Entity* org) {
    Hitbox* bbTgt = tgt->hitbox;
    Hitbox* bbOrg = org->hitbox;
    s32 dx, dy;
    u32 w, h, overlapX, overlapY, dirX, dir;

    if (bbTgt == NULL || bbOrg == NULL)
        return 0;

    dx = org->x.HALF_U.HI + bbOrg->offset_x - tgt->x.HALF_U.HI - bbTgt->offset_x;
    w = bbTgt->width + bbOrg->width;
    if (w * 2 < (u32)(dx + w))
        return 0;
    if (dx >= 0) {
        dirX = 8;
    } else {
        dirX = 0x18;
        dx = -dx;
    }
    overlapX = w - dx;

    dy = org->y.HALF_U.HI + bbOrg->offset_y - tgt->y.HALF_U.HI - bbTgt->offset_y;
    h = bbTgt->height + bbOrg->height;
    if (h * 2 < (u32)(dy + h))
        return 0;
    if (dy >= 0) {
        dir = 0x10;
    } else {
        dir = 0;
        dy = -dy;
    }
    overlapY = h - dy;

    if (overlapY >= overlapX) {
        overlapY = overlapX;
        dir = dirX;
    }
    if (overlapY == 0)
        return 0;
    if (overlapY >= 5)
        overlapY = 4;
    sub_080044AE(org, overlapY << 8, dir);
    return 1;
}

// ---------------------------------------------------------------------------
// Interrupts, bit sets, palette fades

void IntrMain(void) {
    u16 ie = REG_IE;
    u16 flags = ie & REG_IF;
    u16 ime = REG_IME;
    u32 index;
    u16 handled;

    if (flags & INTR_FLAG_GAMEPAK) {
        REG_IME = 0;
        *(vu8*)(REG_BASE + 0x84) = 0; // sound off
        for (;;) {
        }
    }
    if ((handled = flags & INTR_FLAG_SERIAL)) {
        index = 1;
    } else if ((handled = flags & INTR_FLAG_VCOUNT)) {
        index = 3;
    } else if ((handled = flags & INTR_FLAG_VBLANK)) {
        index = 4;
    } else if ((handled = flags & INTR_FLAG_TIMER3)) {
        index = 5;
    } else {
        index = 0;
    }
    REG_IF &= ~handled; // writing 1 acknowledges on the GBA
    REG_IE = ie & ~handled & 0x208C;
    ((void (*)(void))(uptr)gUnk_080B2A70[index])();
    REG_IE = ie;
    REG_IME = ime;
}

void ram_IntrMain(void) {
    IntrMain();
}

u32 ram_CheckBits(void* bits, u32 start, u32 count) {
    u8* p = (u8*)bits + (start >> 3);
    u32 mask = 0x01010101u << (start & 7);
    do {
        if (!(*p & mask))
            return 0;
        if (mask & 0x80000000u) {
            mask = (mask << 1) + 1;
            p++;
        } else {
            mask <<= 1;
        }
    } while (--count);
    return 1;
}

void ram_MakeFadeBuff256(u16* src, u16* dest, u32 amount, u32 target) {
    u32 base = amount * target;
    u32 scale = 0x400 - (amount << 2);
    u16* const* tables = gUnk_08000F54[*(u8*)(EWRAM_START + 6)]; // brightness preference
    s32 i;

    for (i = 0; i < 16; i++) {
        u32 c = (u32)*src++ << 1;
        u32 r = (scale * (c & 0x3E) + base) >> 10 & 0x3E;
        u32 g = (scale * ((c >> 5) & 0x3E) + base) >> 10 & 0x3E;
        u32 b = (scale * ((c >> 10) & 0x3E) + base) >> 10 & 0x3E;
        *dest++ = *(u16*)((u8*)tables[0] + r) | *(u16*)((u8*)tables[1] + g) | *(u16*)((u8*)tables[2] + b);
    }
}

// ---------------------------------------------------------------------------
// Entity updates


extern void PlayerUpdate(Entity*);
extern void EnemyUpdate(Entity*);
extern void ProjectileUpdate(Entity*);
extern void ObjectUpdate(Entity*);
extern void NPCUpdate(Entity*);
extern void ItemUpdate(Entity*);
extern void ManagerUpdate(Entity*);
static void DeleteThisEntityThunk(Entity* e) {
    DeleteThisEntity();
}

// Update function by entity kind; gUnk_080026A4 points here.
void (*const ram_sub_080B2248[])(Entity*) = {
    DeleteThisEntityThunk, PlayerUpdate, DeleteThisEntityThunk, EnemyUpdate, ProjectileUpdate,
    DeleteThisEntityThunk, ObjectUpdate, NPCUpdate,           ItemUpdate,  ManagerUpdate,
};

static jmp_buf sUpdateRestart;

// Updates the entities in lists 0-7, or the managers in list 8. (The ARM code
// takes the lists from the table gUnk_080026A4.)
void ram_UpdateEntities(u32 managers) {
    LinkedList* volatile list = managers ? &gEntityLists[8] : &gEntityLists[0];
    LinkedList* const end = managers ? &gEntityLists[9] : &gEntityLists[8];
    Entity* volatile entity;

    gUpdateContext.table = NULL;
    gUpdateContext.restore_sp = &sUpdateRestart;
    for (; list < end; list++) {
        gUpdateContext.list_top = list;
        entity = list->first;
        while (entity != (Entity*)list) {
            gEnemyTarget = NULL;
            gUpdateContext.current_entity = entity;
            // ram_ClearAndUpdateEntities jumps back here after deleting the entity.
            if (setjmp(sUpdateRestart) == 0) {
                ram_sub_080B2248[entity->kind](entity);
                if (gUpdateContext.current_entity == entity)
                    UpdateCollision(entity);
            }
            list = gUpdateContext.list_top;
            entity = gUpdateContext.current_entity->next;
        }
    }
    gUpdateContext.current_entity = NULL;
}

void ram_ClearAndUpdateEntities(void) {
    longjmp(sUpdateRestart, 1);
}

// ---------------------------------------------------------------------------
// Sprite drawing

typedef struct {
    s32 x;       // r6: screen x
    s32 y;       // r7: screen y
    u32 attr01;  // r8: OAM attributes 0 and 1
    u32 attr2;   // sb: OAM attribute 2, object count and OAM index while drawing
    u8* frame;   // sl: frame object list
} DrawRegs;

#define OAMC ((u8*)&gOAMControls)
#define OAMC_UPDATED OAMC[3]
#define OAMC_CAMERA_X (*(s16*)(OAMC + 4))
#define OAMC_CAMERA_Y (*(s16*)(OAMC + 6))
#define OAMC_FLASH_PALETTE OAMC[0xE]
#define OAMC_SHADOW_OFFSET OAMC[0x12]

static jmp_buf sOamFull;

static void DrawFrameObjects(DrawRegs* r) {
    u8* frame = r->frame;
    u32 n = *frame++;
    u32 sb, shape;
    const u8* sizes;
    u32* oam;

    if (n == 0)
        return;
    sb = (n << 24) | (r->attr2 & 0xFFFF);
    if (r->attr01 & 0x300) {
        shape = (r->attr01 & 0x300) == 0x100 ? 0 : 0xC0;
    } else {
        shape = ((r->attr01 & 0x30000000) >> 24) * 3;
    }
    sizes = IwramTables(IWRAM_OBJ_SIZES) + shape;
    sb |= OAMC_UPDATED << 16;
    oam = (u32*)(OAMC + 0x20 + OAMC_UPDATED * 8);

    for (;;) {
        s32 ox, oy;
        u32 flags;
        const u8* size;

        sb -= 0x1000000;
        ox = (s8)frame[0];
        oy = (s8)frame[1];
        flags = frame[2];
        if (!(r->attr01 & 0x300)) {
            if (r->attr01 & 0x20000000)
                oy = -oy;
            if (r->attr01 & 0x10000000)
                ox = -ox;
        }
        size = sizes + ((flags & 0xF0) >> 2);
        oy += r->y - size[1];
        ox += r->x - size[0];
        if (oy < 0xA0 && oy + size[3] > 0 && ox < 0xF0 && ox + size[2] > 0) {
            u32 a = (oy & 0xFF) | (((u32)ox << 23) >> 7) | r->attr01 | ((flags & 0xC0) << 8);
            a ^= (flags & 0x3C) << 26;
            *oam++ = a;
            a = frame[3] + sb;
            if (flags & 1)
                a &= ~0xF000;
            a += frame[4] << 8;
            *(u16*)oam = a;
            oam++;
            sb += 0x10000;
        }
        frame += 5;
        if (sb & 0x800000) { // OAM full
            OAMC_UPDATED = 0x80;
            longjmp(sOamFull, 1);
        }
        if ((sb >> 24) == 0)
            break;
    }
    OAMC_UPDATED = sb >> 16;
    r->frame = frame;
    r->attr2 = sb;
}

// Loads an entity's sprite state into the drawing registers.
static void LoadEntityDrawRegs(DrawRegs* r, Entity* e) {
    u32 settings = *(u32*)&e->spriteSettings;
    u32 flash = e->iframes > 0 ? OAMC_FLASH_PALETTE : 0;

    r->attr2 = e->spriteVramOffset;
    r->attr2 |= ((e->palette.raw | flash) & 0xF) << 12;
    r->attr2 |= (*(u8*)&e->spriteOrientation & 0xC0) << 4;
    r->x = e->x.HALF.HI + (s8)e->spriteOffsetX;
    r->y = e->y.HALF.HI + e->z.HALF.HI + e->spriteOffsetY;
    if ((settings & 3) != 2) {
        r->x -= OAMC_CAMERA_X;
        r->y -= OAMC_CAMERA_Y;
    }
    r->attr01 = (settings & 0x3E003F00) | (OAMC[2] << 12) | (((e->frameSpriteSettings ^ settings) & 0xC0) << 22);
}

static void DrawFrame(DrawRegs* r, u32 frameIndex, u32 spriteIndex) {
    u8* frames = (u8*)gFrameObjLists + gFrameObjLists[spriteIndex];
    r->frame = (u8*)gFrameObjLists + ((u32*)frames)[frameIndex];
    DrawFrameObjects(r);
}

static void DrawEntityFrame(DrawRegs* r, Entity* e) {
    if (e->frameIndex != 0xFF)
        DrawFrame(r, e->frameIndex, (u16)e->spriteIndex);
}

static void DrawEntitySprite(DrawRegs* r, Entity* e) {
    s8 parts = e->spriteAnimation[2];
    struct_gUnk_020000C0_1* entry;
    u8* part;
    s32 i;

    if (parts == 0) {
        DrawEntityFrame(r, e);
        return;
    }
    if (parts < 0) {
        r->frame = e->myHeap;
        DrawFrameObjects(r);
        return;
    }

    // Composite sprite made of up to 4 parts.
    entry = gUnk_020000C0[parts].unk_00;
    r->attr2 &= ~0xF000;
    for (i = 0; i < 4; i++, entry++) {
        part = (u8*)entry;
        if (!(part[0] & 1))
            return;
        if (part[0] & 2) {
            Entity* child = entry->unk_0C;
            if (child != NULL) {
                LoadEntityDrawRegs(r, child);
                r->y += OAMC_SHADOW_OFFSET;
                DrawEntityFrame(r, child);
            }
        } else if (part[1] != 0xFF) {
            DrawRegs saved = *r;
            r->attr01 ^= part[4] << 28;
            if (r->attr01 & 0x10000000)
                r->x -= (s8)part[6];
            else
                r->x += (s8)part[6];
            r->y += (s8)part[7];
            r->attr2 |= part[5] << 12;
            r->attr2 += part[8];
            DrawFrame(r, part[1], *(u16*)(part + 2));
            saved.frame = r->frame;
            *r = saved;
        }
    }
}

static void DrawEntity_(DrawRegs* r, Entity* e) {
    s32 z;
    ShadowDrawList* shadows;

    LoadEntityDrawRegs(r, e);
    OAMC_SHADOW_OFFSET = 0;
    if (!(*(u8*)&e->spritePriority & 8)) {
        DrawEntitySprite(r, e);
        return;
    }

    if (e->z.HALF.HI >= 0) {
        u32 tile = sub_080B1BCC(e, 0, 0);
        u32 shadowSize = *(u8*)&e->spriteSettings & 0x30;
        u32 kind;
        if (tile == 0x19 || tile == 0xF || tile == 0x2F) {
            if (tile == 0x2F) {
                kind = (e->x.HALF_U.HI ^ e->y.HALF_U.HI) & 6;
            } else {
                OAMC_SHADOW_OFFSET = 2;
                r->y += 2;
                kind = ((OAMC[1] & 0x18) + 0x80) >> 2;
            }
            if (tile != 0x19) {
                // Water ripple or grass drawn under the entity.
                DrawRegs saved = *r;
                r->frame = IwramTables(*(u32*)(IwramTables(IWRAM_SHADOW_FRAMES) + shadowSize + (kind << 1)));
                r->attr01 = 0;
                r->attr2 &= 0xC00;
                DrawFrameObjects(r);
                saved.frame = r->frame;
                *r = saved;
                DrawEntitySprite(r, e);
                return;
            }
        }
    }

    DrawEntitySprite(r, e);

    z = e->z.HALF.HI;
    if ((*(u8*)&e->spritePriority & 0x10) && z >= 0)
        return;
    if ((*(u8*)&e->spritePriority & 0x20) && (OAMC[1] & 1))
        return;

    // Queue a shadow; they are drawn after all entities of the layer.
    shadows = &gOAMControls.shadows;
    if (shadows->count >= 0x40)
        return;
    shadows->entries[shadows->count].yAndPriority = ((r->attr2 >> 10) & 3) | ((r->y - (z < 0 ? z : 0)) << 6);
    shadows->entries[shadows->count].xAndShape = ((*(u8*)&e->spriteSettings & 0x30) >> 4) | (r->x << 6);
    shadows->count++;
}

// Sorts the entities of a draw list by priority and y position (shell sort).
static void ResolveOamDrawPriority(EntityDrawList* list) {
    Entity** first = list->entities;
    u32 gap = list->count - 1;
    Entity** last = first + gap;

#define DRAW_PRIORITY(e) ((((u32)(e)->y.WORD + 0x80000000u) >> 3) | ((u32)~*(u8*)&(e)->spritePriority << 29))
    while (gap != 0) {
        u32 j;
        for (j = 0; j < gap; j++) {
            Entity** a = last - gap - j;
            while (a >= first) {
                Entity* ent = *a;
                u32 prio = DRAW_PRIORITY(ent);
                Entity** b = a + gap;
                while (b <= last && prio < DRAW_PRIORITY(*b)) {
                    *(b - gap) = *b;
                    b += gap;
                }
                *(b - gap) = ent;
                a -= gap;
            }
        }
        gap >>= 1;
    }
#undef DRAW_PRIORITY
}

static void DrawShadows(DrawRegs* r) {
    ShadowDrawList* shadows = &gOAMControls.shadows;
    s32 i;

    for (i = 0; i < shadows->count; i++) {
        s16 x = shadows->entries[i].xAndShape;
        s16 y = shadows->entries[i].yAndPriority;
        r->x = x >> 6;
        r->frame = IwramTables(((u32*)IwramTables(IWRAM_SHADOW_FRAMES2))[x & 7]);
        r->y = y >> 6;
        r->attr2 = (y & 3) << 10;
        r->attr01 = 0;
        DrawFrameObjects(r);
    }
}

static void DrawList(DrawRegs* r, EntityDrawList* list) {
    s32 i;

    if (list->count == 0)
        return;
    gOAMControls.shadows.count = 0;
    ResolveOamDrawPriority(list);
    for (i = 0; i < list->count; i++)
        DrawEntity_(r, list->entities[i]);
    DrawShadows(r);
}

void ram_DrawEntities(void) {
    DrawRegs r;
    s32 i;

    if (OAMC_UPDATED >= 0x80)
        return;
    if (setjmp(sOamFull))
        return;
    for (i = 0; i < 4; i++)
        DrawList(&r, &gOAMControls.drawLists[i]);
}

void ram_sub_080ADA04(OAMCommand* cmd, u8* frame) {
    DrawRegs r;

    if (frame[0] == 0)
        return;
    if (OAMC_UPDATED >= 0x80)
        return;
    r.frame = frame;
    r.x = cmd->x;
    r.y = cmd->y;
    r.attr01 = *(u32*)&cmd->_4;
    r.attr2 = cmd->_8;
    if (setjmp(sOamFull))
        return;
    DrawFrameObjects(&r);
}

void ram_DrawDirect(OAMCommand* cmd, u32 spriteIndex, u32 frameIndex) {
    u8* frames;

    if (frameIndex == 0xFF)
        return;
    frames = (u8*)gFrameObjLists + gFrameObjLists[spriteIndex];
    ram_sub_080ADA04(cmd, (u8*)gFrameObjLists + ((u32*)frames)[frameIndex]);
}

// ---------------------------------------------------------------------------
// Scrolling: copies the visible map rows to a BG buffer (gUpdateVisibleTiles == 1).
// On the GBA each row is an immediate 16-bit DMA of 0x20 halfwords.

void ram_sub_080B197C(u8* src, u8* dest) {
    u32 x = (u16)gRoomControls.scroll_x - gRoomControls.origin_x;
    u32 y = (u16)gRoomControls.scroll_y - gRoomControls.origin_y;
    s32 row;

    src += ((x >> 4) + ((y >> 4) << 7)) << 2;
    dest -= 0x40;
    // The first row repeats the second unless the view is at least 8 pixels into the room.
    if (y >= 8)
        src -= 0x100;
    memcpy(dest, src, 0x40);
    if (y >= 8)
        src += 0x100;
    for (row = 1; row < 0x17; row++) {
        dest += 0x40;
        memcpy(dest, src, 0x40);
        src += 0x100;
    }
}
