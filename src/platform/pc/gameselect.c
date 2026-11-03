// Game select menu, shown after the Nintendo/Capcom logos: Minish Cap, Oracle
// of Ages or Oracle of Seasons, each with its title screen. Ages starts the
// Minish Cap game in Oracle mode (see oracle.c); Seasons shows a message and
// returns to the menu.
//
// The title screen art is loaded from menu/*.bmp next to the executable (made
// by PC.mk). Missing art is drawn as a plain panel.

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "pc.h"

// Logical resolution of the menu, scaled to the window like the game.
#define MENU_W 720
#define MENU_H 480

#define KEY_A (1 << 0)
#define KEY_B (1 << 1)
#define KEY_START (1 << 3)
#define KEY_RIGHT (1 << 4)
#define KEY_LEFT (1 << 5)

typedef struct {
    const char* name;
    const char* art;
    int playable;
    SDL_Texture* texture;
} GameEntry;

static GameEntry sGames[] = {
    { "The Minish Cap", "minishcap.bmp", 1, NULL },
    { "Oracle of Ages", "ages.bmp", 1, NULL },
    { "Oracle of Seasons", "seasons.bmp", 0, NULL },
};
#define NUM_GAMES ((int)(sizeof(sGames) / sizeof(sGames[0])))

static SDL_Texture* LoadArt(SDL_Renderer* renderer, const char* file) {
    char path[4096];
    SDL_Surface* surface;
    SDL_Texture* texture;

    snprintf(path, sizeof(path), "%smenu/%s", SDL_GetBasePath(), file);
    surface = SDL_LoadBMP(path);
    if (surface == NULL)
        return NULL;
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (texture != NULL)
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    return texture;
}

// Draws text with SDL's built-in 8x8 font, centered at x, scaled up.
static void DrawText(SDL_Renderer* renderer, const char* text, float x, float y, float scale) {
    float w = (float)SDL_strlen(text) * SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * scale;
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_RenderDebugText(renderer, (x - w / 2) / scale, y / scale, text);
    SDL_SetRenderScale(renderer, 1, 1);
}

static void DrawCard(SDL_Renderer* renderer, GameEntry* game, SDL_FRect* box, int selected) {
    SDL_FRect art = { box->x + 8, box->y + 8, box->w - 16, box->h - 48 };
    float tw, th, scale;

    SDL_SetRenderDrawColor(renderer, selected ? 0xF8 : 0x40, selected ? 0xD0 : 0x40, selected ? 0x30 : 0x58, 255);
    SDL_RenderFillRect(renderer, box);
    SDL_SetRenderDrawColor(renderer, 0x10, 0x10, 0x20, 255);
    SDL_RenderFillRect(renderer, &art);

    if (game->texture != NULL) {
        // Fit the title screen into the art area, keeping its aspect ratio.
        SDL_FRect dst;
        SDL_GetTextureSize(game->texture, &tw, &th);
        scale = SDL_min(art.w / tw, art.h / th);
        dst.w = tw * scale;
        dst.h = th * scale;
        dst.x = art.x + (art.w - dst.w) / 2;
        dst.y = art.y + (art.h - dst.h) / 2;
        SDL_RenderTexture(renderer, game->texture, NULL, &dst);
    }

    SDL_SetRenderDrawColor(renderer, selected ? 0x20 : 0xE0, selected ? 0x18 : 0xE0, selected ? 0x10 : 0xE0, 255);
    DrawText(renderer, game->name, box->x + box->w / 2, box->y + box->h - 30, 1.5f);
}

static void DrawMenu(SDL_Renderer* renderer, int selected, const char* message) {
    const float cardW = 224, cardH = 240, gap = 8;
    float x = (MENU_W - (cardW * NUM_GAMES + gap * (NUM_GAMES - 1))) / 2;
    int i;

    SDL_SetRenderDrawColor(renderer, 0x18, 0x18, 0x30, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0xF0, 0xF0, 0xF0, 255);
    DrawText(renderer, "SELECT A GAME", MENU_W / 2, 48, 3);

    for (i = 0; i < NUM_GAMES; i++) {
        SDL_FRect box = { x + i * (cardW + gap), 112, cardW, cardH };
        DrawCard(renderer, &sGames[i], &box, i == selected);
    }

    SDL_SetRenderDrawColor(renderer, 0xC0, 0xC0, 0xD0, 255);
    if (message != NULL) {
        SDL_FRect panel = { 96, 384, MENU_W - 192, 72 };
        SDL_SetRenderDrawColor(renderer, 0x60, 0x18, 0x18, 255);
        SDL_RenderFillRect(renderer, &panel);
        SDL_SetRenderDrawColor(renderer, 0xF0, 0xF0, 0xF0, 255);
        DrawText(renderer, message, MENU_W / 2, 400, 1.5f);
        DrawText(renderer, "Press A or B to go back", MENU_W / 2, 428, 1.5f);
    } else {
        DrawText(renderer, "LEFT/RIGHT: choose     A/START: play", MENU_W / 2, 400, 1.5f);
    }
    PcRenderPresent();
}

// TMC_MENU_SHOT=path saves the first menu frame and exits (for testing).
static void SaveMenuShot(SDL_Renderer* renderer) {
    const char* path = SDL_getenv("TMC_MENU_SHOT");
    SDL_Surface* surface;
    if (path == NULL)
        return;
    surface = SDL_RenderReadPixels(renderer, NULL);
    if (surface == NULL || !SDL_SaveBMP(surface, path))
        SDL_Log("tmc: cannot save %s: %s", path, SDL_GetError());
    SDL_DestroySurface(surface);
    exit(0);
}

void PcGameSelect(void) {
    SDL_Renderer* renderer = PcGetRenderer();
    char message[64];
    int selected = 0, showMessage = 0, i;
    uint16_t held, prev = 0xFFFF; // ignore buttons still held from the logos

    i = PcPreselectedGame();
    if (i >= 0) {
        gPcOracleMode = i == 1;
        return;
    }
    return; // menu disabled for now: boot straight into Minish Cap
    if (renderer == NULL)
        return; // headless
    for (i = 0; i < NUM_GAMES; i++) {
        if (sGames[i].texture == NULL)
            sGames[i].texture = LoadArt(renderer, sGames[i].art);
    }
    SDL_SetRenderLogicalPresentation(renderer, MENU_W, MENU_H, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    for (;;) {
        uint16_t pressed;
        held = PcPollInput();
        pressed = held & ~prev;
        prev = held;

        if (showMessage) {
            if (pressed & (KEY_A | KEY_B | KEY_START))
                showMessage = 0;
        } else if (pressed & KEY_LEFT) {
            selected = (selected + NUM_GAMES - 1) % NUM_GAMES;
        } else if (pressed & KEY_RIGHT) {
            selected = (selected + 1) % NUM_GAMES;
        } else if (pressed & (KEY_A | KEY_START)) {
            if (sGames[selected].playable && (selected != 1 || PcOracleAvailable())) {
                gPcOracleMode = selected == 1; // Ages: the Lynna City proof of concept
                break;
            }
            snprintf(message, sizeof(message), "%s is not available yet.", sGames[selected].name);
            showMessage = 1;
        }
        DrawMenu(renderer, selected, showMessage ? message : NULL);
        SaveMenuShot(renderer);
        SDL_Delay(16);
    }

    PcRestoreGameView();
}
