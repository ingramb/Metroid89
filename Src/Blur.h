// C Header File
// Created 7/26/02; 12:13:09 AM

#ifndef __BLUR__
#define __BLUR__

typedef struct {
	SPRITE_HEADER *header;
	unsigned char *gfx;
	short x;
	short y;
	short counter;
	char flash;
} BLUR;

char blur_setup();
void blur_reset();
void blur_cleanup();
void blur_create(short x, short y, SPRITE_HEADER *header, unsigned char *gfx);
void blur_process();
void blur_draw();

#endif

