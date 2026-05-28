// SDL frontend for the Metroid 89 port.
// Owns the window/renderer and translates the game's two 1-bpp planes
// (light + dark = 4 gray levels) into a texture each frame.

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform/screen.h"

#define SCALE 4

static SDL_Window   *g_window;
static SDL_Renderer *g_renderer;
static SDL_Texture  *g_texture;

// 4-gray palette, light/dark plane bit pair -> ARGB. Index = (dark<<1)|light.
// 0 = white background, 3 = black. Matches calculator: set bit = darker.
static const Uint32 g_palette[4] = {
    0xFFFFFFFF, // 00 white
    0xFFAAAAAA, // 01 light gray (light plane only)
    0xFF555555, // 10 dark gray  (dark plane only)
    0xFF000000, // 11 black      (both)
};

int screen_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 0;
    }
    g_window = SDL_CreateWindow("Metroid 89",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_W * SCALE, SCREEN_H * SCALE, SDL_WINDOW_SHOWN);
    if (!g_window) { fprintf(stderr, "CreateWindow: %s\n", SDL_GetError()); return 0; }

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer)
        g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_SOFTWARE);
    if (!g_renderer) { fprintf(stderr, "CreateRenderer: %s\n", SDL_GetError()); return 0; }

    g_texture = SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, SCREEN_W, SCREEN_H);
    if (!g_texture) { fprintf(stderr, "CreateTexture: %s\n", SDL_GetError()); return 0; }

    return 1;
}

// Blit a light/dark plane pair (PLANE_STRIDE bytes/row, MSB-first 1bpp) to the window.
void screen_present(const unsigned char *light, const unsigned char *dark)
{
    Uint32 *pixels;
    int pitch;
    int x, y;

    if (SDL_LockTexture(g_texture, NULL, (void **)&pixels, &pitch) != 0)
        return;

    for (y = 0; y < SCREEN_H; y++) {
        const unsigned char *lrow = light + y * PLANE_STRIDE;
        const unsigned char *drow = dark  + y * PLANE_STRIDE;
        Uint32 *prow = (Uint32 *)((unsigned char *)pixels + y * pitch);
        for (x = 0; x < SCREEN_W; x++) {
            int bit = 0x80 >> (x & 7);
            int l = (lrow[x >> 3] & bit) ? 1 : 0;
            int d = (drow[x >> 3] & bit) ? 1 : 0;
            prow[x] = g_palette[(d << 1) | l];
        }
    }

    // Debug: METROID89_SHOT=path:frame saves the computed SCREEN_WxSCREEN_H frame
    // as BMP and exits. Captured straight from the pixel buffer (not via
    // RenderReadPixels, which is unreliable after Present).
    {
        static int frame = 0;
        const char *env = getenv("METROID89_SHOT");
        frame++;
        if (env) {
            char path[512]; int want = 1;
            const char *colon = strrchr(env, ':');
            if (colon) { want = atoi(colon + 1); snprintf(path, sizeof(path), "%.*s", (int)(colon - env), env); }
            else snprintf(path, sizeof(path), "%s", env);
            if (frame == want) {
                SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_W, SCREEN_H, 32, SDL_PIXELFORMAT_ARGB8888);
                int yy;
                for (yy = 0; yy < SCREEN_H; yy++)
                    memcpy((unsigned char *)s->pixels + yy * s->pitch,
                           (unsigned char *)pixels + yy * pitch, SCREEN_W * 4);
                SDL_SaveBMP(s, path);
                SDL_FreeSurface(s);
                SDL_UnlockTexture(g_texture);
                screen_quit();
                exit(0);
            }
        }
    }

    SDL_UnlockTexture(g_texture);
    SDL_RenderClear(g_renderer);
    SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
    SDL_RenderPresent(g_renderer);
}

void screen_quit(void)
{
    if (g_texture)  SDL_DestroyTexture(g_texture);
    if (g_renderer) SDL_DestroyRenderer(g_renderer);
    if (g_window)   SDL_DestroyWindow(g_window);
    SDL_Quit();
}

// --- input: emulate the TI-89 keyboard matrix that _rowread() polls ---------
// The game selects a matrix row via an active-low mask (one cleared bit) and
// reads back the key bits for that row. We map physical Mac keys to the exact
// (row, bit) positions the game tests (see Src/Player.c, Utility.h).

static int g_should_quit = 0;

void platform_pump(void)
{
    SDL_Event e;
    SDL_PumpEvents();
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) g_should_quit = 1;
    }
}

int platform_should_quit(void) { return g_should_quit; }

// Replacement for the calculator's AUTO_INT speed timer: return how many logic
// steps should run this frame based on elapsed wall-clock time (~70 Hz, capped).
int platform_logic_steps(void)
{
    static Uint32 last = 0, accum = 0;
    Uint32 now = SDL_GetTicks();
    int steps;
    if (last == 0) last = now;
    accum += now - last;
    last = now;
    steps = accum / 14;     // ~71 logic ticks/sec
    accum %= 14;
    if (steps > 4) steps = 4;
    return steps;
}

short _rowread(short mask)
{
    const Uint8 *k;
    unsigned short row = (unsigned short)mask;   // active-low: cleared bit = selected
    short r = 0;
    platform_pump();
    k = SDL_GetKeyboardState(NULL);

    // ARROWS_ROW (0xfffe, bit0): movement + 2nd/diamond/shift.
    if (!(row & 0x0001)) {
        if (k[SDL_SCANCODE_UP])     r |= 1;    // UP_KEY
        if (k[SDL_SCANCODE_LEFT])   r |= 2;    // LEFT_KEY
        if (k[SDL_SCANCODE_DOWN])   r |= 4;    // DOWN_KEY
        if (k[SDL_SCANCODE_RIGHT])  r |= 8;    // RIGHT_KEY
        if (k[SDL_SCANCODE_Z])      r |= 16;   // SEL_KEY  (2nd / shoot)
        if (k[SDL_SCANCODE_LSHIFT] || k[SDL_SCANCODE_RSHIFT]) r |= 32; // SHIFT (cheat)
        if (k[SDL_SCANCODE_SPACE])  r |= 64;   // DMND_KEY (diamond / jump)
    }
    // ESC_ROW (0xffbf, bit6)
    if (!(row & 0x0040)) {
        if (k[SDL_SCANCODE_ESCAPE]) r |= 1;    // ESC_KEY
    }
    // APPS_ROW (0xffdf, bit5): APPS=bit0 (map), F1=bit7 (beam select)
    if (!(row & 0x0020)) {
        if (k[SDL_SCANCODE_A] || k[SDL_SCANCODE_TAB]) r |= 1;   // APPS_KEY
        if (k[SDL_SCANCODE_F1]) r |= 128;                       // beam
    }
    // 0xffef bit4: F2 (missile) at bit7
    if (!(row & 0x0010)) { if (k[SDL_SCANCODE_F2]) r |= 128; }
    // 0xfff7 bit3: F3 (super missile) at bit7
    if (!(row & 0x0008)) { if (k[SDL_SCANCODE_F3]) r |= 128; }
    // 0xfffb bit2 (BSPACE_ROW): diag-up=bit6, F4 (power bomb)=bit7
    if (!(row & 0x0004)) {
        if (k[SDL_SCANCODE_Q]) r |= 64;     // BSPACE_KEY (diagonal up)
        if (k[SDL_SCANCODE_F4]) r |= 128;   // power bomb
    }
    // 0xfffd bit1 (CLEAR_ROW): diag-down=bit6, reverse-grav=bit7, plus=bit1, minus=bit2
    if (!(row & 0x0002)) {
        if (k[SDL_SCANCODE_E]) r |= 64;     // CLEAR_KEY (diagonal down)
        if (k[SDL_SCANCODE_F5]) r |= 128;   // reverse gravity
        if (k[SDL_SCANCODE_EQUALS]) r |= 2; // plus  (grayscale adjust)
        if (k[SDL_SCANCODE_MINUS]) r |= 4;  // minus
    }
    return r;
}

extern void _main(void);   // the game entry point (Src/metroid.c)

int main(void)
{
    if (!screen_init()) return 1;
    _main();
    screen_quit();
    return 0;
}
