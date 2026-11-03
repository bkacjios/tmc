// The PC settings file, tmc.cfg (next to tmc.sav): the controls (input.c) and
// the system options of the settings menu (volume, display). One "name=value"
// per line; lines starting with # are comments.

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"

// Files kept between runs go in SDL's per-user data directory: %APPDATA%\tmc\
// on Windows, $XDG_DATA_HOME/tmc/ (~/.local/share/tmc/) on Linux. Headless
// runs (tests, the menu art build step) use the working directory instead, so
// that they leave the player's files alone. Earlier builds kept the files in
// the working directory: one found there is copied over the first time.
const char* PcUserFile(const char* name) {
    static char* sDir;
    static char path[4096];
    if (sDir == NULL && getenv("TMC_HEADLESS") == NULL)
        sDir = SDL_GetPrefPath(NULL, "tmc");
    if (sDir == NULL)
        return name;
    snprintf(path, sizeof(path), "%s%s", sDir, name);
    if (!SDL_GetPathInfo(path, NULL) && SDL_GetPathInfo(name, NULL))
        SDL_CopyFile(name, path);
    return path;
}

#define CONFIG_NAME "tmc.cfg"

PcOptions gPcOptions = {
    .volume = PC_MAX_VOLUME / 2,
    .integerScale = 1,
};

static const struct {
    const char* name;
    int* value;
    int max;
} sOptions[] = {
    { "volume", &gPcOptions.volume, PC_MAX_VOLUME },
    { "fullscreen", &gPcOptions.fullscreen, 1 },
    { "smooth", &gPcOptions.smooth, 1 },
    { "integer_scale", &gPcOptions.integerScale, 1 },
};
#define NUM_OPTIONS (sizeof(sOptions) / sizeof(sOptions[0]))

void PcLoadConfig(void) {
    char line[256];
    FILE* f;

    PcDefaultBindings();
    f = fopen(PcUserFile(CONFIG_NAME), "r");
    if (f == NULL)
        return;
    while (fgets(line, sizeof(line), f) != NULL) {
        char* value = strchr(line, '=');
        size_t i;
        if (line[0] == '#' || value == NULL)
            continue;
        *value++ = '\0';
        value[strcspn(value, "\r\n")] = '\0';
        for (i = 0; i < NUM_OPTIONS; i++) {
            int v;
            if (strcmp(line, sOptions[i].name) == 0 && sscanf(value, "%d", &v) == 1 && v >= 0 &&
                v <= sOptions[i].max)
                *sOptions[i].value = v;
        }
        PcParseBinding(line, value);
    }
    fclose(f);
}

void PcSaveConfig(void) {
    const char* path = PcUserFile(CONFIG_NAME);
    FILE* f = fopen(path, "w");
    size_t i;
    if (f == NULL) {
        PcError("cannot save the settings to %s", path);
        return;
    }
    fprintf(f, "# Settings, changed in the settings menu (Esc).\n");
    fprintf(f, "# Controls: button=keyboard key,gamepad button.\n");
    PcWriteBindings(f);
    fprintf(f, "# volume: 0-%d. fullscreen, smooth (filtering), integer_scale: 0 or 1.\n", PC_MAX_VOLUME);
    for (i = 0; i < NUM_OPTIONS; i++)
        fprintf(f, "%s=%d\n", sOptions[i].name, *sOptions[i].value);
    fclose(f);
}
