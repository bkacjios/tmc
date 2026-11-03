// C version of asm/src/code_08003FC4.s: gravity, animation and on-screen checks.

#include "global.h"
#include "entity.h"
#include "room.h"
#include "vram.h"

extern u8 gUnk_02024048;
extern u16 gUnk_02021F20[];
extern u8* const gMapDataPtrs[];
extern u8* const gCollisionDataPtrs[];
#include "structures.h"

extern u32 sub_0806F58C(Entity*, Entity*);
extern void SoundReq(u32 sound);
extern u16 Sqrt(u32 num);
extern Entity* CreateObject(u32 id, u32 type, u32 type2);

u32 GravityUpdate(Entity* entity, u32 gravity) {
    s32 z = entity->z.WORD - entity->zVelocity;
    if (z >= 0) {
        entity->z.WORD = 0;
        entity->zVelocity = 0;
        return 0;
    }
    entity->z.WORD = z;
    entity->zVelocity -= gravity;
    return z;
}

u32 CheckEntityPickup(Entity* this, Entity* other, u32 width, u32 height) {
    Hitbox* hitbox = other->hitbox;
    s32 offsetX = hitbox->offset_x;
    u32 x, y;

    width += hitbox->width;
    height += hitbox->height;
    if (*(u8*)&other->spriteSettings & 4)
        offsetX = -offsetX;
    x = other->x.HALF_U.HI + offsetX;
    y = other->y.HALF_U.HI + hitbox->offset_y;
    if ((this->collisionLayer & other->collisionLayer) == 3)
        return 0;
    if (width != 0 && width * 2 < (u32)(this->x.HALF_U.HI - x + width))
        return 0;
    if (height != 0 && height * 2 < (u32)(this->y.HALF_U.HI - y + height))
        return 0;
    return 1;
}

u32 sub_08003FDE(Entity* this, Entity* other, u32 width, u32 height) {
    if (!CheckEntityPickup(this, other, width, height))
        return 0;
    return sub_0806F58C(this, other);
}

u32 CheckOnScreen(Entity* entity) {
    u32 x = entity->x.HALF.HI - (u16)gRoomControls.scroll_x + 0x3F;
    u32 y;
    if (x >= 0x16E)
        return 0;
    y = entity->y.HALF.HI - (u16)gRoomControls.scroll_y + entity->z.HALF.HI + 0x3F;
    if (y >= 0x11E)
        return 0;
    return 1;
}

u32 sub_080040A2(Entity* entity) {
    if (*(u8*)&entity->spriteSettings & 2)
        return 1;
    return CheckOnScreen(entity);
}

void DrawEntity(Entity* entity) {
    u32 mode = *(u8*)&entity->spriteSettings & 3;
    u32 count;
    u16* sound;

    if (mode == 0 || (mode != 3 && !sub_080040A2(entity))) {
        gUnk_02024048 = 0;
        return;
    }
    {
        EntityDrawList* list = &gOAMControls.drawLists[(*(u8*)&entity->spriteRendering & 0xC0) >> 6];
        if (list->count < 0x40)
            list->entities[list->count++] = entity;
    }
    count = gUnk_02024048;
    gUnk_02024048 = 0;
    for (sound = gUnk_02021F20; count != 0; count--)
        SoundReq(*sound++);
}

// Looks up a bit in the collision data at world coordinates (x, y).
static u32 GetCollisionBit(Entity* entity, const u16* table, u32 x, u32 y) {
    u32 tx = ((x - gRoomControls.origin_x) & 0x3F0) >> 4;
    u32 ty = ((y - gRoomControls.origin_y) & 0x3F0) << 2;
    u32 data = gCollisionDataPtrs[entity->collisionLayer][tx + ty];

    if (data < 0x10) {
        if (!(y & 8))
            data >>= 2;
        if (!(x & 8))
            data >>= 1;
    } else if (data != 0xFF) {
        u32 sub;
        data = table[data - 0x10];
        sub = x & 0xF;
        if (sub >= 4)
            data >>= 1;
        if (sub >= 8)
            data >>= 1;
        if (sub >= 0xC)
            data >>= 1;
        sub = y & 0xF;
        if (sub >= 4)
            data >>= 4;
        if (sub >= 8)
            data >>= 4;
        if (sub >= 0xC)
            data >>= 4;
    }
    return data & 1;
}

u32 sub_080040D8(Entity* entity, u8* table, s32 x, s32 y) {
    return GetCollisionBit(entity, (const u16*)table, x, y);
}

bool32 sub_080040E2(Entity* entity, u8* table) {
    return GetCollisionBit(entity, (const u16*)table, entity->x.HALF_U.HI, entity->y.HALF_U.HI);
}

void SnapToTile(Entity* entity) {
    entity->x.WORD = (entity->x.WORD & ~0xFFFFF) + 0x80000;
    entity->y.WORD = (entity->y.WORD & ~0xFFFFF) + 0x80000;
}

// Mirrors the direction off the walls in a collision mask.
void sub_0800417E(Entity* entity, u32 collisions) {
    u32 dir = entity->direction;
    if (collisions & 0xEE00)
        dir = 0x20 - dir;
    if (collisions & 0xEE)
        dir = 0x10 - dir;
    entity->direction = dir & 0x1F;
}

u32 sub_0800419C(Entity* this, Entity* other, u32 radiusX, u32 radiusY) {
    if (radiusX != 0 && radiusX * 2 < (u32)(this->x.HALF_U.HI - other->x.HALF_U.HI + radiusX))
        return 0;
    if (radiusY != 0 && radiusY * 2 < (u32)(this->y.HALF_U.HI - other->y.HALF_U.HI + radiusY))
        return 0;
    return 1;
}

bool32 EntityInRectRadius(Entity* this, Entity* other, u32 radiusX, u32 radiusY) {
    if (!(this->collisionLayer & other->collisionLayer & 3))
        return 0;
    return sub_0800419C(this, other, radiusX, radiusY);
}

u32 CalcDistance(s32 x, s32 y) {
    return Sqrt((x * x + y * y) << 8);
}

s32 sub_080041E8(s32 x1, s32 y1, s32 x2, s32 y2) {
    return CalcDistance(x1 - x2, y1 - y2);
}

u32 sub_080041DC(Entity* entity, u32 x, u32 y) {
    return sub_080041E8(entity->x.HALF.HI, entity->y.HALF.HI, x, y);
}

// Returns the tile type next to a map position (in bytes) in the given
// direction, and the new position.
static u32 sub_08004212(Entity* entity, u32 dir, u32* pos) {
    u32 p = *pos;
    u8* const* layer;
    u32 tile;

    if (dir & 3)
        p += (dir & 4) ? -2 : 2;
    if ((dir & 3) != 2)
        p += ((dir + 1) & 4) ? 0x80 : -0x80;
    p &= 0x1FFF;
    *pos = p;
    layer = &gMapDataPtrs[entity->collisionLayer * 2];
    tile = *(u16*)(layer[0] + p);
    if (!(tile & 0x4000))
        tile = *(u16*)(layer[1] + ((tile << 17) >> 16));
    return tile;
}

u32 sub_08004202(Entity* entity, u8* outTileType, u32 pos) {
    *(u32*)outTileType = sub_08004212(entity, entity->animationState, &pos);
    return pos;
}

static void FrameZero(Entity* entity) {
    u8* frame = entity->animPtr;
    entity->lastFrameIndex = entity->frameIndex;
    entity->frameIndex = frame[0];
    entity->frameDuration = frame[1];
    entity->frameSpriteSettings = frame[2];
    entity->frame = frame[3];
    if (frame[3] & 0x80) // loop back
        frame = frame + 4 - frame[4] * 4;
    else
        frame += 4;
    entity->animPtr = frame;
}

void InitializeAnimation(Entity* entity, u32 animIndex) {
    const SpritePtr* sprite = &gSpritePtrs[(u16)entity->spriteIndex];
    entity->animIndex = animIndex;
    if (sprite->animations == NULL) {
        // Some invisible entities have no animations. The GBA reads junk from
        // the BIOS area at address 0 there; give them a blank looping frame.
        static const u8 sNoFrame[] = { 0xFF, 1, 0, 0x80, 1 };
        entity->animPtr = (void*)sNoFrame;
    } else {
        entity->animPtr = ((void**)sprite->animations)[animIndex];
    }
    FrameZero(entity);
}

void UpdateAnimationVariableFrames(Entity* entity, u32 frames) {
    s32 t = entity->frameDuration - frames;
    u8* frame;

    if (t == 0) {
        FrameZero(entity);
        return;
    }
    if (t > 0) {
        entity->frameDuration = t;
        return;
    }
    // Skip frames until the remaining time is used up.
    frame = entity->animPtr;
    for (;;) {
        t += frame[1];
        if (t > 0)
            break;
        if (frame[3] & 0x80)
            frame = frame + 4 - frame[4] * 4;
        else
            frame += 4;
    }
    entity->animPtr = frame;
    FrameZero(entity);
    entity->frameDuration = t;
}

void GetNextFrame(Entity* entity) {
    UpdateAnimationVariableFrames(entity, 1);
}

// Points the entity's GFX slot at the tiles of a new frame. For these slots
// (status 5 and up), GfxSlot's paletteIndex holds the frame's tile count (low
// byte) and palettePointer its tiles.
void sub_080042D0(Entity* entity, u32 frameIndex, u16 spriteIndex) {
    const SpritePtr* sprite;
    const u8* frameInfo;
    GfxSlot* slot;
    const void* tiles;
    const void* oldTiles;
    u32 oldCount;

    if (frameIndex == 0xFF)
        return;
    sprite = &gSpritePtrs[spriteIndex];
    if (sprite->frames == NULL)
        return;
    frameInfo = (const u8*)sprite->frames + frameIndex * 4;
    if (frameInfo[0] == 0)
        return;
    slot = &gGFXSlots.slots[entity->spriteAnimation[0]];
    if (slot->status < 5)
        return;
    oldCount = slot->paletteIndex & 0xFF;
    slot->paletteIndex = (slot->paletteIndex & 0xFF00) | frameInfo[0];
    tiles = (const u8*)sprite->ptr + (*(const u16*)(frameInfo + 2) << 5);
    oldTiles = slot->palettePointer;
    slot->palettePointer = tiles;
    if (oldCount != frameInfo[0] || oldTiles != tiles)
        slot->vramStatus = 3;
}

static void UpdateFrameGfx(Entity* entity) {
    u32 frameIndex = entity->frameIndex;
    u32 last = entity->lastFrameIndex;
    entity->lastFrameIndex = frameIndex;
    if (frameIndex != last)
        sub_080042D0(entity, frameIndex, entity->spriteIndex);
}

void InitAnimationForceUpdate(Entity* entity, u32 animIndex) {
    InitializeAnimation(entity, animIndex);
    entity->lastFrameIndex = 0xFF;
    UpdateFrameGfx(entity);
}

void sub_080042BA(Entity* entity, u32 frames) {
    UpdateAnimationVariableFrames(entity, frames);
    UpdateFrameGfx(entity);
}

void UpdateAnimationSingleFrame(Entity* entity) {
    sub_080042BA(entity, 1);
}

static void CreateFx(Entity* parent, u32 fx) {
    Entity* obj = CreateObject(0xF /* SPECIAL_FX */, fx, 0);
    if (obj != NULL) {
        obj->x.HALF_U.HI = parent->x.HALF_U.HI;
        obj->y.HALF_U.HI = parent->y.HALF_U.HI;
        obj->z.HALF_U.HI = parent->z.HALF_U.HI;
        if (parent->kind == 3) // ENEMY
            obj->type2 = 1;
    }
    DeleteEntity(parent);
}

void CreateDrownFx(Entity* parent) {
    CreateFx(parent, 0xB); // FX_WATER_SPLASH
}

void CreateLavaDrownFx(Entity* parent) {
    CreateFx(parent, 0xC); // FX_LAVA_SPLASH
}

void CreateSwampDrownFx(Entity* parent) {
    CreateFx(parent, 0x52); // FX_GREEN_SPLASH
}

void CreatePitFallFx(Entity* parent) {
    CreateFx(parent, 0); // FX_FALL_DOWN
}
