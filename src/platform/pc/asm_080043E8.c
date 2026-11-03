// C version of asm/src/code_080043E8.s: hazards, collision layers, bouncing
// and direction helpers.

#include "global.h"
#include "entity.h"

extern Entity gPlayerEntity;
extern u8 gUnk_02024048;
extern u16 gUnk_02021F20[];
extern const u16 gUnk_080C93E0[];

extern u32 UpdateCollisionLayer(Entity*);
extern u32 CheckOnScreen(Entity*);
extern void SoundReq(u32 sound);
extern u32 GetActTileAtEntity(Entity*);
extern bool32 PlayerCanBeMoved(void);
extern void CalculateEntityTileCollisions(Entity* this, u32 direction, u32 collisionType);
extern bool32 ProcessMovementInternal(Entity*, s32, s32, u32);
extern void sub_08079E58(s32 speed, u32 direction);
extern u32 ram_sub_080B227C(Entity*, Entity*);
extern u32 ram_CalcCollisionDirection(s32 x1, s32 y1, s32 x2, s32 y2);
extern void CreatePitFallFx(Entity*);
extern void CreateDrownFx(Entity*);
extern void CreateLavaDrownFx(Entity*);
extern void CreateSwampDrownFx(Entity*);
extern s32 Div(s32 num, s32 denom);

u32 GetTileHazardType(Entity* entity) {
    static const u16 sHazards[][2] = {
        { 0xD, 1 }, { 0x10, 2 }, { 0x11, 2 }, { 0x5A, 3 }, { 0x13, 4 }, { 0, 0 },
    };
    u32 tile;
    s32 i;

    if (entity->action == 0)
        return 0;
    tile = UpdateCollisionLayer(entity);
    if (entity->z.HALF.HI < 0 || tile == 0)
        return 0;
    for (i = 0; sHazards[i][0] != 0; i++) {
        if (sHazards[i][0] == tile)
            return sHazards[i][1];
    }
    return 0;
}

u32 sub_0800442E(Entity* entity) {
    static void (*const sCreateFx[])(Entity*) = {
        CreatePitFallFx,
        CreateDrownFx,
        CreateLavaDrownFx,
        CreateSwampDrownFx,
    };
    u32 hazard = GetTileHazardType(entity);
    if (hazard == 0)
        return 0;
    sCreateFx[hazard - 1](entity);
    return 1;
}

u32 CalcCollisionStaticEntity(Entity* this, Entity* other) {
    return ram_sub_080B227C(this, other);
}

u32 sub_0800445C(Entity* entity) {
    if (!PlayerCanBeMoved())
        return 0;
    if (!CalcCollisionStaticEntity(entity, &gPlayerEntity))
        return 0;
    if (gPlayerEntity.action == 2)
        gPlayerEntity.subAction = 3;
    return 1;
}

void EnqueueSFX(u32 sfx) {
    if (gUnk_02024048 < 8)
        gUnk_02021F20[gUnk_02024048++] = sfx;
}

void SoundReqClipped(Entity* entity, u32 sfx) {
    if (CheckOnScreen(entity))
        SoundReq(sfx);
}

void sub_080044AE(Entity* entity, u32 speed, u32 direction) {
    if (entity == &gPlayerEntity) {
        sub_08079E58(speed, direction);
    } else {
        CalculateEntityTileCollisions(entity, direction, 2);
        ProcessMovementInternal(entity, speed, direction, 2);
    }
}

// Returns 2 while in the air, 1 when bouncing off the ground and 0 when done.
u32 BounceUpdate(Entity* entity, u32 acceleration) {
    s32 z = entity->z.WORD - entity->zVelocity;
    u32 velocity;

    if (z < 0) {
        entity->z.WORD = z;
        entity->zVelocity -= acceleration;
        return 2;
    }
    entity->z.WORD = 1;
    velocity = (u32)-(entity->zVelocity - acceleration) >> 1;
    velocity += velocity >> 2;
    if ((velocity >> 12) < 0xC) {
        entity->zVelocity = 0;
        return 0;
    }
    entity->zVelocity = velocity;
    return 1;
}

static void SetCollisionLayer(Entity* entity, u32 layer, u32 priority) {
    entity->collisionLayer = layer;
    *(u8*)&entity->spriteOrientation = (*(u8*)&entity->spriteOrientation & ~0xC0) + priority;
    *(u8*)&entity->spriteRendering = (*(u8*)&entity->spriteRendering & ~0xC0) + priority;
}

void sub_08004542(Entity* entity) {
    SetCollisionLayer(entity, 2, 0x40);
}

void ResetCollisionLayer(Entity* entity) {
    SetCollisionLayer(entity, 1, 0x80);
}

void sub_0800451C(Entity* entity) {
    switch (GetActTileAtEntity(entity)) {
        case 0xC:
        case 0xB:
            sub_08004542(entity);
            break;
        case 0xA:
        case 0x9:
            ResetCollisionLayer(entity);
            break;
        case 0x26:
        case 0x27:
            SetCollisionLayer(entity, 3, 0x40);
            break;
    }
}

// Turns the entity's direction one step towards the target direction.
void sub_08004596(Entity* entity, u32 target) {
    u32 dir = entity->direction;
    if (dir < 0x20) {
        u32 diff = target - dir;
        if (diff == 0)
            return;
        target = dir + ((diff & 0x1F) < 0x10 ? 1 : -1);
    }
    entity->direction = target & 0x1F;
}

u32 sub_080045B4(Entity* entity, u32 x, u32 y) {
    return ram_CalcCollisionDirection(entity->x.HALF.HI, entity->y.HALF.HI, x, y);
}

u32 GetFacingDirection(Entity* origin, Entity* target) {
    return ram_CalcCollisionDirection(origin->x.HALF.HI, origin->y.HALF.HI, target->x.HALF.HI, target->y.HALF.HI);
}

u32 CalculateDirectionTo(u32 x1, u32 y1, u32 x2, u32 y2) {
    return ram_CalcCollisionDirection(x1, y1, x2, y2);
}

// Returns a direction (0-0xFF) for a vector, using a table of slopes.
u32 CalculateDirectionFromOffsets(s32 x, s32 y) {
    u32 offset = 0x40;

    if (x != 0) {
        s32 q = Div(y << 8, x);
        u32 slope = q < 0 ? -q : q;
        u32 lo, hi;

        if (slope < 0x106) {
            if (slope < 0x6E) {
                offset = 0;
                lo = 0;
                hi = 0x20;
            } else {
                lo = 0x20;
                hi = 0x40;
            }
        } else if (slope < 0x280) {
            lo = 0x40;
            hi = 0x60;
        } else {
            lo = 0x60;
            hi = 0x7E;
        }
        for (; lo < hi; lo += 2) {
            const u16* entry = (const u16*)((const u8*)gUnk_080C93E0 + lo);
            if (slope >= entry[0] && slope < entry[1]) {
                offset = (lo >> 1) + 1;
                break;
            }
        }
    }
    if (x >= 0)
        return y < 0 ? 0x40 - offset : 0x40 + offset;
    return y < 0 ? 0xC0 + offset : 0xC0 - offset;
}
