// C Header File
// Created 11/14/2002; 5:05:55 PM
void GraySpriteClip8_OR(short x, short y, short h, unsigned char* sprite1);
void GraySpriteClip8_MASK(
	short x asm("%d0"), short y asm("%d1"), short h asm("%d2"),
	char *sprt asm("%a0"), void *light_buffer asm("%a1"), void *dark_buffer asm("%a2"));
void GraySpriteClip16_MASK(
	short x asm("%d0"), short y asm("%d1"), short h asm("%d2"),
	short *sprt asm("%a0"), void *light_buffer asm("%a1"), void *dark_buffer asm("%a2"));
void GraySpriteClip32_MASK(
	short x asm("%d0"), short y asm("%d1"), short h asm("%d2"),
	long *sprt asm("%a0"), void *light_buffer asm("%a1"), void *dark_buffer asm("%a2"));

void GraySpriteClipX8_MASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash);
void GraySpriteClipX8_WMASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash);
void GraySpriteClipVFlipX8_MASK(
	short x,short y,short h, unsigned char* sprite1, short bytewidth, char flash);