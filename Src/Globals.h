// C Header File
// Created 7/1/2003; 3:12:27 PM

#ifndef __GLOBALS__
#define __GLOBALS__

#include <intr.h>
#include "dllexports.h"
#include "player.h"
#include "map.h"
#include "utility.h"
#include "shots.h"
#include "bomb.h"
#include "pipe.h"
#include "block.h"
#include "tiles.h"
#include "animation.h"
#include "environment.h"

typedef struct {
	PLAYER player;
	CURRENT_MAP current_map;
	CAMERA camera;
	MAP_SPECIAL special;
	SAVE_GAME save_game;
	SAVE_GAME respawn_point;
	METPACK_HEADER *metpack_header;
	void *metpack_base;
	BACKGROUND_HEADER *bg_list;

	SHOT shots[SHOT_MAX];
	SHOT_COMBO *combo_data;
	SHOT_DATA *shot_data;
	BOMB bombs[BOMB_MAX];

	PIPE *pipes;
	short pipe_number;

	unsigned char *bg_tile;
	char *bg_light;
	char *bg_dark;
	char *footer_light;
	char *footer_dark;

	TILE_NODE_CLIPPED tile_list_clipped[TILE_LIST_CLIPPED_SIZE];
	TILE_NODE tile_list[TILE_LIST_SIZE];
	BLOCK blocks[BLOCK_MAX];
	short first_block;
	short first_empty_block;
	void *tile_file;
	void *tile_buffer;
	TILE_DATA *tile_data;
	short tileset;
	char universal_loaded;
	HOLE holes[HOLE_MAX];
	short first_hole;
	short first_empty_hole;
	ANIMATION animations[ANIM_MAX];
	short first_animation;
	short first_empty_animation;
	char environment_type;
	WATER water;

	void *dll_imports[DLL_IMPORT_NUMBER];
	void *light_buffer;
	void *dark_buffer;
	void *dbuffer;
	short display_width;
	short display_hieght;
	short gray_adjust;
	char vti;
	char hw2;
	char ti92;
	char use_flipping;
	char use_flipping_save;
	INT_HANDLER save_int[AUTO_INT_COUNT];
	volatile short game_counter;
	volatile short fps_counter;
	volatile short current_fps;
	volatile short frames;
	volatile unsigned long seconds;
	volatile short timer;
	volatile short hw2_counter;

	unsigned char *font;
	unsigned char *map_tiles;
	unsigned char *water_gfx;
	unsigned char *elevator_gfx;
	unsigned char *bomb_gfx;
	unsigned char *charge_gfx;
	unsigned char *item_gfx;
	unsigned char *pipe_gfx;
	unsigned char *save_room_gfx;
	unsigned char *special_gfx_base;
	unsigned char *special_gfx_anim;
	unsigned char *compressed_title;

	SPRITE_HEADER *shot_header;
	unsigned char *shot_gfx;

	SPRITE_HEADER *anim_header;
	unsigned char *anim_gfx;

	SPRITE_HEADER *door_header;
	unsigned char *door_gfx;

	SPRITE_HEADER *powerup_header;
	unsigned char *powerup_gfx;

	void *samus_death_header;
	void *samus_death_gfx;

	char *trig_table;
	char *atan_table;
} GLOBALS;

register GLOBALS *glbs asm("a5");
extern GLOBALS *glbs_base;

#endif
