// DMA. Immediate transfers are done on the spot. Transfers timed to VBlank or
// HBlank stay in the DMA registers and are run by PcRunTimedDma.

#include <string.h>
#include "pc.h"
#include "global.h"

#define DMA_REG_BASE 0xB0
#define DMA_REG_SIZE 0xC

#define CNT_DEST(c) (((c) >> 21) & 3)
#define CNT_SRC(c) (((c) >> 23) & 3)
#define CNT_REPEAT(c) ((c) & (1u << 25))
#define CNT_32BIT(c) ((c) & (1u << 26))
#define CNT_TIMING(c) (((c) >> 28) & 3)
#define CNT_ENABLE(c) ((c) & (1u << 31))

enum { ADDR_INC, ADDR_DEC, ADDR_FIXED, ADDR_RELOAD };

// Internal source and destination addresses, which advance across repeats.
static uint32_t sSrc[4];
static uint32_t sDest[4];

static void Transfer(uint32_t ch) {
    uint32_t base = DMA_REG_BASE + ch * DMA_REG_SIZE;
    uint32_t control = PC_REG32(base + 8);
    uint32_t count = control & (ch == 3 ? 0xFFFF : 0x3FFF);
    uint32_t unit = CNT_32BIT(control) ? 4 : 2;
    int32_t srcStep, destStep;
    uint32_t src = sSrc[ch], dest = sDest[ch];

    if (count == 0)
        count = ch == 3 ? 0x10000 : 0x4000;
    srcStep = CNT_SRC(control) == ADDR_INC ? unit : CNT_SRC(control) == ADDR_DEC ? -(int32_t)unit : 0;
    destStep = (CNT_DEST(control) == ADDR_INC || CNT_DEST(control) == ADDR_RELOAD) ? unit
               : CNT_DEST(control) == ADDR_DEC                                     ? -(int32_t)unit
                                                                                   : 0;
    if (unit == 4) {
        src &= ~3;
        dest &= ~3;
        while (count--) {
            *(volatile uint32_t*)dest = *(volatile uint32_t*)src;
            src += srcStep;
            dest += destStep;
        }
    } else {
        src &= ~1;
        dest &= ~1;
        while (count--) {
            *(volatile uint16_t*)dest = *(volatile uint16_t*)src;
            src += srcStep;
            dest += destStep;
        }
    }
    sSrc[ch] = src;
    sDest[ch] = CNT_DEST(control) == ADDR_RELOAD ? PC_REG32(base + 4) : dest;
    if (!CNT_REPEAT(control) || CNT_TIMING(control) == 0)
        PC_REG32(base + 8) = control & ~(1u << 31);
}

void PcDmaSet(u32 ch, const void* src, void* dest, u32 control) {
    uint32_t base = DMA_REG_BASE + ch * DMA_REG_SIZE;

    PC_REG32(base) = (uint32_t)src;
    PC_REG32(base + 4) = (uint32_t)dest;
    PC_REG32(base + 8) = control;
    sSrc[ch] = (uint32_t)src;
    sDest[ch] = (uint32_t)dest;
    if (CNT_ENABLE(control) && CNT_TIMING(control) == 0)
        Transfer(ch);
}

void PcRunTimedDma(uint32_t timing) {
    uint32_t ch;
    for (ch = 0; ch < 4; ch++) {
        uint32_t control = PC_REG32(DMA_REG_BASE + ch * DMA_REG_SIZE + 8);
        if (CNT_ENABLE(control) && CNT_TIMING(control) == timing)
            Transfer(ch);
    }
}
