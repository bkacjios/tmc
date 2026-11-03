// Scanline renderer for the GBA picture processing unit, modes 0-2: text and
// affine backgrounds, sprites (incl. affine), windows, blending and mosaic.
// Reads the I/O registers, palette, VRAM and OAM at their GBA addresses.

#include <string.h>
#include "pc.h"

#define WIDTH 240
#define HEIGHT 160

#define REG(o) PC_REG16(o)
#define DISPCNT REG(0x00)
#define BGCNT(n) REG(0x08 + 2 * (n))
#define BGHOFS(n) REG(0x10 + 4 * (n))
#define BGVOFS(n) REG(0x12 + 4 * (n))
#define WIN0H REG(0x40)
#define WIN1H REG(0x42)
#define WIN0V REG(0x44)
#define WIN1V REG(0x46)
#define WININ REG(0x48)
#define WINOUT REG(0x4A)
#define MOSAIC REG(0x4C)
#define BLDCNT REG(0x50)
#define BLDALPHA REG(0x52)
#define BLDY REG(0x54)

#define LAYER_OBJ 4
#define LAYER_BACKDROP 5

#define OPAQUE 0x8000

static const uint16_t* const sBgPalette = (const uint16_t*)PC_PLTT;
static const uint16_t* const sObjPalette = (const uint16_t*)(PC_PLTT + 0x200);

uint32_t gPcFramebuffer[HEIGHT][WIDTH];

static uint16_t sBgLine[4][WIDTH]; // color | OPAQUE
static uint16_t sObjLine[WIDTH];
static uint8_t sObjPrio[WIDTH];
static uint8_t sObjSemi[WIDTH];
static uint8_t sObjWindow[WIDTH];

// Affine background reference points, latched like the hardware does.
static int32_t sAffineX[2], sAffineY[2];
static uint32_t sAffineRegX[2], sAffineRegY[2];

static int32_t SignExtend28(uint32_t v) {
    return (int32_t)(v << 4) >> 4;
}

static void RenderTextBg(int bg, int line) {
    uint16_t cnt = BGCNT(bg);
    const uint8_t* chars = PC_VRAM + ((cnt >> 2) & 3) * 0x4000;
    const uint8_t* screen = PC_VRAM + ((cnt >> 8) & 0x1F) * 0x800;
    int is8bpp = cnt & 0x80;
    int width = (cnt & 0x4000) ? 512 : 256;
    int height = (cnt & 0x8000) ? 512 : 256;
    int mosaicH = (cnt & 0x40) ? (MOSAIC & 0xF) + 1 : 1;
    int mosaicV = (cnt & 0x40) ? ((MOSAIC >> 4) & 0xF) + 1 : 1;
    int y = ((line - line % mosaicV) + BGVOFS(bg)) & (height - 1);
    int hofs = BGHOFS(bg);
    int x;

    for (x = 0; x < WIDTH; x++) {
        int px = ((x - x % mosaicH) + hofs) & (width - 1);
        int tx = px >> 3, ty = y >> 3;
        int block = (tx >> 5) + ((ty >> 5) * (width == 512 ? 2 : 1));
        uint16_t entry = *(const uint16_t*)(screen + block * 0x800 + (((ty & 31) << 5) + (tx & 31)) * 2);
        int tile = entry & 0x3FF;
        int fx = (px & 7) ^ ((entry & 0x400) ? 7 : 0);
        int fy = (y & 7) ^ ((entry & 0x800) ? 7 : 0);
        uint32_t offset;
        int index;

        if (is8bpp) {
            offset = (chars - PC_VRAM) + tile * 64 + fy * 8 + fx;
            if (offset >= 0x10000) {
                sBgLine[bg][x] = 0;
                continue;
            }
            index = PC_VRAM[offset];
            sBgLine[bg][x] = index ? (sBgPalette[index] | OPAQUE) : 0;
        } else {
            offset = (chars - PC_VRAM) + tile * 32 + fy * 4 + (fx >> 1);
            if (offset >= 0x10000) {
                sBgLine[bg][x] = 0;
                continue;
            }
            index = (fx & 1) ? PC_VRAM[offset] >> 4 : PC_VRAM[offset] & 0xF;
            sBgLine[bg][x] = index ? (sBgPalette[(entry >> 12) * 16 + index] | OPAQUE) : 0;
        }
    }
}

static void RenderAffineBg(int bg, int line) {
    int n = bg - 2;
    uint16_t cnt = BGCNT(bg);
    const uint8_t* chars = PC_VRAM + ((cnt >> 2) & 3) * 0x4000;
    const uint8_t* screen = PC_VRAM + ((cnt >> 8) & 0x1F) * 0x800;
    int size = 128 << (cnt >> 14);
    int wrap = cnt & 0x2000;
    int16_t pa = (int16_t)REG(0x20 + n * 0x10);
    int16_t pc = (int16_t)REG(0x24 + n * 0x10);
    int32_t rx = sAffineX[n], ry = sAffineY[n];
    int x;

    (void)line;
    for (x = 0; x < WIDTH; x++, rx += pa, ry += pc) {
        int tx = rx >> 8, ty = ry >> 8;
        int tile, index;
        if (tx < 0 || ty < 0 || tx >= size || ty >= size) {
            if (!wrap) {
                sBgLine[bg][x] = 0;
                continue;
            }
            tx &= size - 1;
            ty &= size - 1;
        }
        tile = screen[(ty >> 3) * (size >> 3) + (tx >> 3)];
        index = chars[tile * 64 + (ty & 7) * 8 + (tx & 7)];
        sBgLine[bg][x] = index ? (sBgPalette[index] | OPAQUE) : 0;
    }
}

static void LatchAffine(int line) {
    int n;
    for (n = 0; n < 2; n++) {
        uint32_t regX = PC_REG32(0x28 + n * 0x10);
        uint32_t regY = PC_REG32(0x2C + n * 0x10);
        if (line == 0 || regX != sAffineRegX[n]) {
            sAffineRegX[n] = regX;
            sAffineX[n] = SignExtend28(regX);
        }
        if (line == 0 || regY != sAffineRegY[n]) {
            sAffineRegY[n] = regY;
            sAffineY[n] = SignExtend28(regY);
        }
    }
}

static void AdvanceAffine(void) {
    int n;
    for (n = 0; n < 2; n++) {
        sAffineX[n] += (int16_t)REG(0x22 + n * 0x10); // PB
        sAffineY[n] += (int16_t)REG(0x26 + n * 0x10); // PD
    }
}

static const uint8_t sObjSizes[3][4][2] = {
    { { 8, 8 }, { 16, 16 }, { 32, 32 }, { 64, 64 } },
    { { 16, 8 }, { 32, 8 }, { 32, 16 }, { 64, 32 } },
    { { 8, 16 }, { 8, 32 }, { 16, 32 }, { 32, 64 } },
};

static void RenderObjects(int line) {
    const uint16_t* oam = (const uint16_t*)PC_OAM;
    int oneDimensional = DISPCNT & 0x40;
    int mosaicH = (MOSAIC >> 8 & 0xF) + 1;
    int mosaicV = (MOSAIC >> 12 & 0xF) + 1;
    int i;

    memset(sObjLine, 0, sizeof(sObjLine));
    memset(sObjSemi, 0, sizeof(sObjSemi));
    memset(sObjWindow, 0, sizeof(sObjWindow));
    memset(sObjPrio, 4, sizeof(sObjPrio));

    for (i = 0; i < 128; i++) {
        uint16_t a0 = oam[i * 4], a1 = oam[i * 4 + 1], a2 = oam[i * 4 + 2];
        int affine = a0 & 0x100;
        int mode = (a0 >> 10) & 3;
        int shape = a0 >> 14;
        int w, h, bw, bh, x, y, sx, objLine;
        int is8bpp = a0 & 0x2000;
        int tile = a2 & 0x3FF, prio = (a2 >> 10) & 3, pal = a2 >> 12;
        int16_t pa = 0x100, pb = 0, pc = 0, pd = 0x100;

        if ((!affine && (a0 & 0x200)) || mode == 3 || shape == 3)
            continue;
        w = sObjSizes[shape][a1 >> 14][0];
        h = sObjSizes[shape][a1 >> 14][1];
        bw = w;
        bh = h;
        if (affine && (a0 & 0x200)) { // double size
            bw *= 2;
            bh *= 2;
        }
        y = a0 & 0xFF;
        if (y >= HEIGHT)
            y -= 256;
        objLine = line;
        if (a0 & 0x1000)
            objLine -= (objLine - y) % mosaicV;
        if (line < y || line >= y + bh)
            continue;
        x = a1 & 0x1FF;
        if (x >= WIDTH)
            x -= 512;
        if (affine) {
            int param = (a1 >> 9) & 0x1F;
            pa = oam[param * 16 + 3];
            pb = oam[param * 16 + 7];
            pc = oam[param * 16 + 11];
            pd = oam[param * 16 + 15];
        }

        for (sx = 0; sx < bw; sx++) {
            int px = x + sx, tx, ty, tileNum, index;
            uint32_t offset;
            uint16_t color;

            if (px < 0 || px >= WIDTH)
                continue;
            if (affine) {
                int dx = sx - bw / 2, dy = (objLine - y) - bh / 2;
                tx = ((pa * dx + pb * dy) >> 8) + w / 2;
                ty = ((pc * dx + pd * dy) >> 8) + h / 2;
                if (tx < 0 || ty < 0 || tx >= w || ty >= h)
                    continue;
            } else {
                tx = (a0 & 0x1000) ? sx - sx % mosaicH : sx;
                ty = objLine - y;
                if (a1 & 0x1000)
                    tx = w - 1 - tx;
                if (a1 & 0x2000)
                    ty = h - 1 - ty;
            }
            if (oneDimensional)
                tileNum = tile + ((ty >> 3) * (w >> 3) + (tx >> 3)) * (is8bpp ? 2 : 1);
            else
                tileNum = tile + (ty >> 3) * 32 + (tx >> 3) * (is8bpp ? 2 : 1);
            tileNum &= 0x3FF;
            if (is8bpp) {
                offset = 0x10000 + tileNum * 32 + (ty & 7) * 8 + (tx & 7);
                index = offset < PC_VRAM_SIZE ? PC_VRAM[offset] : 0;
                color = sObjPalette[index];
            } else {
                offset = 0x10000 + tileNum * 32 + (ty & 7) * 4 + ((tx & 7) >> 1);
                index = offset < PC_VRAM_SIZE ? PC_VRAM[offset] : 0;
                index = (tx & 1) ? index >> 4 : index & 0xF;
                color = sObjPalette[pal * 16 + index];
            }
            if (index == 0)
                continue;
            if (mode == 2) {
                sObjWindow[px] = 1;
                continue;
            }
            if (prio < sObjPrio[px]) {
                sObjLine[px] = color | OPAQUE;
                sObjPrio[px] = prio;
                sObjSemi[px] = mode == 1;
            }
        }
    }
}

// Window range check. start > end wraps around; an end past the screen is clamped.
static int InRange(int v, int start, int end, int limit) {
    if (start <= end) {
        if (end > limit)
            end = limit;
        return v >= start && v < end;
    }
    return v >= start || v < end;
}

static uint16_t Blend(uint16_t a, uint16_t b, int eva, int evb) {
    int r = ((a & 31) * eva + (b & 31) * evb) >> 4;
    int g = (((a >> 5) & 31) * eva + ((b >> 5) & 31) * evb) >> 4;
    int bl = (((a >> 10) & 31) * eva + ((b >> 10) & 31) * evb) >> 4;
    if (r > 31)
        r = 31;
    if (g > 31)
        g = 31;
    if (bl > 31)
        bl = 31;
    return r | (g << 5) | (bl << 10);
}

static uint16_t Brighten(uint16_t c, int evy) {
    int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
    r += ((31 - r) * evy) >> 4;
    g += ((31 - g) * evy) >> 4;
    b += ((31 - b) * evy) >> 4;
    return r | (g << 5) | (b << 10);
}

static uint16_t Darken(uint16_t c, int evy) {
    int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
    r -= (r * evy) >> 4;
    g -= (g * evy) >> 4;
    b -= (b * evy) >> 4;
    return r | (g << 5) | (b << 10);
}

static uint32_t ToRgb(uint16_t c) {
    uint32_t r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
    r = (r << 3) | (r >> 2);
    g = (g << 3) | (g >> 2);
    b = (b << 3) | (b >> 2);
    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

static void ComposeLine(int line, int bgEnabled) {
    uint16_t dispcnt = DISPCNT;
    int windows = dispcnt & 0xE000;
    int win0 = (dispcnt & 0x2000) && InRange(line, WIN0V >> 8, WIN0V & 0xFF, HEIGHT);
    int win1 = (dispcnt & 0x4000) && InRange(line, WIN1V >> 8, WIN1V & 0xFF, HEIGHT);
    uint16_t bldcnt = BLDCNT;
    int effect = (bldcnt >> 6) & 3;
    int eva = BLDALPHA & 0x1F, evb = (BLDALPHA >> 8) & 0x1F, evy = BLDY & 0x1F;
    int x;

    if (eva > 16)
        eva = 16;
    if (evb > 16)
        evb = 16;
    if (evy > 16)
        evy = 16;

    for (x = 0; x < WIDTH; x++) {
        int mask = 0x3F;
        int topLayer = LAYER_BACKDROP, secondLayer = LAYER_BACKDROP;
        int topPrio = 5, secondPrio = 5;
        uint16_t top = sBgPalette[0], second = sBgPalette[0];
        int bg;

        if (windows) {
            if (win0 && InRange(x, WIN0H >> 8, WIN0H & 0xFF, WIDTH))
                mask = WININ & 0x3F;
            else if (win1 && InRange(x, WIN1H >> 8, WIN1H & 0xFF, WIDTH))
                mask = (WININ >> 8) & 0x3F;
            else if ((dispcnt & 0x8000) && sObjWindow[x])
                mask = (WINOUT >> 8) & 0x3F;
            else
                mask = WINOUT & 0x3F;
        }

        // Find the top two layers. Ties go to OBJ, then the lower BG.
        if ((mask & 0x10) && (sObjLine[x] & OPAQUE)) {
            topLayer = LAYER_OBJ;
            topPrio = sObjPrio[x];
            top = sObjLine[x];
        }
        for (bg = 0; bg < 4; bg++) {
            int prio;
            if (!(bgEnabled & (1 << bg)) || !(mask & (1 << bg)) || !(sBgLine[bg][x] & OPAQUE))
                continue;
            prio = BGCNT(bg) & 3;
            if (prio < topPrio) {
                secondLayer = topLayer;
                secondPrio = topPrio;
                second = top;
                topLayer = bg;
                topPrio = prio;
                top = sBgLine[bg][x];
            } else if (prio < secondPrio) {
                secondLayer = bg;
                secondPrio = prio;
                second = sBgLine[bg][x];
            }
        }
        top &= 0x7FFF;
        second &= 0x7FFF;

        if (mask & 0x20) {
            int topTarget = bldcnt & (1 << topLayer);
            int secondTarget = bldcnt & (1 << (secondLayer + 8));
            if (topLayer == LAYER_OBJ && sObjSemi[x] && secondTarget)
                top = Blend(top, second, eva, evb);
            else if (effect == 1 && topTarget && secondTarget)
                top = Blend(top, second, eva, evb);
            else if (effect == 2 && topTarget)
                top = Brighten(top, evy);
            else if (effect == 3 && topTarget)
                top = Darken(top, evy);
        }
        gPcFramebuffer[line][x] = ToRgb(top);
    }
}

void PcRenderLine(int line) {
    uint16_t dispcnt = DISPCNT;
    int mode = dispcnt & 7;
    int bgEnabled = (dispcnt >> 8) & 0xF;
    int bg;

    LatchAffine(line);
    if (dispcnt & 0x80) { // forced blank
        int x;
        for (x = 0; x < WIDTH; x++)
            gPcFramebuffer[line][x] = 0xFFFFFFFFu;
        AdvanceAffine();
        return;
    }
    if (mode == 1)
        bgEnabled &= 0x7;
    else if (mode == 2)
        bgEnabled &= 0xC;
    else if (mode > 2)
        bgEnabled = 0; // bitmap modes are not used by the game

    for (bg = 0; bg < 4; bg++) {
        if (!(bgEnabled & (1 << bg)))
            continue;
        if (mode == 0 || (mode == 1 && bg < 2))
            RenderTextBg(bg, line);
        else
            RenderAffineBg(bg, line);
    }
    if (dispcnt & 0x1000)
        RenderObjects(line);
    else
        memset(sObjLine, 0, sizeof(sObjLine)), memset(sObjWindow, 0, sizeof(sObjWindow));
    ComposeLine(line, bgEnabled);
    AdvanceAffine();
}
