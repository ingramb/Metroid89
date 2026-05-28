// C Header File
// Created 10/21/2002; 7:25:07 PM

#ifndef __BLOCK__
#define __BLOCK__

#include "dllexports.h"

typedef struct {
	short x;
	short y;
	short width;
	short hieght;
	short next;
} BLOCK;

#define BLOCK_MAX 15
#define BLOCK_STRETCH 1
#define BLOCK_SQUISH (-1)

#ifndef __IN_DLL__
char block_setup(void);
void block_reset(void);
void block_cleanup(void);
short block_create(short, short, short, short);
void block_stretch_vert(short, short, short);
void block_stretch_horiz(short, short, short);
void block_move(short, short, short);
void block_delete(short);
char block_test(short, short, short, short, short, short);
char block_check(short, short, short, short, short);
#endif

#endif