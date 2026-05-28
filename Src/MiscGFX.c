// C Source File
// Created 1/20/02; 5:01:38 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>
#include "entity.h"
#include "enemy.h"
#include "utility.h"
#include "miscgfx.h"
#include "globals.h"

// The gfx archive (Windows SpriteMaker output) is big-endian: each
// SPRITE_HEADER array is prefixed by a big-endian frame count and each
// header's .offset is big-endian. Swap offsets to host order in place.
static void swap_sprite_headers(SPRITE_HEADER *h)
{
	unsigned char *p = (unsigned char *)h - 2;          // big-endian frame count
	short count = (short)((p[0] << 8) | p[1]);
	short i;
	for(i = 0 ; i < count ; i++) {
		unsigned short o = h[i].offset;
		h[i].offset = (unsigned short)((o >> 8) | (o << 8));
	}
}

char miscgfx_setup()
{
	short i;

	void *file, *data;

	file = file_pointer(SYMSTR("gfx"));
	if(file == NULL) return FALSE;
	if(!ttarchive_valid(file)) return FALSE;

	glbs->font = (unsigned char *)ttarchive_data(file, 0);
	glbs->map_tiles = (unsigned char *)ttarchive_data(file, 1);
	glbs->anim_header = (SPRITE_HEADER *)(ttarchive_data(file, 2) + 2);
	glbs->anim_gfx = (unsigned char *)ttarchive_data(file, 3);
	glbs->door_header = (SPRITE_HEADER *)(ttarchive_data(file, 4) + 2);
	glbs->door_gfx = (unsigned char *)ttarchive_data(file, 5);
	glbs->shot_header = (SPRITE_HEADER *)(ttarchive_data(file, 6) + 2);
	glbs->shot_gfx = (unsigned char *)ttarchive_data(file, 7);
	//bg_tiles = (unsigned char *)(ttarchive_data(file, 8)); //KILL THIS!!!!
	glbs->water_gfx = (unsigned char *)(ttarchive_data(file, 8));
	glbs->powerup_header = (SPRITE_HEADER *)(ttarchive_data(file, 9) + 2);
	glbs->powerup_gfx = (unsigned char *)(ttarchive_data(file, 10));
	glbs->elevator_gfx = (unsigned char *)(ttarchive_data(file, 11));
	glbs->bomb_gfx = (unsigned char *)(ttarchive_data(file, 13));
	glbs->charge_gfx = (unsigned char *)(ttarchive_data(file, 14));
	glbs->bg_tile = (unsigned char *)(ttarchive_data(file, 15));
	glbs->item_gfx = (unsigned char *)(ttarchive_data(file, 16));
	glbs->compressed_title = (unsigned char *)(ttarchive_data(file, 17));
	glbs->pipe_gfx = (unsigned char *)(ttarchive_data(file, 18));
	glbs->save_room_gfx = (unsigned char *)(ttarchive_data(file, 19));
	glbs->special_gfx_base = (unsigned char *)(ttarchive_data(file, 20));
	glbs->special_gfx_anim = (unsigned char *)(ttarchive_data(file, 21));
	glbs->samus_death_header = ttarchive_data(file, 22);
	glbs->samus_death_gfx = ttarchive_data(file, 23);

	swap_sprite_headers(glbs->anim_header);
	swap_sprite_headers(glbs->door_header);
	swap_sprite_headers(glbs->shot_header);
	swap_sprite_headers(glbs->powerup_header);

	for(i = 0 ; i < 2 ; i++) {
		data = ttarchive_data(file, 24 + i);
		ridley_tail_data[i].frames = data + 2;
		ridley_tail_data[i].frame_number =
			(short)((((unsigned char *)data)[0] << 8) | ((unsigned char *)data)[1]);  // big-endian
	}

	glbs->trig_table = (char *)(ttarchive_data(file, 12));
	glbs->atan_table = (char *)(glbs->trig_table + 128);

	//for(i = 0 ; i < 128 ; i++) {
	//	ClrScr();
	//	printf_xy(0, 0, "%d", trig_table[i]);
	//	pause();
	//}

	return TRUE;
}

void GraySpriteX8_mask(short x, short y, short h, unsigned char* sprite1,
	short bytewidth, unsigned char *light, unsigned char *dark)
{
	SpriteX8(x, y, h, sprite1 + bytewidth * h * 2, bytewidth, light, SPRITE_AND);
	SpriteX8(x, y, h, sprite1 + bytewidth * h * 2, bytewidth, dark, SPRITE_AND);
	SpriteX8(x, y, h, sprite1, bytewidth, light, SPRITE_OR);
	SpriteX8(x, y, h, sprite1 + bytewidth * h, bytewidth, dark, SPRITE_OR);
}

void GraySpriteX8(short x, short y, short h, unsigned char* sprite1, unsigned char *sprite2,
	short bytewidth, unsigned char *light, unsigned char *dark, char mode)
{
	SpriteX8(x, y, h, sprite1, bytewidth, light, mode);
	SpriteX8(x, y, h, sprite2, bytewidth, dark, mode);
}

void SpriteX8(short x, short y, short h, unsigned char* sprite1, short bytewidth,
	unsigned char *buffer, char mode)
{
	unsigned char		*addr1;
	unsigned short	shift1;
	unsigned short	shift2;
	unsigned short	lineoffset;
	short						loop;

	addr1  = ((unsigned char*)buffer) + y * 30 + (x>>3);
	shift1 = x & 7;
	lineoffset = 30 - bytewidth;
	shift2 = 8 - shift1;

	if(mode == SPRITE_OR) {
		for(;h; h--, addr1+=lineoffset) {
			*addr1++ |= *sprite1 >> shift1;
			for(loop=1; loop<bytewidth; loop++) {
				*addr1   |= ((*sprite1++) << shift2);
				*addr1++ |= (*sprite1 >> shift1);
			}
			*addr1 |= (*sprite1++ << shift2);
		}
	} else if(mode == SPRITE_AND) {
		unsigned char startmask = 0xff << shift2;
		unsigned char endmask   = 0xff >> shift1;
		for(;h;h--,addr1+=lineoffset) {
			*addr1++ &= (*sprite1 >> shift1) | startmask;
			for(loop=1;loop<bytewidth;loop++) {
				unsigned char val = ((*sprite1++) << shift2);
				*addr1++ &= val | (*sprite1 >> shift1);
			}
			*addr1 &= (*sprite1++ << shift2) | endmask;
		}
	} else if(mode == SPRITE_XOR) {
		for(;h; h--, addr1+=lineoffset) {
			*addr1++ ^= *sprite1 >> shift1;
			for(loop=1; loop<bytewidth; loop++) {
				*addr1   ^= ((*sprite1++) << shift2);
				*addr1++ ^= (*sprite1 >> shift1);
			}
			*addr1 ^= (*sprite1++ << shift2);
		}
	}
}


