// C Header File
// Created 2/2/02; 12:36:29 PM

#ifndef __CLIPSPRITES__
#define __CLIPSPRITES__

#include "dllexports.h"
//char rect_diag_overlap(short x0, short y0, short xdim0, short ydim0, short x1, short y1, short d1, short h1, char direction);
void GraySpriteClip8_OR(short, short, short, unsigned char*);
void GraySpriteClip8_MASK(short, short, short, unsigned char*, void*, void*);
void GraySpriteClip16_MASK(short, short, short, unsigned short*, void*, void*);
void GraySpriteClip32_MASK(short, short, short, unsigned long*, void*, void*);

void GraySpriteClipX8_MASK(short, short, short, unsigned char*, short, char);
void GraySpriteClipX8_WMASK(short, short, short, unsigned char*, short, char);
void GraySpriteClipVFlipX8_MASK(short, short, short, unsigned char*, short, char);

char rect_overlap(short, short, short, short, short, short, short, short);
char rect_diag_overlap(short, short, short, short, short, short, short, short, char);
char TestCollideX8(short, short, short, short, short, short, short, short, unsigned char*, unsigned char *);


#endif


