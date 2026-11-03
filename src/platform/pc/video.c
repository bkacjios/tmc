// Window and frame pacing with SDL3. Input is in input.c.

#include <SDL3/SDL.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "pc.h"

#define WIDTH 240
#define HEIGHT 160
#define SCALE 3
// The GBA refreshes at 16.78 MHz / 280896 cycles per frame.
#define FRAME_NS 16742706ull

extern uint32_t gPcFramebuffer[HEIGHT][WIDTH];

#ifdef PC_S30
// The S30's panel is portrait (320x480), mounted landscape. Everything is drawn
// to a landscape view at twice the GBA's size, which is turned onto the panel
// when it is presented.
#define VIEW_W (WIDTH * 2)
#define VIEW_H (HEIGHT * 2)
static SDL_Texture* sView;
#endif

static SDL_Window* sWindow;
static SDL_Renderer* sRenderer;
static SDL_Texture* sTexture;
static uint64_t sNextFrame;

static void ShowError(const char* fmt, va_list args) {
    char msg[4352];
    vsnprintf(msg, sizeof(msg), fmt, args);
    fprintf(stderr, "tmc: %s\n", msg);
    // SDL allows this before SDL_Init. Headless test runs skip it: the box
    // would wait for a click.
    if (getenv("TMC_HEADLESS") == NULL)
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "The Minish Cap", msg, sWindow);
}

void PcError(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ShowError(fmt, args);
    va_end(args);
}

void PcFatal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ShowError(fmt, args);
    va_end(args);
    exit(1);
}

void PcVideoInit(void) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
        PcFatal("SDL_Init failed: %s", SDL_GetError());
#ifdef PC_S30
    SDL_HideCursor();
    sWindow = SDL_CreateWindow("The Minish Cap", 0, 0, SDL_WINDOW_FULLSCREEN);
#else
    sWindow = SDL_CreateWindow("The Minish Cap", WIDTH * SCALE, HEIGHT * SCALE, SDL_WINDOW_RESIZABLE);
#endif
    if (sWindow == NULL)
        PcFatal("cannot create window: %s", SDL_GetError());
    SDL_SetWindowMinimumSize(sWindow, WIDTH, HEIGHT); // no smaller than the GBA screen
    sRenderer = SDL_CreateRenderer(sWindow, NULL);
    if (sRenderer == NULL)
        PcFatal("cannot create renderer: %s", SDL_GetError());
#ifdef PC_S30
    sView = SDL_CreateTexture(sRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, VIEW_W, VIEW_H);
    if (sView == NULL || !SDL_SetRenderTarget(sRenderer, sView))
        PcFatal("cannot create the view: %s", SDL_GetError());
    SDL_SetTextureScaleMode(sView, SDL_SCALEMODE_NEAREST);
#endif
    sTexture = SDL_CreateTexture(sRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    PcApplyVideoOptions();
    sNextFrame = SDL_GetTicksNS();
}

static void SetGameView(void) {
    SDL_SetRenderLogicalPresentation(sRenderer, WIDTH, HEIGHT,
                                     gPcOptions.integerScale ? SDL_LOGICAL_PRESENTATION_INTEGER_SCALE
                                                             : SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void PcApplyVideoOptions(void) {
    if (sWindow == NULL)
        return;
#ifndef PC_S30 // always fullscreen
    SDL_SetWindowFullscreen(sWindow, gPcOptions.fullscreen != 0);
#endif
    SDL_SetTextureScaleMode(sTexture, gPcOptions.smooth ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
    SetGameView();
}

void PcPresent(void) {
    PcPresentPixels(&gPcFramebuffer[0][0]);
}

// Shows a 240x160 image (the game's frame or the settings menu) and waits for
// the next frame.
void PcPresentPixels(const uint32_t* pixels) {
    uint64_t now;

    SDL_UpdateTexture(sTexture, NULL, pixels, WIDTH * 4);
    SDL_SetRenderDrawColor(sRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sRenderer);
    SDL_RenderTexture(sRenderer, sTexture, NULL, NULL);
    PcRenderPresent();

    // Pace to the GBA frame rate.
    sNextFrame += FRAME_NS;
    now = SDL_GetTicksNS();
    if (gPcFastForward || now > sNextFrame + 4 * FRAME_NS) {
        sNextFrame = now; // fell behind (or fast forward): don't try to catch up
    } else if (now < sNextFrame) {
        SDL_DelayPrecise(sNextFrame - now);
    }
}

// Shows what was drawn with the renderer.
void PcRenderPresent(void) {
#ifdef PC_S30
    int w, h;
    SDL_FRect dst;

    SDL_SetRenderTarget(sRenderer, NULL);
    SDL_GetCurrentRenderOutputSize(sRenderer, &w, &h);
    dst.x = (w - VIEW_W) / 2.0f;
    dst.y = (h - VIEW_H) / 2.0f;
    dst.w = VIEW_W;
    dst.h = VIEW_H;
    SDL_SetRenderDrawColor(sRenderer, 0, 0, 0, 255);
    SDL_RenderClear(sRenderer);
    // A quarter turn counterclockwise: the view's top edge along the panel's
    // left edge (the panel's top is the device's right side).
    SDL_RenderTextureRotated(sRenderer, sView, NULL, &dst, -90.0, NULL, SDL_FLIP_NONE);
    SDL_RenderPresent(sRenderer);
    SDL_SetRenderTarget(sRenderer, sView);
#else
    SDL_RenderPresent(sRenderer);
#endif
}

SDL_Renderer* PcGetRenderer(void) {
    return sRenderer;
}

// Back to the game's 240x160 view after another screen (the game select menu).
void PcRestoreGameView(void) {
    SetGameView();
    sNextFrame = SDL_GetTicksNS();
}

// Writes the current frame to a BMP file (used for testing).
int PcSaveScreenshot(const char* path) {
    return PcSavePixels(path, &gPcFramebuffer[0][0]);
}

int PcSavePixels(const char* path, const uint32_t* pixels) {
    SDL_Surface* surface = SDL_CreateSurfaceFrom(WIDTH, HEIGHT, SDL_PIXELFORMAT_ARGB8888, (void*)pixels, WIDTH * 4);
    int ok = surface != NULL && SDL_SaveBMP(surface, path);
    SDL_DestroySurface(surface);
    return ok;
}
