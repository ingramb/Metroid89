// C Header File
// Created 11/14/2002; 5:05:55 PM
void GraySpriteClip8_OR(short x, short y, short h, unsigned char* sprite1);
void GraySpriteClip8_MASK(
	short x, short y, short h,
	char *sprt, void *light_buffer, void *dark_buffer);
void GraySpriteClip16_MASK(
	short x, short y, short h,
	short *sprt, void *light_buffer, void *dark_buffer);
void GraySpriteClip32_MASK(
	short x, short y, short h,
	long *sprt, void *light_buffer, void *dark_buffer);

void GraySpriteClipX8_MASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash);
void GraySpriteClipX8_WMASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash);
void GraySpriteClipVFlipX8_MASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash);