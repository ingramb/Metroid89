// C Header File
// Created 11/19/2002; 1:08:58 PM

// Native port: the old per-DLL import thunks (calls through glbs->dll_imports[])
// are now plain prototypes resolved by direct static linking.

#ifndef __DLL_UTILITY__
#define __DLL_UTILITY__

#include "dllexports.h"

#define NONE -1

#define LEFT 1
#define RIGHT 2
#define UP 4
#define DOWN 8

char gfx_setup();
void gfx_cleanup(void);
void flipping_on(void);
void flipping_off(void);
void clear_buffer(void);
void update_screen(void);
void pause(void);
void delay(short a);
char file_exists(const char *file);
void *file_pointer(const char *file);
void error(char *txt);

void SpriteX8(short, short, short, unsigned char *, short, unsigned char *, char);
void GraySpriteX8(short, short, short, unsigned char *, unsigned char *, short, unsigned char *, unsigned char *, char);
void GraySpriteX8_mask(short, short, short, unsigned char *, short, unsigned char *, unsigned char *);

unsigned short decompress(unsigned char *, unsigned char *);

#endif
