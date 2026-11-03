// The ROM and BIOS built into the executable (EMBED_ROM=1 in PC.mk), so that
// it runs without baserom.gba or gba_bios.bin next to it. Without them, the
// pointers are NULL and the files are looked up at runtime (rom.c, bootintro.c).
//
// Such an executable contains Nintendo's ROM and BIOS: it is for personal use
// only, not for distribution.

#include "pc.h"

#ifdef PC_EMBED_ROM
static const uint8_t sRom[] = {
#embed PC_EMBED_ROM limit(PC_ROM_SIZE)
};
_Static_assert(sizeof(sRom) == PC_ROM_SIZE, "baserom.gba is smaller than 16 MiB");
const uint8_t* const gPcEmbeddedRom = sRom;
#else
const uint8_t* const gPcEmbeddedRom = NULL;
#endif

#ifdef PC_EMBED_BIOS
static const uint8_t sBios[] = {
#embed PC_EMBED_BIOS limit(PC_BIOS_SIZE)
};
_Static_assert(sizeof(sBios) == PC_BIOS_SIZE, "gba_bios.bin is not a 16 KiB GBA BIOS");
const uint8_t* const gPcEmbeddedBios = sBios;
#else
const uint8_t* const gPcEmbeddedBios = NULL;
#endif
