// C Header File
// Created 1/20/2003; 9:28:36 PM

#ifndef __DLL_BLOCK__
#define __DLL_BLOCK__

#define __IN_DLL__
#include "block.h"

char block_setup();
void block_reset();
void block_cleanup();
short block_create(short x, short y, short width, short hieght);
void block_stretch_vert(short a, short d, short mode);
void block_stretch_horiz(short a, short d, short mode);
void block_move(short a, short direction, short speed);
void block_delete(short a);
char block_test(short x, short y, short width, short hieght, short x_dir, short y_dir);
char block_check(short i, short x, short y, short width, short hieght);

#endif
