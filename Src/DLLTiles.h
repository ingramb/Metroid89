// C Header File
// Created 1/22/2003; 3:31:58 PM

#ifndef __DLL_TILES__
#define __DLL_TILES__

#define __IN_DLL__
#include "tiles.h"

char tile_setup();
void tile_cleanup();
char tileset_load(short i, char u);

void hole_reset();
void hole_enemy_check();
char hole_create(short x, short y, unsigned short damage_type);
void hole_process();

#endif
