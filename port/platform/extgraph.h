// Native port replacement for the ExtGraph library header.
// Declarations only; implementations live in sprite_shim.c (and a few are
// already pure C in the game tree: MiscGFX.c, dllClipSprites.c, unpack.c).

#ifndef PORT_EXTGRAPH_H
#define PORT_EXTGRAPH_H

// ---- line / pixel ----
void FastDrawLine(void *plane, short x1, short y1, short x2, short y2, short mode);
void FastDrawHLine(void *plane, short x1, short x2, short y, short mode);
void FastDrawLine2(void *plane, short x1, short y1, short x2, short y2, short mode);

// Set/clear/swap a single pixel in a 1bpp plane (PLANE_STRIDE byte rows, MSB-first).
#define EXT_SETPIX(plane, x, y) \
    ( ((unsigned char *)(plane))[(y) * PLANE_STRIDE + ((x) >> 3)] |= (0x80 >> ((x) & 7)) )
#define EXT_SETPIX2(plane, x, y) EXT_SETPIX(plane, x, y)
#define EXT_CLRPIX(plane, x, y) \
    ( ((unsigned char *)(plane))[(y) * PLANE_STRIDE + ((x) >> 3)] &= ~(0x80 >> ((x) & 7)) )
#define EXT_CLRPIX2(plane, x, y) EXT_CLRPIX(plane, x, y)
#define EXT_XCHG(a, b) do { typeof(a) _t = (a); (a) = (b); (b) = _t; } while (0)

// ---- fixed-width gray sprites (8/16 px) ----
void GraySprite8_OR  (short x, short y, short h, unsigned char *l, unsigned char *d, void *lb, void *db);
void GraySprite8_XOR (short x, short y, short h, unsigned char *l, unsigned char *d, void *lb, void *db);
void GraySprite8_MASK(short x, short y, short h, unsigned char *l, unsigned char *d, unsigned char *m, unsigned char *m2, void *lb, void *db);
void GraySprite8_BLIT(short x, short y, short h, unsigned char *l, unsigned char *d, unsigned char mask, void *lb, void *db);

void GraySprite16_OR  (short x, short y, short h, unsigned short *l, unsigned short *d, void *lb, void *db);
void GraySprite16_XOR (short x, short y, short h, unsigned short *l, unsigned short *d, void *lb, void *db);
void GraySprite16_MASK(short x, short y, short h, unsigned short *l, unsigned short *d, unsigned short *m, void *lb, void *db);

// ---- horizontal mirror helper ----
void SpriteX8_MIRROR_H(short h, unsigned char *src, short bytewidth, unsigned char *dest);

// ---- TTArchive container (reverse-engineered in tifile.c) ----
short  ttarchive_valid(void *archive);
void  *ttarchive_data(void *archive, unsigned short index);

// ---- TTUnpack: ttunpack_valid/size + result codes (header lives in Src/) ----
#include "ttunpack.h"
// The real decompressor is C in unpack.c; the m68k inline-asm fallback in
// ttunpack.h is never expanded on native.
short UnpackBuffer(unsigned char *src, unsigned char *dest);

#endif
