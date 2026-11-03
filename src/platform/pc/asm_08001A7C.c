// C version of the code in asm/src/code_08001A7C.s.

#include "pc.h"
#include "global.h"
#include "entity.h"
#include "room.h"

extern Entity gPlayerEntity;
extern const u8 gUnk_0800232E[];
extern const u8 gUnk_08002342[];
extern const u16 gUnk_0800275C[];
extern const s16 gSineTable[];

extern s16 FixedMul(s16, s16);
extern u32 Random(void);

// Returns the fuser id in the low and the fuser name text id in the high word,
// like the two registers the original returns.
u64 GetFuserIdImpl(Entity* entity) PC_SYMBOL(GetFuserId);
u64 GetFuserIdImpl(Entity* entity) {
    // Which of id, type and type2 must match: type2 type id (little endian).
    static const u32 sMasks[] = { 0x00FFFFFF, 0x00FFFF00, 0x00FF00FF, 0x00FF0000 };
    const u8* entry;
    u32 key;

    if (entity->kind == 3) // ENEMY
        entry = gUnk_0800232E;
    else if (entity->kind == 7) // NPC
        entry = gUnk_08002342;
    else
        return 0;
    key = (entity->id << 16) | (entity->type << 8) | entity->type2;
    for (;;) {
        u32 mask, entryKey;
        entry += 6;
        if (entry[0] == 0)
            return 0;
        mask = sMasks[(entry[1] == 0xFF ? 2 : 0) + (entry[2] == 0xFF ? 1 : 0)];
        entryKey = (entry[0] << 16) | (entry[1] << 8) | entry[2];
        if ((key & mask) == (entryKey & mask))
            return entry[3] | ((u64) * (const u16*)(entry + 4) << 32);
    }
}

// Draws one column of a 4bpp glyph: maps source pixels through a palette lookup
// into either nibble of the destination bytes.
static void BlitGlyphColumn(const u8* src, u8* dest, const u8* lut, u32 x, bool32 transparent) {
    u32 keep = 0xF0;
    s32 i;

    dest += (x >> 3) << 6;
    if (x & 1) {
        keep = 0xF;
        lut += 0x10;
    }
    dest += (x >> 1) & 3;
    for (i = 0; i < 16; i++) {
        u32 v = lut[*src];
        if (!transparent || v != 0)
            *dest = (*dest & keep) | v;
        dest += 4;
        src += 8;
    }
}

void sub_080026C4(u8* src, u8* dest, u8* lut, u32 x) {
    BlitGlyphColumn(src, dest, lut, x, FALSE);
}

void sub_080026F2(u8* src, void* dest, u8* lut, u32 x) {
    BlitGlyphColumn(src, dest, lut, x, TRUE);
}

void UnpackTextNibbles(void* src, u8* dest) {
    const u8* s = src;
    s32 i;
    for (i = 0; i < 16 * 4; i++) {
        *dest++ = *s & 0xF;
        *dest++ = *s++ >> 4;
    }
}

u32 GetNextFunction(Entity* entity) {
    u32 gustJarState = entity->gustJarState;

    if (!(gustJarState & 4) && (entity->contactFlags & 0x80))
        return 1;
    if (entity->knockbackDuration != 0)
        return 2;
    if (entity->health == 0)
        return (entity->action | entity->subAction) ? 3 : 0;
    if (gustJarState & 4)
        return 5;
    if (entity->confusedTime != 0)
        return 4;
    return 0;
}

// Slides along walls: returns a new direction if the collisions block the given one.
static u32 CalcCollisionDirectionOLD(u32 dir, u32 collisions) {
    switch (dir >> 3) {
        case 0:
            if (!(collisions & 0xE))
                return dir;
            if (!(collisions & 0xE004))
                return 8;
            if (!(collisions & 0xE02))
                return 0x18;
            return dir;
        case 1:
            if (!(collisions & 0xE000))
                return dir;
            if (!(collisions & 0x200E))
                return 0;
            if (!(collisions & 0x40E0))
                return 0x10;
            return dir;
        case 2:
            if (!(collisions & 0xE0))
                return dir;
            if (!(collisions & 0xE040))
                return 8;
            if (!(collisions & 0xE20))
                return 0x18;
            return dir;
        default:
            if (!(collisions & 0xE00))
                return dir;
            if (!(collisions & 0x20E))
                return 0;
            if (!(collisions & 0x4E0))
                return 0x10;
            return dir;
    }
}

void LinearMoveDirectionOLD(Entity* entity, u32 speed, u32 dir) {
    u32 collisions;

    if (dir & 0x80)
        return;
    collisions = entity->collisions;
    if (!(dir & 7)) {
        u32 newDir = CalcCollisionDirectionOLD(dir, collisions);
        if (newDir != dir) {
            dir = newDir;
            speed = 0x100;
        }
    }
    collisions &= gUnk_0800275C[dir];
    if (!(collisions & 0xEE00)) {
        u16 sine = gSineTable[dir * 8];
        if (sine != 0)
            entity->x.WORD += (s32)FixedMul(sine, speed) << 8;
    }
    if (!(collisions & 0xEE)) {
        u16 cosine = gSineTable[dir * 8 + 0x40];
        if (cosine != 0)
            entity->y.WORD -= (s32)FixedMul(cosine, speed) << 8;
    }
}

// Moves iframes one step towards 0.
void sub_080028E0(Entity* entity) {
    if (entity->iframes > 0)
        entity->iframes--;
    else if (entity->iframes < 0)
        entity->iframes++;
}

u32 GetRandomByWeight(const u8* weights) {
    s32 r = Random() & 0xFF;
    u32 i = 0;
    do {
        r -= weights[i++];
    } while (r >= 0);
    return i - 1;
}

u32 CheckRectOnScreen(s32 x, s32 y, u32 width, u32 height) {
    if ((u32)(x - ((u16)gRoomControls.scroll_x - gRoomControls.origin_x - width)) >= width * 2 + 0xF0)
        return 0;
    if ((u32)(y - ((u16)gRoomControls.scroll_y - gRoomControls.origin_y - height)) >= height * 2 + 0xA0)
        return 0;
    return 1;
}

u32 CheckPlayerInRegion(u32 centerX, u32 centerY, u32 radiusX, u32 radiusY) {
    if (centerX - (gPlayerEntity.x.HALF_U.HI - gRoomControls.origin_x - radiusX) >= radiusX * 2)
        return 0;
    if (centerY - (gPlayerEntity.y.HALF_U.HI - gRoomControls.origin_y - radiusY) >= radiusY * 2)
        return 0;
    return 1;
}
