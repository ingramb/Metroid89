// SDL frontend for the Metroid 89 port.
// Owns the window/renderer and translates the game's two 1-bpp planes
// (light + dark = 4 gray levels) into a texture each frame.

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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

static Uint32 timer_cb(Uint32 interval, void *param);   // game-clock timer
int platform_hz(void);

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

#ifndef __EMSCRIPTEN__
    // Native: a background SDL timer thread ticks the game clock. In the WASM
    // build there are no threads on the main thread; the clock is driven from
    // platform_yield() off the wall clock instead (see below).
    SDL_AddTimer(1000 / platform_hz(), timer_cb, NULL);   // game clock (~100 Hz)
#endif

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

// Calculator auto-interrupt emulation: a background SDL timer ticks the game
// clock (~70 Hz), so logic advances and the countdown timer that delay()/fade
// busy-wait loops depend on actually decrements.
extern void platform_timer_tick(void);

#ifdef __EMSCRIPTEN__
// WASM build: single-threaded on the browser main thread, compiled with
// ASYNCIFY. There is no timer thread, so advance the game clock here from the
// real wall clock, calling platform_timer_tick() once per 1000/hz ms elapsed.
static void platform_clock_pump(void)
{
    static double last = -1.0;
    static double accum = 0.0;
    double now = emscripten_get_now();
    double per = 1000.0 / platform_hz();
    double dt;
    int guard = 0;
    if (last < 0.0) last = now;
    dt = now - last;
    // Cap dt so a long stall (backgrounded tab, OS suspend, debugger pause)
    // can't make the game run in turbo when we resume. Without this the
    // accumulator banks every missed ms and drains over many frames.
    if (dt > 100.0) dt = 100.0;
    accum += dt;
    last = now;
    while (accum >= per && guard++ < 240) {   // clamp catch-up after a long stall
        accum -= per;
        platform_timer_tick();
    }
}

// The yield primitive every busy-wait loop calls: advance the clock, then hand
// control to the browser (ASYNCIFY unwinds/rewinds the C stack) so it can
// repaint and deliver input events.
void platform_yield(void)
{
    platform_clock_pump();
    emscripten_sleep(0);
}
#endif

// Game-clock frequency. The calculator drove logic off its programmable timer
// (~80 Hz here); tunable via METROID89_HZ.
int platform_hz(void)
{
    const char *e = getenv("METROID89_HZ");
    int hz = e ? atoi(e) : 80;
    if (hz < 20)  hz = 20;
    if (hz > 500) hz = 500;
    return hz;
}

static Uint32 timer_cb(Uint32 interval, void *param)
{
    (void)param;
    platform_timer_tick();
    return interval;
}

#ifdef __EMSCRIPTEN__
// On-screen touch controls (mobile). The HTML shell (port/shell.html) maintains
// a bitmask of held virtual buttons and pushes it here via Module._touch_set_keys;
// _rowread() then ORs the relevant bits into the matrix rows the game polls. The
// low bits intentionally match the ARROWS_ROW layout so they can be masked in
// directly. Keep these values in sync with the constants in shell.html.
enum {
    TK_UP = 1, TK_LEFT = 2, TK_DOWN = 4, TK_RIGHT = 8,  // ARROWS_ROW bits
    TK_SHOOT = 16, TK_JUMP = 64,                         // SEL, DMND (same row)
    TK_MAP = 0x100, TK_BEAM = 0x200, TK_MISSILE = 0x400  // other rows (see below)
};
static volatile unsigned g_touch = 0;
EMSCRIPTEN_KEEPALIVE void touch_set_keys(unsigned mask) { g_touch = mask; }
#endif

short _rowread(short mask)
{
    const Uint8 *k;
    unsigned short row = (unsigned short)mask;   // active-low: cleared bit = selected
    short r = 0;
#ifdef __EMSCRIPTEN__
    // _rowread is the game's universal input poll and is the body of many tight
    // wait-for-key spins (pause(), key-release waits) that never reach
    // update_screen. Those spins need a yield to make progress, but normal-frame
    // logic also calls _rowread dozens of times and must NOT yield each time (a
    // single emscripten_sleep already burns the browser's ~4 ms setTimeout floor,
    // so a wall-clock throttle defeats itself). Throttle by call count instead:
    // tight spins reach the threshold quickly, per-frame logic almost never does.
    {
        static unsigned calls = 0;
        if (++calls >= 256) { calls = 0; platform_clock_pump(); emscripten_sleep(0); }
    }
#endif
    platform_pump();
    k = SDL_GetKeyboardState(NULL);

    // Dev autopilot: continuously cycle SEL then DMND so we reliably walk past
    // the title pause() (needs a SEL press+release) and the save menu (needs
    // DMND), regardless of how long setup took.
    if (getenv("METROID89_AUTOPLAY") && !(row & 0x0001)) {
        Uint32 t = SDL_GetTicks();
        if (t < 300) return 0;
        switch ((t / 150) % 4) {
            case 0: return 16;   // SEL (2nd)
            case 2: return 64;   // DMND (diamond)
            default: return 0;
        }
    }
    // Walk-right autopilot (use with METROID89_SKIPTITLE): hold RIGHT and jump
    // periodically to verify movement/animation/scrolling.
    if (getenv("METROID89_WALK") && !(row & 0x0001)) {
        Uint32 t = SDL_GetTicks();
        short r;
        if (t < 800) return 0;                  // let setup settle
        r = 8;                                  // hold RIGHT
        if ((t / 350) % 3 == 0) r |= 64;        // periodic jump
        return r;
    }

    // ARROWS_ROW (0xfffe, bit0): movement + 2nd/diamond/shift.
    if (!(row & 0x0001)) {
        if (k[SDL_SCANCODE_UP])     r |= 1;    // UP_KEY
        if (k[SDL_SCANCODE_LEFT])   r |= 2;    // LEFT_KEY
        if (k[SDL_SCANCODE_DOWN])   r |= 4;    // DOWN_KEY
        if (k[SDL_SCANCODE_RIGHT])  r |= 8;    // RIGHT_KEY
        if (k[SDL_SCANCODE_Z])      r |= 16;   // SEL_KEY  (2nd / shoot)
        if (k[SDL_SCANCODE_LSHIFT] || k[SDL_SCANCODE_RSHIFT]) r |= 32; // SHIFT (cheat)
        if (k[SDL_SCANCODE_SPACE])  r |= 64;   // DMND_KEY (diamond / jump)
#ifdef __EMSCRIPTEN__
        r |= (short)(g_touch & (TK_UP|TK_LEFT|TK_DOWN|TK_RIGHT|TK_SHOOT|TK_JUMP));
#endif
    }
    // ESC_ROW (0xffbf, bit6). Closing the window (SDL_QUIT) also returns ESC so
    // the game loop exits cleanly and game_save() runs. METROID89_AUTOQUIT=<ms>
    // is a test hook to force a clean quit after a delay.
    if (!(row & 0x0040)) {
        const char *aq = getenv("METROID89_AUTOQUIT");
        if (k[SDL_SCANCODE_ESCAPE] || g_should_quit ||
            (aq && SDL_GetTicks() > (Uint32)atoi(aq))) r |= 1;   // ESC_KEY
    }
    // APPS_ROW (0xffdf, bit5): APPS=bit0 (map), F1=bit7 (beam select)
    if (!(row & 0x0020)) {
        if (k[SDL_SCANCODE_A] || k[SDL_SCANCODE_TAB]) r |= 1;   // APPS_KEY
        if (k[SDL_SCANCODE_F1]) r |= 128;                       // beam
#ifdef __EMSCRIPTEN__
        if (g_touch & TK_MAP)  r |= 1;
        if (g_touch & TK_BEAM) r |= 128;
#endif
    }
    // 0xffef bit4: F2 (missile) at bit7
    if (!(row & 0x0010)) {
        if (k[SDL_SCANCODE_F2]) r |= 128;
#ifdef __EMSCRIPTEN__
        if (g_touch & TK_MISSILE) r |= 128;
#endif
    }
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

#ifndef BUILD_ID
#define BUILD_ID "dev"
#endif

int main(void)
{
#ifdef __EMSCRIPTEN__
    // Stamp the build id into the page so we can confirm a fresh build loaded
    // (this runs from the WASM, so a matching stamp proves the .wasm is current).
    {
        char js[256];
        snprintf(js, sizeof(js),
            "var e=document.getElementById('ver'); if(e) e.textContent='build: %s';", BUILD_ID);
        emscripten_run_script(js);
    }
#endif
    if (!screen_init()) return 1;
    _main();
    screen_quit();
    return 0;
}
