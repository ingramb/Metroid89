// ExtGraph drawing primitives for the native port.
//
// The fixed-width gray sprite routines are layered on the game's own
// byte-oriented SpriteX8 (Src/MiscGFX.c), which already handles arbitrary-x
// shifting and is endianness-safe (it reads sprite data byte by byte, MSB
// first -- exactly how the big-endian calculator data is laid out).

#include "tigcc_compat.h"

// From MiscGFX.c
#define SPRITE_OR  0
#define SPRITE_AND 1
#define SPRITE_XOR 2
void SpriteX8(short x, short y, short h, unsigned char *sprite, short bytewidth,
              unsigned char *buffer, char mode);

// --- single-plane line drawing (1bpp, PLANE_STRIDE byte rows, MSB-first) ---
static void hspan(unsigned char *plane, short x1, short x2, short y, short mode) {
    short x;
    if (y < 0 || y >= PLANE_ROWS) return;
    if (x1 > x2) { short t = x1; x1 = x2; x2 = t; }
    if (x1 < 0) x1 = 0;
    if (x2 >= PLANE_STRIDE * 8) x2 = PLANE_STRIDE * 8 - 1;
    for (x = x1; x <= x2; x++) {
        unsigned char *b = plane + y * PLANE_STRIDE + (x >> 3);
        unsigned char  m = 0x80 >> (x & 7);
        if      (mode == A_REVERSE) *b &= ~m;
        else if (mode == A_XOR)     *b ^= m;
        else                        *b |= m;   // A_NORMAL / A_REPLACE
    }
}

void FastDrawHLine(void *plane, short x1, short x2, short y, short mode) {
    hspan((unsigned char *)plane, x1, x2, y, mode);
}

void FastDrawLine(void *plane, short x1, short y1, short x2, short y2, short mode) {
    unsigned char *p = (unsigned char *)plane;
    short dx = x2 - x1, dy = y2 - y1;
    short adx = dx < 0 ? -dx : dx, ady = dy < 0 ? -dy : dy;
    short sx = dx < 0 ? -1 : 1, sy = dy < 0 ? -1 : 1;
    short x = x1, y = y1, err, e2;
    if (ady == 0) { hspan(p, x1, x2, y1, mode); return; }
    err = adx - ady;
    for (;;) {
        if (x >= 0 && x < PLANE_STRIDE * 8 && y >= 0 && y < PLANE_ROWS) {
            unsigned char *b = p + y * PLANE_STRIDE + (x >> 3);
            unsigned char  m = 0x80 >> (x & 7);
            if      (mode == A_REVERSE) *b &= ~m;
            else if (mode == A_XOR)     *b ^= m;
            else                        *b |= m;
        }
        if (x == x2 && y == y2) break;
        e2 = err << 1;
        if (e2 > -ady) { err -= ady; x += sx; }
        if (e2 <  adx) { err += adx; y += sy; }
    }
}

void FastDrawLine2(void *plane, short x1, short y1, short x2, short y2, short mode) {
    FastDrawLine(plane, x1, y1, x2, y2, mode);
}

// --- fixed-width gray sprites (two planes) ---
void GraySprite8_OR(short x, short y, short h, unsigned char *l, unsigned char *d, void *lb, void *db) {
    SpriteX8(x, y, h, l, 1, lb, SPRITE_OR);
    SpriteX8(x, y, h, d, 1, db, SPRITE_OR);
}
void GraySprite8_XOR(short x, short y, short h, unsigned char *l, unsigned char *d, void *lb, void *db) {
    SpriteX8(x, y, h, l, 1, lb, SPRITE_XOR);
    SpriteX8(x, y, h, d, 1, db, SPRITE_XOR);
}
// MASK: clear the destination through the mask plane(s), then OR in the data.
void GraySprite8_MASK(short x, short y, short h, unsigned char *l, unsigned char *d,
                      unsigned char *m, unsigned char *m2, void *lb, void *db) {
    SpriteX8(x, y, h, m,  1, lb, SPRITE_AND);
    SpriteX8(x, y, h, m2, 1, db, SPRITE_AND);
    SpriteX8(x, y, h, l,  1, lb, SPRITE_OR);
    SpriteX8(x, y, h, d,  1, db, SPRITE_OR);
}
// BLIT: AND each row with a constant byte mask, then OR in the data.
void GraySprite8_BLIT(short x, short y, short h, unsigned char *l, unsigned char *d,
                      unsigned char mask, void *lb, void *db) {
    unsigned char maskrow[64];
    short i;
    for (i = 0; i < h && i < 64; i++) maskrow[i] = mask;
    SpriteX8(x, y, h, maskrow, 1, lb, SPRITE_AND);
    SpriteX8(x, y, h, maskrow, 1, db, SPRITE_AND);
    SpriteX8(x, y, h, l, 1, lb, SPRITE_OR);
    SpriteX8(x, y, h, d, 1, db, SPRITE_OR);
}

void GraySprite16_OR(short x, short y, short h, unsigned short *l, unsigned short *d, void *lb, void *db) {
    SpriteX8(x, y, h, (unsigned char *)l, 2, lb, SPRITE_OR);
    SpriteX8(x, y, h, (unsigned char *)d, 2, db, SPRITE_OR);
}
void GraySprite16_XOR(short x, short y, short h, unsigned short *l, unsigned short *d, void *lb, void *db) {
    SpriteX8(x, y, h, (unsigned char *)l, 2, lb, SPRITE_XOR);
    SpriteX8(x, y, h, (unsigned char *)d, 2, db, SPRITE_XOR);
}
void GraySprite16_MASK(short x, short y, short h, unsigned short *l, unsigned short *d,
                       unsigned short *m, void *lb, void *db) {
    SpriteX8(x, y, h, (unsigned char *)m, 2, lb, SPRITE_AND);
    SpriteX8(x, y, h, (unsigned char *)m, 2, db, SPRITE_AND);
    SpriteX8(x, y, h, (unsigned char *)l, 2, lb, SPRITE_OR);
    SpriteX8(x, y, h, (unsigned char *)d, 2, db, SPRITE_OR);
}

// Horizontal mirror: reverse the bit order across the whole bytewidth*8 width.
void SpriteX8_MIRROR_H(short h, unsigned char *src, short bw, unsigned char *dest) {
    static const unsigned char rev[16] = {
        0x0,0x8,0x4,0xC,0x2,0xA,0x6,0xE,0x1,0x9,0x5,0xD,0x3,0xB,0x7,0xF };
    short row, b;
    for (row = 0; row < h; row++) {
        unsigned char *s = src  + row * bw;
        unsigned char *o = dest + row * bw;
        for (b = 0; b < bw; b++) {
            unsigned char v = s[b];
            unsigned char r = (unsigned char)((rev[v & 0x0f] << 4) | rev[v >> 4]);
            o[bw - 1 - b] = r;
        }
    }
}
