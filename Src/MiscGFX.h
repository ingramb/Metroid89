// C Header File
// Created 1/20/02; 5:02:06 PM

#ifndef __MISCGFX__
#define __MISCGFX__

#include "entity.h"

char miscgfx_setup();

#define SPRITE_OR 0
#define SPRITE_AND 1
#define SPRITE_XOR 2

void SpriteX8(short x, short y, short h, unsigned char* sprite1, short bytewidth,
	unsigned char *buffer, char mode);

void GraySpriteX8_mask(short x, short y, short h, unsigned char* sprite1,
	short bytewidth, unsigned char *light, unsigned char *dark);

void GraySpriteX8(short x, short y, short h, unsigned char* sprite1, unsigned char *sprite2,
	short bytewidth, unsigned char *light, unsigned char *dark, char mode);

#endif


