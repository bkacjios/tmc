// C versions of asm/src/enemy.s, asm/src/projectileUpdate.s and asm/src/script.s,
// plus the Thumb routine embedded in data/scripts.s.

#include "global.h"
#include "entity.h"

extern void (*const gEnemyFunctions[])(Entity*);
extern void (*const gProjectileFunctions[])(Entity*);
extern EntityAction* const gUnk_080012C8[];

extern bool32 EnemyInit(Entity*);
extern bool32 ProjectileInit(Entity*);
extern bool32 EntityDisabled(Entity*);
extern void DrawEntity(Entity*);
extern void sub_080028E0(Entity*);
extern void UpdateAnimationVariableFrames(Entity*, u32);
extern void CreatePitFallFx(Entity*);
extern void EnemyDetachFX(Entity*);
extern u32 GravityUpdate(Entity*, u32);
extern u32 GetTileHazardType(Entity*);
extern u32 GetNextFunction(Entity*);
extern u32 GetFacingDirection(Entity*, Entity*);
extern void Knockback1(Entity*);
extern void Knockback2(Entity*);
extern u32 GetTileTypeAtWorldCoords(s32, s32, u32);
extern u32 GetActTileForTileType(u32);
extern u32 GetActTileAtEntity(Entity*);
extern void CreateDeathFx(Entity*);

// ---------------------------------------------------------------------------
// enemy.s

void EnemyUpdate(Entity* this) {
    if (this->action == 0) {
        if (!EnemyInit(this))
            DeleteThisEntity();
    } else if (EntityDisabled(this)) {
        DrawEntity(this);
        return;
    } else {
        sub_080028E0(this);
    }
    if (!(((u8*)this)[0x6D] & 0x10)) {
        gEnemyFunctions[this->id](this);
        this->contactFlags &= 0x7F;
    }
    DrawEntity(this);
}

// Falling into a pit.
void sub_08001214(Entity* this) {
    if (!(this->gustJarState & 1)) {
        this->gustJarState = 1;
        this->timer = (this->frame & 0x40) ? 0x20 : 1;
    }
    if (--this->timer == 0) {
        CreatePitFallFx(this);
        return;
    }
    UpdateAnimationVariableFrames(this, 4);
}

void GenericConfused(Entity* this) {
    static const s8 sShake[] = { 0, 1, 0, -1 };
    u32 t = (u32)this->confusedTime - 1;

    this->confusedTime = t;
    if (t < 0x3C) {
        this->spriteOffsetX = sShake[t & 3];
        if (t == 0) {
            Entity* fx = *(Entity**)((u8*)this + 0x68);
            if (fx != NULL && fx->kind == 6 && fx->id == 0xF && fx->type == 0x1C)
                EnemyDetachFX(this);
        }
    }
    GravityUpdate(this, Q_8_8(24));
}

void sub_08001290(Entity* this, u32 index) {
    if (index != 0)
        gUnk_080012C8[index](this);
}

s32 sub_080012DC(Entity* this) {
    u32 hazard;

    if (this->gustJarState & 4)
        return 0;
    hazard = GetTileHazardType(this);
    if (hazard == 4)
        return 0;
    if (hazard == 0) {
        if (this->gustJarState & 1)
            this->flags |= 0x80;
        return 0;
    }
    if (hazard != 1) {
        this->timer = 1;
        this->gustJarState |= 1;
    }
    return hazard;
}

void EnemyFunctionHandler(Entity* this, EntityAction* const* table) {
    u32 index = sub_080012DC(this);
    if (index != 0)
        table = gUnk_080012C8;
    else
        index = GetNextFunction(this);
    table[index](this);
}

void GenericKnockback(Entity* this) {
    Knockback1(this);
}

void GenericKnockback2(Entity* this) {
    Knockback2(this);
}

void sub_08001318(Entity* this) {
    if (this->z.HALF.HI < 0)
        this->direction = 0xFF;
    Knockback1(this);
}

u32 sub_0800132C(Entity* this, Entity* other) {
    if (!(this->collisionLayer & other->collisionLayer))
        return 0xFF;
    if ((u32)(this->x.HALF_U.HI - other->x.HALF_U.HI + 8) < 0x11 &&
        (u32)(this->y.HALF_U.HI - other->y.HALF_U.HI + 8) < 0x11)
        return 0xFF;
    return GetFacingDirection(this, other);
}

// ---------------------------------------------------------------------------
// projectileUpdate.s

void ProjectileUpdate(Entity* this) {
    if (this->action == 0) {
        if (!ProjectileInit(this))
            DeleteThisEntity();
    } else if (EntityDisabled(this)) {
        DrawEntity(this);
        return;
    } else {
        sub_080028E0(this);
    }
    gProjectileFunctions[this->id](this);
    this->contactFlags &= 0x7F;
    DrawEntity(this);
}

// ---------------------------------------------------------------------------
// script.s

typedef struct {
    u16 tile;
    u8 srcLayer;
    u8 destLayer;
} TransitionTile;

static const TransitionTile sTransitionTiles[] = {
    { 0x2A, 3, 3 }, { 0x2D, 3, 3 }, { 0x2B, 3, 3 }, { 0x2C, 3, 3 }, { 0x4C, 3, 3 }, { 0x4E, 3, 3 },
    { 0x4D, 3, 3 }, { 0x4F, 3, 3 },
    // gTransitionTiles
    { 0x0A, 2, 1 }, { 0x09, 2, 1 }, { 0x0C, 1, 2 }, { 0x0B, 1, 2 }, { 0x52, 3, 3 }, { 0x27, 3, 3 },
    { 0x26, 3, 3 }, { 0 },
};
#define gTransitionTiles (&sTransitionTiles[8])

u32 GetNextScriptCommandHalfword(u16* script) {
    return script[0];
}

u32 GetNextScriptCommandHalfwordAfterCommandMetadata(u16* script) {
    return script[1];
}

u32 GetNextScriptCommandWord(u16* script) {
    return script[0] | (script[1] << 16);
}

u32 GetNextScriptCommandWordAfterCommandMetadata(u16* script) {
    return script[1] | (script[2] << 16);
}

void UpdateSpriteForCollisionLayer(Entity* entity) {
    static const u8 sPriorities[] = { 0x80, 0x80, 0x80, 0x80, 0x40, 0x40, 0x40, 0x40 };
    const u8* p = &sPriorities[entity->collisionLayer * 2];
    *(u8*)&entity->spriteRendering = (*(u8*)&entity->spriteRendering & ~0xC0) | p[0];
    *(u8*)&entity->spriteOrientation = (*(u8*)&entity->spriteOrientation & ~0xC0) | p[1];
}

u32 ResolveCollisionLayer(Entity* entity) {
    if (entity->collisionLayer == 0) {
        u32 layer = 1;
        u32 tileType = GetTileTypeAtWorldCoords(entity->x.HALF_U.HI, entity->y.HALF_U.HI, 2);
        if (tileType != 0) {
            // The search starts at the second entry of the table.
            const TransitionTile* t;
            u32 actTile = GetActTileForTileType(tileType);
            layer = 2;
            for (t = &sTransitionTiles[1]; t->tile != 0; t++) {
                if (t->tile == actTile) {
                    layer = t->destLayer;
                    break;
                }
            }
        }
        entity->collisionLayer = layer;
    }
    UpdateSpriteForCollisionLayer(entity);
    return (u32)entity;
}

u32 CheckOnLayerTransition(Entity* entity) {
    u32 actTile = GetActTileAtEntity(entity);
    const TransitionTile* t;
    for (t = gTransitionTiles; t->tile != 0; t++) {
        if (t->tile == actTile) {
            if (entity->collisionLayer != t->srcLayer)
                entity->collisionLayer = t->destLayer;
            break;
        }
    }
    return actTile;
}

u32 UpdateCollisionLayer(Entity* entity) {
    u32 actTile = CheckOnLayerTransition(entity);
    UpdateSpriteForCollisionLayer(entity);
    return actTile;
}

// ---------------------------------------------------------------------------
// data/scripts.s

void CreateDustFromScript(Entity* entity) {
    CreateDeathFx(entity);
}
