// C Header File
// Created 11/19/2002; 1:08:58 PM

#ifndef __DLL_UTILITY__
#define __DLL_UTILITY__

#include "dllexports.h"

#define NONE -1

#define LEFT 1
#define RIGHT 2
#define UP 4
#define DOWN 8

#define gfx_setup (*(char (*)())(glbs->dll_imports[GFX_SETUP]))
#define gfx_cleanup (*(void (*)())(glbs->dll_imports[GFX_CLEANUP]))
#define flipping_on (*(void (*)())(glbs->dll_imports[FLIPPING_ON]))
#define flipping_off (*(void (*)())(glbs->dll_imports[FLIPPING_OFF]))
#define clear_buffer (*(void (*)())(glbs->dll_imports[CLEAR_BUFFER]))
#define update_screen (*(void (*)())(glbs->dll_imports[UPDATE_SCREEN]))
#define pause (*(void (*)())(glbs->dll_imports[PAUSE]))
#define delay (*(void (*)(short))(glbs->dll_imports[DELAY]))
#define file_exists (*(char (*)(const char *))(glbs->dll_imports[FILE_EXISTS]))
#define file_pointer (*(void * (*)(const char *))(glbs->dll_imports[FILE_POINTER]))
#define error (*(void (*)(const char *))(glbs->dll_imports[ERROR]))

#define SpriteX8 (*(void (*)(short, short, short, unsigned char *, short, unsigned char *, char))(glbs->dll_imports[SPRITEX8]))
#define GraySpriteX8 (*(void (*)(short, short, short, unsigned char *, unsigned char *, short, unsigned char *, unsigned char *, char))(glbs->dll_imports[GRAY_SPRITEX8]))
#define GraySpriteX8_mask (*(void (*)(short, short, short, unsigned char *, short, unsigned char *, unsigned char *))(glbs->dll_imports[GRAY_SPRITEX8_MASK]))

#define decompress (*(unsigned short (* __attribute__((__stkparm__)))(unsigned char *, unsigned char *))(glbs->dll_imports[DECOMPRESS]))

#endif
