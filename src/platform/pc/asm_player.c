// C version of asm/src/player.s: player tile collisions, tile interactions and
// ice/jump velocities.

#include "global.h"
#include "entity.h"
#include "room.h"
#include "asm.h"
#include "map.h"

// Byte offsets into gPlayerState used by these routines.
#define PS(offset) (((u8*)&gPlayerState)[offset])
#define PS_JUMP_STATUS PS(2)
#define PS_HELD_OBJECT PS(5)
#define PS_DIRECTION PS(0xD)
#define PS_SWIM_STATE PS(0x26)
#define PS_FLAGS (*(u32*)&PS(0x30))
#define PS_VEL_X (*(s16*)&PS(0x8C))
#define PS_VEL_Y (*(s16*)&PS(0x8E))

extern u8 gPlayerState[];
extern Entity gPlayerEntity;
extern u8* const gCollisionDataPtrs[];
extern const u16* const gUnk_0800823C[];
extern const u8 gUnk_080082DC[], gUnk_0800833C[], gUnk_0800839C[], gUnk_080083FC[], gUnk_0800845C[],
    gUnk_080084BC[], gUnk_0800851C[];
extern const s8 gUnk_08007DF4[];
extern const KeyValuePair gUnk_080046A4[];
extern const u8 gUnk_080047F6[];
extern const KeyValuePair gMapActTileToSurfaceType[];
extern const s16 gSineTable[];

extern const KeyValuePair* FindEntryForKey(u32 key, const KeyValuePair* list);
extern void LinearMoveDirectionOLD(Entity*, u32, u32);
extern u32 GetTileTypeAtWorldCoords(s32 worldX, s32 worldY, u32 layer);
extern u32 GetActTileRelativeToEntity(Entity* entity, s32 xOffset, s32 yOffset);
extern Entity* CreateObject(u32 id, u32 type, u32 type2);
extern void UpdateSpriteForCollisionLayer(Entity* entity);
extern MapLayer* GetLayerByIndex(u32 layer);
extern void sub_0807B7D8(u32 tileType, u32 tilePos, u32 layer);
extern void RestorePrevTileEntity(u32 tilePos, u32 layer);
extern void ResetPlayerVelocity(void);
extern void sub_0807A5B8(u32);
extern void RespawnPlayer(void);

// Returns the collision bit at room pixel coordinates (x, y) for the player's
// tile collision table.
static u32 PlayerCollisionBit(Entity* entity, const u8* table, u32 x, u32 y) {
    u32 pos = ((x & 0x3F0) >> 4) + ((y & 0x3F0) << 2);
    u32 data = gCollisionDataPtrs[entity->collisionLayer][pos];

    if (PS_SWIM_STATE != 0 && PS(0x12) != 0x18 && data < 0x10)
        data = 0xF;
    if (data < 0x10) {
        if (!(y & 8))
            data >>= 2;
        if (!(x & 8))
            data >>= 1;
    } else if (data != 0xFF) {
        const u16* rows = gUnk_0800823C[table[data - 0x10]];
        data = rows[y & 0xF] >> (0xF ^ (x & 0xF));
    }
    return data & 1;
}

u32 sub_080086B4(u32 x, u32 y, const u8* table) {
    return PlayerCollisionBit(&gPlayerEntity, table, x, y);
}

// Updates entity->collisions from the tiles around the player's hitbox.
static void sub_080085CC(Entity* entity) {
    const u8* table;
    Hitbox* hitbox = entity->hitbox;
    u32 x, y, cx, cy, collisions = 0;
    s32 i;

    if (PS_SWIM_STATE != 0) {
        table = (PS_FLAGS & 0x80) ? gUnk_0800839C : gUnk_080083FC;
    } else if (PS_JUMP_STATUS != 0 || (PS_FLAGS & 0x01000000)) {
        table = gUnk_0800845C;
    } else if (PS_FLAGS & 0x80) {
        table = gUnk_0800833C;
    } else if (PS(0x1C) != 0 || PS_HELD_OBJECT != 0) {
        table = gUnk_080084BC;
    } else if (PS(0xAA) != 0) {
        table = gUnk_0800851C;
    } else {
        table = gUnk_080082DC;
    }

    cx = entity->x.HALF_U.HI - gRoomControls.origin_x + hitbox->offset_x;
    cy = entity->y.HALF_U.HI - gRoomControls.origin_y + hitbox->offset_y;

    // Right then left side, each probed above and below.
    x = cx + hitbox->unk2[0];
    for (i = 0; i < 2; i++) {
        collisions <<= 2;
        collisions |= PlayerCollisionBit(entity, table, x, cy + hitbox->unk2[1]);
        collisions <<= 1;
        collisions |= PlayerCollisionBit(entity, table, x, cy - hitbox->unk2[1]);
        collisions <<= 1;
        x -= hitbox->unk2[0] * 2;
    }
    // Bottom then top side, each probed right and left.
    y = cy + hitbox->unk2[3];
    for (i = 0; i < 2; i++) {
        collisions <<= 2;
        collisions |= PlayerCollisionBit(entity, table, cx + hitbox->unk2[2], y);
        collisions <<= 1;
        collisions |= PlayerCollisionBit(entity, table, cx - hitbox->unk2[2], y);
        collisions <<= 1;
        y -= hitbox->unk2[3] * 2;
    }
    entity->collisions = collisions;
}

void sub_080085B0(Entity* entity) {
    sub_080085CC(entity);
}

void sub_0800857C(Entity* entity) {
    if (!(entity->type2 & 0x80) && !(PS_JUMP_STATUS & 0x80))
        sub_080085CC(entity);
    LinearMoveDirectionOLD(entity, (u16)entity->speed, entity->direction);
}

// r0: Entity*, r1: interaction filter, r2: x, r3: y.
// Trampling tiles, digging, picking up tiles...
u16* DoTileInteraction(Entity* entity, u32 filter, u32 x, u32 y) {
    const KeyValuePair* entry;
    const u8* behavior;
    u32 id, type, tilePos, layer, newTile;

    if (gRoomControls.reload_flags == 1)
        return NULL;
    entry = FindEntryForKey(GetTileTypeAtWorldCoords(x, y, entity->collisionLayer), gUnk_080046A4);
    if (entry == NULL)
        return NULL;
    behavior = gUnk_080047F6 + entry->value * 8;
    if (!((*(u16*)behavior >> filter) & 1))
        return NULL;

    id = behavior[2];
    type = behavior[3];
    if (id != 0xFF && filter != 6 && filter != 0xE && filter != 0xA && filter != 0xB &&
        (filter != 0xD || (id == 0xF && type == 0x17))) {
        Entity* obj = CreateObject(id, type, id == 0xF ? 0x80 : 0); // SPECIAL_FX
        if (obj != NULL) {
            if (behavior[2] != 0) {
                obj->x.HALF_U.HI = (x & ~0xF) + 8;
                obj->y.HALF_U.HI = (y & ~0xF) + 8;
            } else {
                obj->x.HALF_U.HI = entity->x.HALF_U.HI;
                obj->y.HALF_U.HI = entity->y.HALF_U.HI;
                // The original also stores z and the parent through the wrong
                // register, into the read-only BIOS area. Those writes are dropped.
            }
            obj->collisionLayer = entity->collisionLayer;
            UpdateSpriteForCollisionLayer(obj);
        }
    }

    tilePos = ((x - gRoomControls.origin_x) >> 4) + (((y - gRoomControls.origin_y) >> 4) << 6);
    layer = entity->collisionLayer;
    newTile = *(u16*)(behavior + 6);
    if (!(newTile & 0x4000)) {
        sub_0807B7D8(newTile, tilePos, layer);
    } else if (newTile == 0xFFFF) {
        RestorePrevTileEntity(tilePos, layer);
    } else {
        ((u16*)((u8*)GetLayerByIndex(layer) + 4))[tilePos] = newTile;
    }
    return (u16*)behavior;
}

Entity* DoTileInteractionOffset(Entity* entity, u32 filter, s32 xOffset, s32 yOffset) {
    return (Entity*)DoTileInteraction(entity, filter, entity->x.HALF_U.HI + xOffset, entity->y.HALF_U.HI + yOffset);
}

u32* DoTileInteractionHere(Entity* entity, u32 filter) {
    return (u32*)DoTileInteraction(entity, filter, entity->x.HALF_U.HI, entity->y.HALF_U.HI);
}

s32 DoItemTileInteraction(Entity* item, u32 filter, u8* behavior) {
    const s8* offset = &gUnk_08007DF4[item->animationState & 6];
    u8* result = (u8*)DoTileInteractionOffset(item, filter, offset[0], offset[1]);
    if (result != NULL) {
        behavior[3] = result[2];
        behavior[7] = result[3];
        behavior[8] = result[5];
    }
    return (s32)result;
}

// ---------------------------------------------------------------------------
// Velocities

static const s8 sVelocities1[] = { 0, -3, 3, -3, 3, 0, 3, 3, 0, 3, -3, 3, -3, 0, -3, -3 };
static const s8 sIceVelocities[] = { 0, -10, 10, -10, 10, 0, 10, 10, 0, 10, -10, 10, -10, 0, -10, -10 };
static const s8 sVelocities3[] = { 0, 6, -6, 0, 0, -6, 6, 0 };

static void ClampPlayerVelocity(s16* velocity, s32 v) {
    if (v >= 0) {
        if (v >= 0x180)
            v = 0x180;
    } else if ((u32)v < (u32)-0x180) {
        v = -0x180;
    }
    *velocity = v;
}

static void AddPlayerVelocity(s32 dx, s32 dy) {
    ClampPlayerVelocity(&PS_VEL_X, PS_VEL_X + dx);
    ClampPlayerVelocity(&PS_VEL_Y, PS_VEL_Y + dy);
}

// Decays a velocity component towards 0 by 3.
static void sub_08008A1A(s16* velocity) {
    s32 v = *velocity;
    if (v >= 0) {
        v -= 3;
        if (v < 0)
            v = 0;
    } else {
        v += 3;
        if (v > 0)
            v = 0;
    }
    *velocity = v;
}

static void ApplyPlayerVelocity(Entity* entity) {
    s32 v = PS_VEL_X;
    if (v != 0) {
        u32 dir = 8;
        if (v < 0) {
            dir = 0x18;
            v = -v;
        }
        LinearMoveDirectionOLD(entity, v, dir);
        sub_0807A5B8(dir);
    }
    v = PS_VEL_Y;
    if (v != 0) {
        u32 dir = 0x10;
        if (v < 0) {
            dir = 0;
            v = -v;
        }
        LinearMoveDirectionOLD(entity, v, dir);
        sub_0807A5B8(dir);
    }
    if (PS_JUMP_STATUS == 0) {
        sub_08008A1A(&PS_VEL_X);
        sub_08008A1A(&PS_VEL_Y);
    }
}

static void AccelerateAndMove(Entity* entity, u32 dir) {
    const s8* velocities = sIceVelocities;
    u32 index;

    if (PS_HELD_OBJECT == 2 || PS_HELD_OBJECT == 1) {
        ResetPlayerVelocity();
        return;
    }
    if (PS_JUMP_STATUS != 0) {
        u32 rel = (((dir >> 2) - ((entity->animationState >> 1) << 1)) + 2) & 7;
        velocities = sVelocities1;
        if (rel > 4) {
            index = (entity->animationState >> 1) << 1;
            AddPlayerVelocity(sVelocities3[index], sVelocities3[index + 1]);
            ApplyPlayerVelocity(entity);
            return;
        }
    }
    index = (dir >> 2) << 1;
    AddPlayerVelocity(velocities[index], velocities[index + 1]);
    ApplyPlayerVelocity(entity);
}

void sub_08008926(Entity* entity) {
    u32 dir;
    if (PS(7) | PS(0xA))
        return;
    dir = PS_DIRECTION;
    entity->direction = dir;
    if (dir & 0x80) {
        ApplyPlayerVelocity(entity);
        return;
    }
    AccelerateAndMove(entity, dir);
}

void UpdateIcePlayerVelocity(Entity* entity) {
    AccelerateAndMove(entity, (entity->animationState >> 1) << 3);
}

void sub_08008AA0(Entity* entity) {
    u32 dir;
    if (PS(0x12) == 1)
        return;
    dir = PS_DIRECTION;
    if (dir == 0xFF)
        return;
    PS_VEL_X = gSineTable[dir * 8];
    PS_VEL_Y = -gSineTable[dir * 8 + 0x40];
}

// Respawns the player if every side is blocked.
void sub_08008AC6(Entity* entity) {
    u32 collisions;
    s32 i;

    if (PS_SWIM_STATE & 0xF)
        return;
    if (PS_FLAGS & 0x02000020)
        return;
    collisions = entity->collisions;
    for (i = 0; i < 4; i++, collisions >>= 4) {
        if (!(collisions & 0xE))
            return;
    }
    entity->iframes = -30;
    RespawnPlayer();
}

u32 PlayerCheckNEastTile(void) {
    u32 actTile = GetActTileRelativeToEntity(&gPlayerEntity, 0, 0);
    const KeyValuePair* entry;
    if (actTile & 0x4000)
        return 0;
    entry = FindEntryForKey(actTile, gMapActTileToSurfaceType);
    return entry != NULL && entry->value == 1;
}
