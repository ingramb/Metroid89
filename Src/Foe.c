// C Source File
// Created 7/17/02; 10:08:13 PM

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>

#include "utility.h"
#include "foe.h"

void enemy_draw(ENEMY *e, char x_pos)
{
	SPRITE_HEADER *header = enemy_header + e->stats.gfx;
	unsigned short frame_size = header->width * header->hieght;
	
	GraySpriteX8_MASK(x_buffer + x_pos, y_buffer + 3, header->hieght,
		enemy_gfx + header->offset,
		enemy_gfx + header->offset + frame_size,
		enemy_gfx + header->offset + frame_size * 2,
		enemy_gfx + header->offset + frame_size * 2,
		header->width, light_buffer, dark_buffer);
}