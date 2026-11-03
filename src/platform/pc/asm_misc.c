// C versions of small handwritten Thumb routines:
// asm/src/code_08000E44.s, asm/src/code_08000F10.s, asm/src/code_08007CAC.s

#include "global.h"
#include "asm.h"
#include "player.h"

extern u32 gRand;

// Returns the sign of value: 0, 1 or -1.
u32 sub_08000E44(u32 value) {
    if (value == 0)
        return 0;
    return (s32)value < 0 ? (u32)-1 : 1;
}

u32 Random(void) {
    u32 r = gRand * 3;
    r = (r >> 13) | (r << 19);
    gRand = r;
    return r >> 1;
}

// Population count.
u32 sub_08000E62(u32 x) {
    x = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x & 0x0F0F0F0F) + ((x >> 4) & 0x0F0F0F0F);
    x = (x & 0x00FF00FF) + ((x >> 8) & 0x00FF00FF);
    return (x + (x << 16)) >> 16;
}

// Queues a copy for the next VBlank.
static void QueueResource(const void* src, void* dest, u32 size, u8 type) {
    QueuedResource* entry;
    if (gUnk_03003DE0 >= 0x28)
        return;
    entry = &gUnk_03000C30[gUnk_03003DE0++];
    entry->type = type;
    entry->size = size;
    entry->src = src;
    entry->dest = dest;
}

void sub_08000E92(const void* src, void* dest, u32 size) {
    QueueResource(src, dest, size, 1);
}

void LoadResourceAsync(const void* src, void* dest, u32 size) {
    QueueResource(src, dest, size, 0);
}

u32 CheckBits(void* bits, u32 start, u32 count) {
    extern u32 ram_CheckBits(void*, u32, u32);
    return ram_CheckBits(bits, start, count);
}

void SumDropProbabilities(s16* out, const s16* a, const s16* b, const s16* c) {
    s32 i;
    for (i = 15; i >= 0; i--)
        out[i] = a[i] + b[i] + c[i];
}

u32 SumDropProbabilities2(s16* out, const s16* a, const s16* b, const s16* c) {
    s32 i;
    u32 sum = 0;
    for (i = 15; i >= 0; i--) {
        s32 v = a[i] + b[i] + c[i];
        if (v < 0)
            v = 0;
        out[i] = v;
        sum += v;
    }
    return sum;
}

void sub_08007DCE(Entity* this) {
    DoPlayerAction((PlayerEntity*)this);
}

const KeyValuePair* FindEntryForKey(u32 key, const KeyValuePair* list) {
    for (; list->key != 0; list++) {
        if (list->key == key)
            return list;
    }
    return NULL;
}

u32 FindValueForKey(u32 key, const KeyValuePair* list) {
    const KeyValuePair* entry = FindEntryForKey(key, list);
    return entry != NULL ? entry->value : 0;
}
