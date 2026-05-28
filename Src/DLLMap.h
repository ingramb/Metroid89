// C Header File
// Created 12/28/2002; 10:52:54 PM

#ifndef __DLL_MAP__
#define __DLL_MAP__

#define __IN_DLL__
#include "map.h"

void draw_map0();
void draw_map1(void *light asm("%a2"), void *dark asm("%a3"),
	TILE_DATA *tile_data asm("%a4"),
	TILE_NODE *tile_list asm("%a0"),
	TILE_NODE_CLIPPED *tile_list_clipped asm("%a1"));
unsigned char get_data(short x, short y);
unsigned char get_prop_data(short x, short y);
void set_data(short x, short y, unsigned char a);
void set_prop_data(short x, short y, unsigned char a);

char bg_setup();
void bg_reset();
void bg_cleanup();
void bg_draw();

//extern TILE_NODE *tile_list;
//extern TILE_NODE_CLIPPED *tile_list_clipped;

#endif
