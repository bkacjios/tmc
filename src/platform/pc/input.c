// Keyboard and gamepad input with SDL3, and the controls set in the settings
// menu (settings.c).
//
// Each GBA button has one keyboard key and one gamepad button (or trigger).
// The left stick always works as the D-pad. Fixed keys: Tab = fast forward,
// F11 or Alt+Enter = fullscreen, Esc = settings menu.
//
// The controls are saved in tmc.cfg (config.c), one line per button:
// "name=key,pad" with SDL's key and gamepad button names.

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"

#define STICK_DEADZONE 16000
#define TRIGGER_THRESHOLD 16000

// Same order as the GBA's KEYINPUT bits.
static const char* const sButtonIds[PC_NUM_BUTTONS] = {
    "a", "b", "select", "start", "right", "left", "up", "down", "r", "l",
};

static const PcBinding sDefaults[PC_NUM_BUTTONS] = {
    { SDL_SCANCODE_X, SDL_GAMEPAD_BUTTON_SOUTH },
    { SDL_SCANCODE_Z, SDL_GAMEPAD_BUTTON_EAST },
    { SDL_SCANCODE_BACKSPACE, SDL_GAMEPAD_BUTTON_BACK },
    { SDL_SCANCODE_RETURN, SDL_GAMEPAD_BUTTON_START },
    { SDL_SCANCODE_RIGHT, SDL_GAMEPAD_BUTTON_DPAD_RIGHT },
    { SDL_SCANCODE_LEFT, SDL_GAMEPAD_BUTTON_DPAD_LEFT },
    { SDL_SCANCODE_UP, SDL_GAMEPAD_BUTTON_DPAD_UP },
    { SDL_SCANCODE_DOWN, SDL_GAMEPAD_BUTTON_DPAD_DOWN },
    { SDL_SCANCODE_S, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER },
    { SDL_SCANCODE_A, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER },
};

PcBinding gPcBindings[PC_NUM_BUTTONS];
int gPcFastForward;
static SDL_Gamepad* sGamepad;
static int sMenuRequested;

static const char* PadId(int pad) {
    if (pad == PC_PAD_LEFT_TRIGGER)
        return SDL_GetGamepadStringForAxis(SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
    if (pad == PC_PAD_RIGHT_TRIGGER)
        return SDL_GetGamepadStringForAxis(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
    if (pad >= 0)
        return SDL_GetGamepadStringForButton((SDL_GamepadButton)pad);
    return NULL;
}

static int PadFromId(const char* id) {
    SDL_GamepadButton button;
    if (strcmp(id, SDL_GetGamepadStringForAxis(SDL_GAMEPAD_AXIS_LEFT_TRIGGER)) == 0)
        return PC_PAD_LEFT_TRIGGER;
    if (strcmp(id, SDL_GetGamepadStringForAxis(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)) == 0)
        return PC_PAD_RIGHT_TRIGGER;
    button = SDL_GetGamepadButtonFromString(id);
    return button == SDL_GAMEPAD_BUTTON_INVALID ? PC_PAD_NONE : button;
}

void PcWriteBindings(FILE* f) {
    int i;
    for (i = 0; i < PC_NUM_BUTTONS; i++) {
        const char* pad = PadId(gPcBindings[i].pad);
        fprintf(f, "%s=%s,%s\n", sButtonIds[i], SDL_GetScancodeName((SDL_Scancode)gPcBindings[i].key), pad ? pad : "");
    }
}

void PcDefaultBindings(void) {
    memcpy(gPcBindings, sDefaults, sizeof(gPcBindings));
}

void PcResetBindings(void) {
    PcDefaultBindings();
    PcSaveConfig();
}

// A line of tmc.cfg, split at '=': sets the binding if it is one.
void PcParseBinding(const char* name, char* value) {
    char* pad = strrchr(value, ','); // a key can be named ","
    int i;
    if (pad == NULL)
        return;
    *pad++ = '\0';
    for (i = 0; i < PC_NUM_BUTTONS; i++) {
        if (strcmp(name, sButtonIds[i]) == 0) {
            gPcBindings[i].key = SDL_GetScancodeFromName(value);
            gPcBindings[i].pad = PadFromId(pad);
        }
    }
}

// Display names, using the labels of the connected gamepad's face buttons.
const char* PcKeyName(int button) {
    const char* name = SDL_GetScancodeName((SDL_Scancode)gPcBindings[button].key);
    return name[0] != '\0' ? name : "-";
}

const char* PcPadName(int button) {
    static const char* const sLabels[] = {
        [SDL_GAMEPAD_BUTTON_LABEL_A] = "A",           [SDL_GAMEPAD_BUTTON_LABEL_B] = "B",
        [SDL_GAMEPAD_BUTTON_LABEL_X] = "X",           [SDL_GAMEPAD_BUTTON_LABEL_Y] = "Y",
        [SDL_GAMEPAD_BUTTON_LABEL_CROSS] = "Cross",   [SDL_GAMEPAD_BUTTON_LABEL_CIRCLE] = "Circle",
        [SDL_GAMEPAD_BUTTON_LABEL_SQUARE] = "Square", [SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE] = "Triangle",
    };
    static const char* const sNames[] = {
        [SDL_GAMEPAD_BUTTON_SOUTH] = "South",         [SDL_GAMEPAD_BUTTON_EAST] = "East",
        [SDL_GAMEPAD_BUTTON_WEST] = "West",           [SDL_GAMEPAD_BUTTON_NORTH] = "North",
        [SDL_GAMEPAD_BUTTON_BACK] = "Back",           [SDL_GAMEPAD_BUTTON_GUIDE] = "Guide",
        [SDL_GAMEPAD_BUTTON_START] = "Start",         [SDL_GAMEPAD_BUTTON_LEFT_STICK] = "L Stick",
        [SDL_GAMEPAD_BUTTON_RIGHT_STICK] = "R Stick", [SDL_GAMEPAD_BUTTON_LEFT_SHOULDER] = "LB",
        [SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER] = "RB",   [SDL_GAMEPAD_BUTTON_DPAD_UP] = "D-Pad Up",
        [SDL_GAMEPAD_BUTTON_DPAD_DOWN] = "D-Pad Down", [SDL_GAMEPAD_BUTTON_DPAD_LEFT] = "D-Pad Left",
        [SDL_GAMEPAD_BUTTON_DPAD_RIGHT] = "D-Pad Right",
    };
    int pad = gPcBindings[button].pad;

    if (pad == PC_PAD_LEFT_TRIGGER)
        return "LT";
    if (pad == PC_PAD_RIGHT_TRIGGER)
        return "RT";
    if (pad < 0)
        return "-";
    if (sGamepad != NULL) {
        SDL_GamepadButtonLabel label = SDL_GetGamepadButtonLabel(sGamepad, (SDL_GamepadButton)pad);
        if (label != SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN && label < SDL_arraysize(sLabels))
            return sLabels[label];
    }
    if (pad < (int)SDL_arraysize(sNames) && sNames[pad] != NULL)
        return sNames[pad];
    return "Other";
}

static int PadPressed(int pad) {
    if (sGamepad == NULL || pad < 0)
        return 0;
    if (pad == PC_PAD_LEFT_TRIGGER)
        return SDL_GetGamepadAxis(sGamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) > TRIGGER_THRESHOLD;
    if (pad == PC_PAD_RIGHT_TRIGGER)
        return SDL_GetGamepadAxis(sGamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) > TRIGGER_THRESHOLD;
    return SDL_GetGamepadButton(sGamepad, (SDL_GamepadButton)pad);
}

// Enter held with Alt is the fullscreen key, not a button.
static int KeyHeld(const bool* k, int key) {
    return k[key] && !(key == SDL_SCANCODE_RETURN && (SDL_GetModState() & SDL_KMOD_ALT));
}

static uint16_t ReadKeys(void) {
    const bool* k = SDL_GetKeyboardState(NULL);
    uint16_t keys = 0;
    int i;

    for (i = 0; i < PC_NUM_BUTTONS; i++) {
        if (KeyHeld(k, gPcBindings[i].key) || PadPressed(gPcBindings[i].pad))
            keys |= 1 << i;
    }
    if (sGamepad != NULL) {
        int lx = SDL_GetGamepadAxis(sGamepad, SDL_GAMEPAD_AXIS_LEFTX);
        int ly = SDL_GetGamepadAxis(sGamepad, SDL_GAMEPAD_AXIS_LEFTY);
        if (lx > STICK_DEADZONE)
            keys |= PC_KEY_RIGHT;
        if (lx < -STICK_DEADZONE)
            keys |= PC_KEY_LEFT;
        if (ly < -STICK_DEADZONE)
            keys |= PC_KEY_UP;
        if (ly > STICK_DEADZONE)
            keys |= PC_KEY_DOWN;
    }
    gPcFastForward = k[SDL_SCANCODE_TAB];
    return keys;
}

// A key or pad button another GBA button has, pressed once while rebinding.
// Pressing it again swaps the two buttons' bindings.
static int sPendingKey = SDL_SCANCODE_UNKNOWN;
static int sPendingPad = PC_PAD_NONE;

// Binds a key or pad button to a GBA button. One another button has needs a
// second press, then the other button gets the old binding. Fixed keys (Tab,
// F11) can't be bound. Returns -1 once bound, else the button that has it,
// or PC_NUM_BUTTONS for a fixed key.
static int Bind(int button, int key, int pad) {
    int i, other = -1;

    if (key == SDL_SCANCODE_TAB || key == SDL_SCANCODE_F11)
        return PC_NUM_BUTTONS;
    for (i = 0; i < PC_NUM_BUTTONS; i++) {
        if (i != button && ((key != SDL_SCANCODE_UNKNOWN && gPcBindings[i].key == key) ||
                            (pad != PC_PAD_NONE && gPcBindings[i].pad == pad)))
            other = i;
    }
    if (other >= 0) {
        if ((key != SDL_SCANCODE_UNKNOWN && key != sPendingKey) || (pad != PC_PAD_NONE && pad != sPendingPad)) {
            sPendingKey = key;
            sPendingPad = pad;
            return other;
        }
        if (key != SDL_SCANCODE_UNKNOWN)
            gPcBindings[other].key = gPcBindings[button].key;
        if (pad != PC_PAD_NONE)
            gPcBindings[other].pad = gPcBindings[button].pad;
    }
    if (key != SDL_SCANCODE_UNKNOWN)
        gPcBindings[button].key = key;
    if (pad != PC_PAD_NONE)
        gPcBindings[button].pad = pad;
    sPendingKey = SDL_SCANCODE_UNKNOWN;
    sPendingPad = PC_PAD_NONE;
    PcSaveConfig();
    return -1;
}

// Handles window events. With rebind >= 0, the next key or pad button pressed
// is bound to that GBA button: returns 1 then, or -1 if Esc cancelled it. A
// press that can't be bound sets *conflict (see Bind) and keeps waiting.
static int PumpEvents(int rebind, int* conflict) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_QUIT:
                exit(0);
            case SDL_EVENT_KEY_DOWN:
                if (e.key.repeat)
                    break;
                if (e.key.scancode == SDL_SCANCODE_ESCAPE) {
                    if (rebind >= 0) {
                        sPendingKey = SDL_SCANCODE_UNKNOWN;
                        sPendingPad = PC_PAD_NONE;
                        return -1;
                    }
                    sMenuRequested = 1;
                } else if (e.key.scancode == SDL_SCANCODE_F11 ||
                           (e.key.scancode == SDL_SCANCODE_RETURN && (e.key.mod & SDL_KMOD_ALT))) {
                    gPcOptions.fullscreen = !gPcOptions.fullscreen;
                    PcApplyVideoOptions();
                    PcSaveConfig();
                } else if (rebind >= 0) {
                    if ((*conflict = Bind(rebind, e.key.scancode, PC_PAD_NONE)) < 0)
                        return 1;
                }
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                if (rebind >= 0 && sGamepad != NULL && e.gbutton.which == SDL_GetGamepadID(sGamepad)) {
                    if ((*conflict = Bind(rebind, SDL_SCANCODE_UNKNOWN, e.gbutton.button)) < 0)
                        return 1;
                }
                break;
            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                if (rebind >= 0 && sGamepad != NULL && e.gaxis.which == SDL_GetGamepadID(sGamepad) &&
                    e.gaxis.value > TRIGGER_THRESHOLD) {
                    if (e.gaxis.axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER) {
                        if ((*conflict = Bind(rebind, SDL_SCANCODE_UNKNOWN, PC_PAD_LEFT_TRIGGER)) < 0)
                            return 1;
                    }
                    if (e.gaxis.axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) {
                        if ((*conflict = Bind(rebind, SDL_SCANCODE_UNKNOWN, PC_PAD_RIGHT_TRIGGER)) < 0)
                            return 1;
                    }
                }
                break;
            case SDL_EVENT_GAMEPAD_ADDED:
                if (sGamepad == NULL)
                    sGamepad = SDL_OpenGamepad(e.gdevice.which);
                break;
            case SDL_EVENT_GAMEPAD_REMOVED:
                if (sGamepad != NULL && SDL_GetGamepadID(sGamepad) == e.gdevice.which) {
                    SDL_CloseGamepad(sGamepad);
                    sGamepad = NULL;
                }
                break;
        }
    }
    return 0;
}

// Handles window events and returns the pressed GBA buttons.
uint16_t PcPollInput(void) {
    int conflict;
    PumpEvents(-1, &conflict);
    return ReadKeys();
}

// Waits for a key or pad button for a GBA button: returns 1 once bound, -1 if
// Esc cancelled it, 0 while waiting. A press that's taken sets *conflict to
// the button that has it, or PC_NUM_BUTTONS for a fixed key.
int PcPollRebind(int button, int* conflict) {
    return PumpEvents(button, conflict);
}

// Whether Esc was pressed since the last call.
int PcTakeMenuRequest(void) {
    int requested = sMenuRequested;
    sMenuRequested = 0;
    return requested;
}

// Buttons for moving around menus: the bound ones, plus arrows, Enter (A),
// and the gamepad's D-pad, south (A) and east (B) buttons, so a bad binding
// can't lock anyone out.
uint16_t PcPollMenuKeys(void) {
    const bool* k = SDL_GetKeyboardState(NULL);
    uint16_t keys = PcPollInput();

    if (k[SDL_SCANCODE_UP])
        keys |= PC_KEY_UP;
    if (k[SDL_SCANCODE_DOWN])
        keys |= PC_KEY_DOWN;
    if (k[SDL_SCANCODE_LEFT])
        keys |= PC_KEY_LEFT;
    if (k[SDL_SCANCODE_RIGHT])
        keys |= PC_KEY_RIGHT;
    if (KeyHeld(k, SDL_SCANCODE_RETURN))
        keys |= PC_KEY_A;
    if (sGamepad != NULL) {
        if (SDL_GetGamepadButton(sGamepad, SDL_GAMEPAD_BUTTON_DPAD_UP))
            keys |= PC_KEY_UP;
        if (SDL_GetGamepadButton(sGamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN))
            keys |= PC_KEY_DOWN;
        if (SDL_GetGamepadButton(sGamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT))
            keys |= PC_KEY_LEFT;
        if (SDL_GetGamepadButton(sGamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT))
            keys |= PC_KEY_RIGHT;
        if (SDL_GetGamepadButton(sGamepad, SDL_GAMEPAD_BUTTON_SOUTH))
            keys |= PC_KEY_A;
        if (SDL_GetGamepadButton(sGamepad, SDL_GAMEPAD_BUTTON_EAST))
            keys |= PC_KEY_B;
    }
    return keys;
}
