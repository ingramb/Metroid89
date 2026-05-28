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

// Phase-0 smoke test: draw the four gray levels as bands and show them.
// (Will be replaced by a call into the game's _main() once the engine is up.)
int main(void)
{
    unsigned char light[PLANE_STRIDE * SCREEN_H];
    unsigned char dark[PLANE_STRIDE * SCREEN_H];
    int running = 1;

    if (!screen_init()) return 1;

    for (int y = 0; y < SCREEN_H; y++) {
        for (int xb = 0; xb < PLANE_STRIDE; xb++) {
            // Four vertical bands of the four gray levels.
            int band = (xb * 8) * 4 / SCREEN_W;
            light[y * PLANE_STRIDE + xb] = (band & 1) ? 0xFF : 0x00;
            dark[y * PLANE_STRIDE + xb]  = (band & 2) ? 0xFF : 0x00;
        }
    }

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }
        screen_present(light, dark);
        SDL_Delay(16);
    }

    screen_quit();
    return 0;
}
