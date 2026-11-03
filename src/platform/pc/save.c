// EEPROM save memory backed by a file. Replaces src/eeprom.c.
//
// The 8 KiB image is stored as 64-bit blocks of four little-endian halfwords,
// in the order EEPROMRead returns them.

#include <stdio.h>
#include <string.h>
#include "pc.h"
#include "global.h"
#include "gba/eeprom.h"

#define EEPROM_SIZE 0x2000
#define SAVE_NAME "tmc.sav"

static u8 sEeprom[EEPROM_SIZE];
static u32 sBlocks = EEPROM_SIZE / 8;
static int sLoaded;
static int sDirty;
static int sWriteFailed; // reported once, not on every retry

static void Load(void) {
    FILE* f;
    if (sLoaded)
        return;
    sLoaded = 1;
    memset(sEeprom, 0xFF, sizeof(sEeprom));
    f = fopen(PcUserFile(SAVE_NAME), "rb");
    if (f != NULL) {
        if (fread(sEeprom, 1, sizeof(sEeprom), f) != sizeof(sEeprom))
            PcError("%s is shorter than %d bytes", PcUserFile(SAVE_NAME), EEPROM_SIZE);
        fclose(f);
    }
}

void PcSaveFlush(void) {
    FILE* f;
    if (!sDirty)
        return;
    f = fopen(PcUserFile(SAVE_NAME), "wb");
    if (f == NULL) {
        if (!sWriteFailed)
            PcError("cannot save the game to %s", PcUserFile(SAVE_NAME));
        sWriteFailed = 1;
        return;
    }
    sWriteFailed = 0;
    fwrite(sEeprom, 1, sizeof(sEeprom), f);
    fclose(f);
    sDirty = 0;
}

u16 EEPROMConfigure(u16 size) {
    Load();
    // 4: 512 bytes, 0x40: 8 KiB. The game uses 0x40.
    sBlocks = size == 4 ? 0x40 : 0x400;
    return size == 4 || size == 0x40 ? 0 : 1;
}

u16 EEPROMRead(u16 address, u16* data) {
    Load();
    if (address >= sBlocks)
        return EEPROM_OUT_OF_RANGE;
    memcpy(data, sEeprom + address * 8, 8);
    return 0;
}

u16 EEPROMWrite0_8k_Check(u16 address, const u16* data) {
    Load();
    if (address >= sBlocks)
        return EEPROM_OUT_OF_RANGE;
    if (memcmp(sEeprom + address * 8, data, 8) != 0) {
        memcpy(sEeprom + address * 8, data, 8);
        sDirty = 1; // written out at the end of the frame
    }
    return 0;
}

u16 EEPROMCompare(u16 address, const u16* data) {
    Load();
    if (address >= sBlocks)
        return EEPROM_OUT_OF_RANGE;
    return memcmp(sEeprom + address * 8, data, 8) == 0 ? 0 : EEPROM_COMPARE_FAILED;
}
