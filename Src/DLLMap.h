// C Header File
// Created 12/28/2002; 10:52:54 PM

#ifndef __DLL_MAP__
#define __DLL_MAP__

#include "map.h"

void draw_map0();
void draw_map1(void *light, void *dark,
	TILE_DATA *tile_data,
	TILE_NODE *tile_list,
	TILE_NODE_CLIPPED *tile_list_clipped);
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
