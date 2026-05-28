// C Header File
// Created 2/2/02; 12:36:29 PM

#ifndef __CLIPSPRITES__
#define __CLIPSPRITES__

#include "dllexports.h"
//char rect_diag_overlap(short x0, short y0, short xdim0, short ydim0, short x1, short y1, short d1, short h1, char direction);
#define GraySpriteClip8_OR _DLL_call(void, (short, short, short, unsigned char*), GRAYSPRITECLIP8_OR)
#define GraySpriteClip8_MASK _DLL_call(void, (short asm("%d0"), short asm("%d1"), short asm("%d2"), unsigned char* asm("%a0"), void* asm("%a1"), void* asm("%a2")), GRAYSPRITECLIP8_MASK)
#define GraySpriteClip16_MASK _DLL_call(void, (short asm("%d0"), short asm("%d1"), short asm("%d2"), unsigned short* asm("%a0"), void* asm("%a1"), void* asm("%a2")), GRAYSPRITECLIP16_MASK)
#define GraySpriteClip32_MASK _DLL_call(void, (short asm("%d0"), short asm("%d1"), short asm("%d2"), unsigned long* asm("%a0"), void* asm("%a1"), void* asm("%a2")), GRAYSPRITECLIP32_MASK)

#define GraySpriteClipX8_MASK _DLL_call(void, (short, short, short, unsigned char*, short, char), GRAYSPRITECLIPX8_MASK)
#define GraySpriteClipX8_WMASK _DLL_call(void, (short, short, short, unsigned char*, short, char), GRAYSPRITECLIPX8_WMASK)
#define GraySpriteClipVFlipX8_MASK _DLL_call(void, (short, short, short, unsigned char*, short, char), GRAYSPRITECLIPVFLIPX8_MASK)

#define rect_overlap _DLL_call(char, (short, short, short, short, short, short, short, short), RECT_OVERLAP)
#define rect_diag_overlap _DLL_call(char, (short, short, short, short, short, short, short, short, char), RECT_DIAG_OVERLAP)
#define TestCollideX8 _DLL_call(char, (short, short, short, short, short, short, short, short, unsigned char*, unsigned char *), TEST_COLLIDE_X8)


#endif


