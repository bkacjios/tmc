// GBA BIOS calls (asm/lib/libagbsyscall.s) implemented in C.

#include <math.h>
#include <string.h>
#include "pc.h"
#include "global.h"

void CpuSet(const void* src, void* dest, u32 control) {
    u32 count = control & 0x1FFFFF;
    int fixed = (control >> 24) & 1;

    if (control & (1 << 26)) {
        const u32* s = (const u32*)((uintptr_t)src & ~3);
        u32* d = (u32*)((uintptr_t)dest & ~3);
        while (count--) {
            *d++ = *s;
            if (!fixed)
                s++;
        }
    } else {
        const u16* s = (const u16*)((uintptr_t)src & ~1);
        u16* d = (u16*)((uintptr_t)dest & ~1);
        while (count--) {
            *d++ = *s;
            if (!fixed)
                s++;
        }
    }
}

void CpuFastSet(const void* src, void* dest, u32 control) {
    // Copies in units of 8 words.
    CpuSet(src, dest, (1 << 26) | (control & (1 << 24)) | (((control & 0x1FFFFF) + 7) & ~7));
}

// Quotient in the low and remainder in the high word, like the BIOS returns
// them in r0 and r1 (see DivAndMod).
u64 DivImpl(s32 num, s32 denom) PC_SYMBOL(Div);
u64 DivImpl(s32 num, s32 denom) {
    s32 q, r;
    if (denom == 0)
        return 0; // the BIOS never returns
    if (num == INT32_MIN && denom == -1)
        return (u32)INT32_MIN;
    q = num / denom;
    r = num % denom;
    return (u32)q | ((u64)(u32)r << 32);
}

s32 Mod(s32 num, s32 denom) {
    if (denom == 0 || (num == INT32_MIN && denom == -1))
        return 0;
    return num % denom;
}

u16 Sqrt(u32 num) {
    u32 res = 0, bit = 1u << 30;
    while (bit > num)
        bit >>= 2;
    while (bit != 0) {
        if (num >= res + bit) {
            num -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}

static void LZ77UnComp(const void* src, void* dest) {
    const u8* s = src;
    u8* d = dest;
    u32 size = (s[1] | (s[2] << 8) | (s[3] << 16));
    u8* end = d + size;

    s += 4;
    while (d < end) {
        u8 flags = *s++;
        s32 i;
        for (i = 0; i < 8 && d < end; i++, flags <<= 1) {
            if (flags & 0x80) {
                u32 len = (s[0] >> 4) + 3;
                u32 disp = (((s[0] & 0xF) << 8) | s[1]) + 1;
                s += 2;
                while (len-- && d < end) {
                    *d = d[-(s32)disp];
                    d++;
                }
            } else {
                *d++ = *s++;
            }
        }
    }
}

void LZ77UnCompWram(const void* src, void* dest) {
    LZ77UnComp(src, dest);
}

void LZ77UnCompVram(const void* src, void* dest) {
    LZ77UnComp(src, dest);
}

static void RLUnComp(const void* src, void* dest) {
    const u8* s = src;
    u8* d = dest;
    u32 size = (s[1] | (s[2] << 8) | (s[3] << 16));
    u8* end = d + size;

    s += 4;
    while (d < end) {
        u8 flag = *s++;
        if (flag & 0x80) {
            u32 len = (flag & 0x7F) + 3;
            u8 value = *s++;
            while (len-- && d < end)
                *d++ = value;
        } else {
            u32 len = (flag & 0x7F) + 1;
            while (len-- && d < end)
                *d++ = *s++;
        }
    }
}

void RLUnCompWram(const void* src, void* dest) {
    RLUnComp(src, dest);
}

void RLUnCompVram(const void* src, void* dest) {
    RLUnComp(src, dest);
}

// The BIOS uses a 256 entry sine table in 1.14 fixed point.
static s16 Sine(u32 angle) {
    return (s16)lrint(sin((angle & 0xFF) * (2 * M_PI / 256)) * 0x4000);
}

void BgAffineSet(struct BgAffineSrcData* src, struct BgAffineDstData* dest, s32 count) {
    for (; count > 0; count--, src++, dest++) {
        s32 sin = Sine(src->alpha >> 8);
        s32 cos = Sine((src->alpha >> 8) + 0x40);
        s16 pa = (cos * src->sx) >> 14;
        s16 pb = -((sin * src->sx) >> 14);
        s16 pc = (sin * src->sy) >> 14;
        s16 pd = (cos * src->sy) >> 14;
        dest->pa = pa;
        dest->pb = pb;
        dest->pc = pc;
        dest->pd = pd;
        dest->dx = src->texX - (pa * src->scrX + pb * src->scrY);
        dest->dy = src->texY - (pc * src->scrX + pd * src->scrY);
    }
}

void ObjAffineSet(struct ObjAffineSrcData* src, void* dest, s32 count, s32 offset) {
    s16* d = dest;
    for (; count > 0; count--, src++) {
        s32 sin = Sine(src->rotation >> 8);
        s32 cos = Sine((src->rotation >> 8) + 0x40);
        d[0] = (cos * src->xScale) >> 14;
        d = (s16*)((u8*)d + offset);
        d[0] = -((sin * src->xScale) >> 14);
        d = (s16*)((u8*)d + offset);
        d[0] = (sin * src->yScale) >> 14;
        d = (s16*)((u8*)d + offset);
        d[0] = (cos * src->yScale) >> 14;
        d = (s16*)((u8*)d + offset);
    }
}

#if UINTPTR_MAX > 0xFFFFFFFF
// The game's RAM variables (see gen_linker.py). EWRAM and IWRAM ones are mixed
// there, so a reset of either clears all of them.
extern u8 __gba_ram_start[], __gba_ram_end[];
#endif

void RegisterRamReset(u32 flags) {
#if UINTPTR_MAX > 0xFFFFFFFF
    if (flags & (RESET_EWRAM | RESET_IWRAM))
        memset(__gba_ram_start, 0, __gba_ram_end - __gba_ram_start);
    if (flags & RESET_EWRAM)
        memset(PC_EWRAM, 0, 0x1000);
    if (flags & RESET_IWRAM)
        memset(PC_IWRAM + PC_IWRAM_SIZE - 0x1000, 0, 0x1000 - 0x200); // keeps the stack area
#else
    if (flags & RESET_EWRAM)
        memset(PC_EWRAM, 0, PC_EWRAM_SIZE);
    if (flags & RESET_IWRAM)
        memset(PC_IWRAM, 0, PC_IWRAM_SIZE - 0x200); // keeps the stack area
#endif
    if (flags & RESET_PALETTE)
        memset(PC_PLTT, 0, PC_PLTT_SIZE);
    if (flags & RESET_VRAM)
        memset(PC_VRAM, 0, PC_VRAM_SIZE);
    if (flags & RESET_OAM)
        memset(PC_OAM, 0, PC_OAM_SIZE);
    if (flags & RESET_REGS) {
        u16 keys = PC_REG16(0x130);
        PcResetIo();
        PC_REG16(0x130) = keys;
        PC_REG16(0) = 0x80; // DISPCNT: forced blank
    }
}

void SoundBiasReset(void) {
}

void SoundBiasSet(void) {
}

void PcSystemCall(u32 number) {
    // Only Stop (3), sleep mode until a button is pressed, is used.
    (void)number;
}
