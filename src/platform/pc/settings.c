// Settings menu: controls (keyboard key and gamepad button for each GBA
// button), the save file's own options when there is one (message speed, and
// brightness shown as Gamma: it picks the color curve palettes are converted
// with), and the PC's system options (volume, display; saved in tmc.cfg, see
// config.c).
//
// It is opened with Esc, from the OPTIONS button of the pause menu's Quest
// Status screen (in place of SLEEP), and from the file select's options (R).
// It runs its own loop over the frozen game frame, like the game select menu,
// and is drawn at the GBA's 240x160 with the game's font and button icons.
//
// TMC_SETTINGS_SHOT=prefix saves each page as prefix<n>.bmp when the menu
// opens headless (for testing).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pc.h"
#include "global.h"
#include "fade.h"
#include "main.h"
#include "save.h"

#define WIDTH 240
#define HEIGHT 160

extern uint32_t gPcFramebuffer[HEIGHT][WIDTH];
extern u32* gUnk_08109248[]; // the font's glyph sets (see text.c)

// Colors of the pause menu's SAVE button.
#define COLOR_OUTLINE 0xFF52636B
#define COLOR_BLUE 0xFF29A5F7
#define COLOR_LIGHT_BLUE 0xFF4AD6FF
#define COLOR_PALE_BLUE 0xFFADEFFF
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_TEXT 0xFF31394A
#define COLOR_FADED 0xFF8C9CA5
#define COLOR_ERROR 0xFFD63131

#define FONT_SET_TEXT 0
#define FONT_SET_ICONS 7 // 16x16: A, B, L, R, D-pad up, down, left, right
enum { ICON_A, ICON_B, ICON_L, ICON_R, ICON_UP, ICON_DOWN, ICON_LEFT, ICON_RIGHT, ICON_NONE };

#define PANEL_X 8
#define PANEL_Y 4
#define PANEL_W 224
#define PANEL_H 152
#define LIST_Y 40
#define ROW_H 16
#define VISIBLE_ROWS 6
#define COLUMN_KEY 76
#define COLUMN_PAD 156
#define TAB_PADDING 6

// Pages, in tab order. The game page (the save file's options) is only there
// for a save file: in a game, or for the file chosen in the file select.
enum { PAGE_CONTROLS, PAGE_GAME, PAGE_SYSTEM, NUM_PAGES };

// Rows of the game page (message speed, gamma) and of the system page (the
// rest), as one list.
enum { OPT_MSG_SPEED, OPT_GAMMA, OPT_VOLUME, OPT_FULLSCREEN, OPT_FILTERING, OPT_SCALING, OPT_QUIT, NUM_OPTION_ROWS };

// Controls page rows, in the order shown; the last row resets them.
static const struct {
    int button;
    int icon;
    const char* name;
} sControlRows[] = {
    { 6, ICON_UP, "Up" },        { 7, ICON_DOWN, "Down" },    { 5, ICON_LEFT, "Left" },
    { 4, ICON_RIGHT, "Right" },  { 0, ICON_A, "A" },          { 1, ICON_B, "B" },
    { 9, ICON_L, "L" },          { 8, ICON_R, "R" },          { 3, ICON_NONE, "Start" },
    { 2, ICON_NONE, "Select" },
};
#define NUM_CONTROL_ROWS ((int)(sizeof(sControlRows) / sizeof(sControlRows[0])))

static const char* const sMsgSpeeds[MAX_MSG_SPEED] = { "Slow", "Normal", "Fast", "Instant" };
static const char* const sBrightnesses[MAX_BRIGHTNESS] = { "Dark", "Normal", "Bright" };
static const char* const sOnOff[] = { "Off", "On" };
static const char* const sFilterings[] = { "Sharp", "Smooth" };
static const char* const sScalings[] = { "Fit", "Integer" };

static uint32_t sScreen[HEIGHT][WIDTH];

typedef struct {
    int page;
    int row;
    int scroll;
    int rebinding; // waiting for a key or pad button
    int conflict;  // the button that has the last key pressed (see PcPollRebind), or -1
    u8* msgSpeed;  // the save file's options, or NULL outside a game
    u8* brightness;
} Menu;

static int NumPages(const Menu* m) {
    return m->msgSpeed != NULL ? NUM_PAGES : NUM_PAGES - 1;
}

// The page shown at a tab.
static int PageAt(const Menu* m, int tab) {
    return m->msgSpeed == NULL && tab >= PAGE_GAME ? tab + 1 : tab;
}

static void FillRect(int x, int y, int w, int h, uint32_t color) {
    int i, j;
    for (j = y; j < y + h; j++)
        for (i = x; i < x + w; i++)
            sScreen[j][i] = color;
}

// A box like the pause menu's buttons: outline, pale inner border, filled.
static void DrawBox(int x, int y, int w, int h, uint32_t fill, uint32_t border) {
    FillRect(x + 1, y, w - 2, h, COLOR_OUTLINE);
    FillRect(x, y + 1, w, h - 2, COLOR_OUTLINE);
    FillRect(x + 1, y + 1, w - 2, h - 2, border);
    FillRect(x + 2, y + 2, w - 4, h - 4, fill);
}

// Unpacks a glyph (8x16, 4bpp) of a font set: 0 is background, 0xF past the
// glyph's width, other values are ink.
static const u8* Glyph(int set, int index) {
    return (const u8*)gUnk_08109248[set] + index * 0x40;
}

static int Pixel(const u8* glyph, int x, int y) {
    u8 b = glyph[y * 4 + x / 2];
    return x & 1 ? b >> 4 : b & 0xF;
}

// Draws text in the game's font (ASCII, and \x07-\x0A for arrows up, down,
// left, right). Returns the width.
static int DrawText(int x, int y, const char* text, uint32_t color) {
    int start = x;
    for (; *text != '\0'; text++) {
        const u8* glyph = Glyph(FONT_SET_TEXT, (u8)*text);
        int first = 0, last, i, j;
        // Like the game, the glyph's columns are the ones not 0xF in its first row.
        while (first < 8 && Pixel(glyph, first, 0) == 0xF)
            first++;
        for (last = first; last < 8 && Pixel(glyph, last, 0) != 0xF; last++) {}
        for (j = 0; j < 16; j++) {
            for (i = first; i < last; i++) {
                int v = Pixel(glyph, i, j);
                int px = x + i - first, py = y + j;
                if (v != 0 && v != 0xF && px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT)
                    sScreen[py][px] = color;
            }
        }
        x += last - first;
    }
    return x - start;
}

static int TextWidth(const char* text) {
    int width = 0;
    for (; *text != '\0'; text++) {
        const u8* glyph = Glyph(FONT_SET_TEXT, (u8)*text);
        int first = 0, last;
        while (first < 8 && Pixel(glyph, first, 0) == 0xF)
            first++;
        for (last = first; last < 8 && Pixel(glyph, last, 0) != 0xF; last++) {}
        width += last - first;
    }
    return width;
}

// Draws one of the button icons the game shows in its messages, in grey like
// the file select's OPTIONS tag. Their 0xF pixels are both the letter (or the
// D-pad's direction) and a drop shadow; the shadow, next to empty pixels, is
// left out.
static void DrawIcon(int x, int y, int icon) {
    static const uint32_t sColors[16] = {
        [0xB] = 0xFFB5BDC6,
        [0xC] = 0xFF7B848C,
        [0xE] = 0xFF5A636B,
        [0xF] = COLOR_WHITE,
    };
    u8 pixels[16][16];
    int half, i, j;

    for (half = 0; half < 2; half++) {
        const u8* glyph = Glyph(FONT_SET_ICONS, icon * 2 + half);
        for (j = 0; j < 16; j++)
            for (i = 0; i < 8; i++)
                pixels[j][half * 8 + i] = Pixel(glyph, i, j);
    }
    for (j = 0; j < 16; j++) {
        for (i = 0; i < 16; i++) {
            int v = pixels[j][i];
            if (v == 0xF && (i == 0 || j == 0 || i == 15 || j == 15 || pixels[j][i - 1] == 0 ||
                             pixels[j][i + 1] == 0 || pixels[j - 1][i] == 0 || pixels[j + 1][i] == 0))
                continue;
            if (sColors[v] != 0)
                sScreen[y + j][x + i] = sColors[v];
        }
    }
}

static int NumRows(const Menu* m) {
    switch (PageAt(m, m->page)) {
        case PAGE_CONTROLS:
            return NUM_CONTROL_ROWS + 1;
        case PAGE_GAME:
            return OPT_VOLUME;
        default:
            return NUM_OPTION_ROWS - OPT_VOLUME;
    }
}

static void DrawRowBox(int y, int selected) {
    if (selected)
        DrawBox(PANEL_X + 6, y, PANEL_W - 12, ROW_H, COLOR_PALE_BLUE, COLOR_BLUE);
}

static const char* ButtonName(int button) {
    int i;
    for (i = 0; i < NUM_CONTROL_ROWS; i++)
        if (sControlRows[i].button == button)
            return sControlRows[i].name;
    return "";
}

static void DrawControls(const Menu* m) {
    int i;

    DrawText(COLUMN_KEY, LIST_Y - 14, "Keyboard", COLOR_BLUE);
    DrawText(COLUMN_PAD, LIST_Y - 14, "Controller", COLOR_BLUE);
    for (i = m->scroll; i < m->scroll + VISIBLE_ROWS && i < NUM_CONTROL_ROWS + 1; i++) {
        int y = LIST_Y + (i - m->scroll) * ROW_H;
        int selected = i == m->row;
        DrawRowBox(y, selected);
        if (i == NUM_CONTROL_ROWS) {
            const char* text = "Reset to defaults";
            DrawText(PANEL_X + (PANEL_W - TextWidth(text)) / 2, y, text, COLOR_TEXT);
            continue;
        }
        if (sControlRows[i].icon != ICON_NONE)
            DrawIcon(PANEL_X + 10, y, sControlRows[i].icon);
        DrawText(PANEL_X + 28, y, sControlRows[i].name, COLOR_TEXT);
        if (selected && m->rebinding) {
            char text[48];
            if (m->conflict == PC_NUM_BUTTONS)
                snprintf(text, sizeof(text), "That key is reserved");
            else if (m->conflict >= 0)
                snprintf(text, sizeof(text), "Used by %s - again to swap", ButtonName(m->conflict));
            else
                snprintf(text, sizeof(text), "Press a key or button");
            DrawText(COLUMN_KEY, y, text, m->conflict >= 0 ? COLOR_ERROR : COLOR_BLUE);
        } else {
            DrawText(COLUMN_KEY, y, PcKeyName(sControlRows[i].button), COLOR_TEXT);
            DrawText(COLUMN_PAD, y, PcPadName(sControlRows[i].button), COLOR_TEXT);
        }
    }
    // Scroll marks.
    if (m->scroll > 0)
        DrawText(PANEL_X + PANEL_W - 14, LIST_Y - 14, "\x07", COLOR_BLUE);
    if (m->scroll + VISIBLE_ROWS < NUM_CONTROL_ROWS + 1)
        DrawText(PANEL_X + PANEL_W - 14, LIST_Y + VISIBLE_ROWS * ROW_H - 2, "\x08", COLOR_BLUE);
}

// A row whose value Left/Right change: its name, and the value's arrows.
// Sets *left and *right to the free space between the arrows.
static void DrawValueRow(int y, const char* name, int selected, int* left, int* right) {
    int x = PANEL_X + 140, aw = TextWidth("\x09");
    DrawRowBox(y, selected);
    DrawText(PANEL_X + 14, y, name, COLOR_TEXT);
    DrawText(x - 4, y, "\x09", selected ? COLOR_BLUE : COLOR_FADED);
    DrawText(x + 56, y, "\x0A", selected ? COLOR_BLUE : COLOR_FADED);
    *left = x - 4 + aw + 2;
    *right = x + 56 - 2;
}

// "< value >", centered between the arrows.
static void DrawChoice(int y, const char* name, const char* value, int selected) {
    int left, right;
    DrawValueRow(y, name, selected, &left, &right);
    DrawText((left + right + 1 - TextWidth(value)) / 2, y, value, COLOR_TEXT);
}

// The option shown at a row of the game or system page.
static int OptionAt(const Menu* m, int row) {
    return PageAt(m, m->page) == PAGE_GAME ? row : row + OPT_VOLUME;
}

// A level from 0 to max as a bar of segments, centered between the arrows:
// "< ||||||.... >".
static void DrawSlider(int y, const char* name, int value, int max, int selected) {
    int left, right, seg, width, start, i;
    DrawValueRow(y, name, selected, &left, &right);
    seg = (right - left + 1) / max;
    width = seg * max - 1;
    start = left + (right - left + 1 - width) / 2;
    for (i = 0; i < max; i++)
        FillRect(start + i * seg, y + 4, seg - 1, 8, i < value ? COLOR_BLUE : COLOR_FADED);
}

static void DrawOptions(const Menu* m) {
    int i, rows = NumRows(m);
    for (i = m->scroll; i < m->scroll + VISIBLE_ROWS && i < rows; i++) {
        int y = LIST_Y - 8 + (i - m->scroll) * ROW_H, selected = i == m->row;
        switch (OptionAt(m, i)) {
            case OPT_MSG_SPEED:
                DrawChoice(y, "Message Speed", sMsgSpeeds[*m->msgSpeed], selected);
                break;
            case OPT_GAMMA:
                DrawChoice(y, "Gamma", sBrightnesses[*m->brightness], selected);
                break;
            case OPT_VOLUME:
                DrawSlider(y, "Volume", gPcOptions.volume, PC_MAX_VOLUME, selected);
                break;
            case OPT_FULLSCREEN:
                DrawChoice(y, "Fullscreen", sOnOff[gPcOptions.fullscreen], selected);
                break;
            case OPT_FILTERING:
                DrawChoice(y, "Filtering", sFilterings[gPcOptions.smooth], selected);
                break;
            case OPT_SCALING:
                DrawChoice(y, "Scaling", sScalings[gPcOptions.integerScale], selected);
                break;
            default: {
                const char* text = "Quit Game";
                DrawRowBox(y, selected);
                DrawText(PANEL_X + (PANEL_W - TextWidth(text)) / 2, y, text, COLOR_TEXT);
                break;
            }
        }
    }
    // Scroll marks.
    if (m->scroll > 0)
        DrawText(PANEL_X + PANEL_W - 14, LIST_Y - 22, "\x07", COLOR_BLUE);
    if (m->scroll + VISIBLE_ROWS < rows)
        DrawText(PANEL_X + PANEL_W - 14, LIST_Y - 8 + VISIBLE_ROWS * ROW_H - 2, "\x08", COLOR_BLUE);
}

static void Draw(const Menu* m) {
    static const char* const sPages[NUM_PAGES] = { "CONTROLS", "GAME", "SYSTEM" };
    int x, y, i;

    // The game's frame, darkened.
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            uint32_t c = gPcFramebuffer[y][x];
            sScreen[y][x] = 0xFF000000 | ((c >> 2) & 0x3F3F3F);
        }
    }
    DrawBox(PANEL_X, PANEL_Y, PANEL_W, PANEL_H, COLOR_WHITE, COLOR_PALE_BLUE);

    // Page tabs between the L and R icons, like the pause menu.
    DrawIcon(PANEL_X + 6, PANEL_Y + 4, ICON_L);
    DrawIcon(PANEL_X + PANEL_W - 22, PANEL_Y + 4, ICON_R);
    // Each tab as wide as its name plus padding, all of them centered.
    for (x = 0, i = 0; i < NumPages(m); i++)
        x += TextWidth(sPages[PageAt(m, i)]) + TAB_PADDING * 2;
    x = PANEL_X + (PANEL_W - x) / 2;
    for (i = 0; i < NumPages(m); i++) {
        const char* name = sPages[PageAt(m, i)];
        int w = TextWidth(name) + TAB_PADDING * 2;
        if (i == m->page)
            DrawBox(x, PANEL_Y + 4, w, 16, COLOR_BLUE, COLOR_LIGHT_BLUE);
        DrawText(x + TAB_PADDING, PANEL_Y + 4, name, i == m->page ? COLOR_WHITE : COLOR_FADED);
        x += w;
    }

    if (PageAt(m, m->page) == PAGE_CONTROLS)
        DrawControls(m);
    else
        DrawOptions(m);

    // Button hints.
    y = PANEL_Y + PANEL_H - 19;
    if (m->rebinding) {
        DrawText(PANEL_X + 14, y, "Esc: cancel", COLOR_FADED);
    } else {
        x = PANEL_X + 14;
        DrawIcon(x, y, ICON_A);
        x += 18 + DrawText(x + 18, y, "Select", COLOR_FADED) + 10;
        DrawIcon(x, y, ICON_B);
        DrawText(x + 18, y, "Back", COLOR_FADED);
    }
}

// The value of an options page row other than Quit Game, and its maximum.
static int OptionValue(const Menu* m, int option, int* max) {
    *max = 1;
    switch (option) {
        case OPT_MSG_SPEED:
            *max = MAX_MSG_SPEED - 1;
            return *m->msgSpeed;
        case OPT_GAMMA:
            *max = MAX_BRIGHTNESS - 1;
            return *m->brightness;
        case OPT_VOLUME:
            *max = PC_MAX_VOLUME;
            return gPcOptions.volume;
        case OPT_FULLSCREEN:
            return gPcOptions.fullscreen;
        case OPT_FILTERING:
            return gPcOptions.smooth;
        default:
            return gPcOptions.integerScale;
    }
}

// Sets an option; the system ones apply and are saved right away. The save
// file's ones apply when the menu closes (PcSettingsMenu).
static void SetOption(Menu* m, int option, int value) {
    switch (option) {
        case OPT_MSG_SPEED:
            *m->msgSpeed = value;
            return;
        case OPT_GAMMA:
            *m->brightness = value;
            return;
        case OPT_VOLUME:
            gPcOptions.volume = value;
            PcApplyVolume();
            break;
        case OPT_FULLSCREEN:
            gPcOptions.fullscreen = value;
            PcApplyVideoOptions();
            break;
        case OPT_FILTERING:
            gPcOptions.smooth = value;
            PcApplyVideoOptions();
            break;
        default:
            gPcOptions.integerScale = value;
            PcApplyVideoOptions();
            break;
    }
    PcSaveConfig();
}

// Left/Right on the game and system pages.
static void ChangeOption(Menu* m, int delta) {
    int option, value, max;
    if (PageAt(m, m->page) == PAGE_CONTROLS || (option = OptionAt(m, m->row)) == OPT_QUIT)
        return;
    value = OptionValue(m, option, &max) + delta;
    if (value >= 0 && value <= max)
        SetOption(m, option, value);
}

static void Select(Menu* m) {
    int option, max;
    if (PageAt(m, m->page) == PAGE_CONTROLS) {
        if (m->row == NUM_CONTROL_ROWS)
            PcResetBindings();
        else {
            m->rebinding = 1;
            m->conflict = -1;
        }
        return;
    }
    option = OptionAt(m, m->row);
    if (option == OPT_QUIT)
        exit(0);
    if (OptionValue(m, option, &max) >= 0 && max == 1) // toggles
        SetOption(m, option, !OptionValue(m, option, &max));
}

static void Run(Menu* m) {
    uint16_t held, prev = 0xFFFF; // ignore buttons still held from opening the menu
    int repeat = 0;

    for (;;) {
        uint16_t pressed;

        if (m->rebinding) {
            if (PcPollRebind(sControlRows[m->row].button, &m->conflict) != 0) {
                m->rebinding = 0;
                prev = 0xFFFF; // the new binding may be held
            }
            Draw(m);
            PcPresentPixels(&sScreen[0][0]);
            continue;
        }

        held = PcPollMenuKeys();
        pressed = held & ~prev;
        // Auto-repeat for the D-pad.
        if (held & prev & (PC_KEY_UP | PC_KEY_DOWN | PC_KEY_LEFT | PC_KEY_RIGHT)) {
            if (++repeat >= 20 && (repeat - 20) % 5 == 0)
                pressed |= held & (PC_KEY_UP | PC_KEY_DOWN | PC_KEY_LEFT | PC_KEY_RIGHT);
        } else {
            repeat = 0;
        }
        prev = held;

        if (PcTakeMenuRequest() || (pressed & (PC_KEY_B | PC_KEY_START)))
            break;
        if (pressed & (PC_KEY_L | PC_KEY_R)) {
            m->page = (m->page + (pressed & PC_KEY_L ? NumPages(m) - 1 : 1)) % NumPages(m);
            m->row = m->scroll = 0;
        } else if (pressed & PC_KEY_UP) {
            m->row = (m->row + NumRows(m) - 1) % NumRows(m);
        } else if (pressed & PC_KEY_DOWN) {
            m->row = (m->row + 1) % NumRows(m);
        } else if (pressed & PC_KEY_LEFT) {
            ChangeOption(m, -1);
        } else if (pressed & PC_KEY_RIGHT) {
            ChangeOption(m, 1);
        } else if (pressed & PC_KEY_A) {
            Select(m);
        }
        if (m->row < m->scroll)
            m->scroll = m->row;
        if (m->row >= m->scroll + VISIBLE_ROWS)
            m->scroll = m->row - VISIBLE_ROWS + 1;

        Draw(m);
        PcPresentPixels(&sScreen[0][0]);
    }

    // Don't let the button that closed the menu reach the game.
    while (PcPollMenuKeys() != 0) {
        Draw(m);
        PcPresentPixels(&sScreen[0][0]);
    }
}

static void SaveShots(Menu* m, const char* prefix) {
    char path[256];
    for (m->page = 0; m->page < NumPages(m); m->page++) {
        snprintf(path, sizeof(path), "%s%d.bmp", prefix, m->page);
        Draw(m);
        if (!PcSavePixels(path, &sScreen[0][0]))
            fprintf(stderr, "tmc: cannot write %s\n", path);
    }
}

// Opens the settings menu. msgSpeed and brightness are the options of the
// current save file (NULL outside a game); changes apply right away.
void PcSettingsMenu(uint8_t* msgSpeed, uint8_t* brightness) {
    Menu menu = { 0 };
    const char* shot = getenv("TMC_SETTINGS_SHOT");

    menu.msgSpeed = msgSpeed;
    menu.brightness = brightness;
    if (PcGetRenderer() == NULL) { // headless
        if (shot != NULL)
            SaveShots(&menu, shot);
        return;
    }
    Run(&menu);
    if (msgSpeed != NULL) {
        gSaveHeader->msg_speed = *msgSpeed;
        gSaveHeader->brightness = *brightness;
        gUsedPalettes = 0xFFFFFFFF;
    }
    PcRestoreGameView();
}

// The Quest Status screen's SLEEP button opens this menu instead, so its
// label becomes OPTIONS: redraws the label in the button's sprite tiles (OBJ
// tiles 736-759, a 32x32 and a 16x32 sprite) in the same letter style.
void PcDrawOptionsButton(void) {
    // 12 rows each: 3 = ink, 5 = edge, e = button.
    static const char* const sLetters[] = {
        "53335|33e33|33e33|33e33|33e33|33e33|33e33|33e33|33e33|33e33|33e33|53335",       // O
        "33335e|33e533|33ee33|33ee33|33e533|33335e|33eeee|33eeee|33eeee|33eeee|33eeee|33eeee", // P
        "333333|553355|ee33ee|ee33ee|ee33ee|ee33ee|ee33ee|ee33ee|ee33ee|ee33ee|ee33ee|ee33ee", // T
        "33|33|33|33|33|33|33|33|33|33|33|33",                                           // I
        "53335|33e33|33e33|33e33|33e33|33e33|33e33|33e33|33e33|33e33|33e33|53335",       // O
        "335e33|333e33|333e33|333533|335333|33e333|33e333|33e333|33e533|33ee33|33ee33|33ee33", // N
        "53335|33e33|33e33|33eee|335ee|e3335|ee533|eee33|eee33|33e33|33e33|53335",       // S
    };
    u8* tiles = (u8*)OBJ_VRAM0 + 736 * 32;
    int x, y, i, left = 3;

#define PUT(px, py, v)                                                                                       \
    do {                                                                                                     \
        int tile = (px) < 32 ? ((py) / 8) * 4 + (px) / 8 : 16 + ((py) / 8) * 2 + ((px)-32) / 8;              \
        u8* b = tiles + tile * 32 + ((py) % 8) * 4 + ((px) % 8) / 2;                                         \
        *b = (px) & 1 ? (*b & 0x0F) | ((v) << 4) : (*b & 0xF0) | (v);                                        \
    } while (0)

    for (y = 4; y < 16; y++)
        for (x = 2; x < 46; x++)
            PUT(x, y, 0xE);
    for (i = 0; i < (int)(sizeof(sLetters) / sizeof(sLetters[0])); i++) {
        const char* p = sLetters[i];
        int width = (int)strcspn(p, "|");
        for (y = 4; *p != '\0'; y++) {
            for (x = 0; x < width; x++, p++)
                PUT(left + x, y, *p == 'e' ? 0xE : *p - '0');
            if (*p == '|')
                p++;
        }
        left += width + 1;
    }
#undef PUT
}
