// C Source File
// Created 11/8/2002; 5:46:47 PM

//#define USE_TI89              // Compile for TI-89
#define NO_CALC_CHECK
#define NO_EXIT_SUPPORT

#include <dll.h>
#include <string.h>
#include "dllexports.h"

#define __IN_DLL__
#include "map.h"
#include "tiles.h"
#include "block.h"
#include "animation.h"
#include "environment.h"
#include "player.h"
#include "globals.h"

DLL_INTERFACE

//extern const PLAYER_ACTION player_action[];

#include "dllclipsprites.h"
#include "dlltestcollide.h"
#include "dllmap.h"
#include "dllblock.h"
#include "dllanimation.h"
#include "dlltiles.h"
#include "dllentity.h"
#include "dllenvironment.h"
#include "dllpipe.h"

DLL_ID METROID_ID
DLL_VERSION MAJOR_VERSION, MINOR_VERSION
DLL_EXPORTS
//	dll_get_state,
//	dll_set_state,

	GraySpriteClip8_OR,
	GraySpriteClip8_MASK,
	GraySpriteClip16_MASK,
	GraySpriteClip32_MASK,
	GraySpriteClipX8_MASK,
	GraySpriteClipX8_WMASK,
	GraySpriteClipVFlipX8_MASK,
	rect_overlap,
	//rect_diag_overlap,
	TestCollideX8,

	//&tile_list,
	//&tile_list_clipped,
	//&current_map,
	bg_setup,
	bg_reset,
	bg_cleanup,
	bg_draw,
	draw_map0,
	draw_map1,
	get_data,
	get_prop_data,
	set_data,
	set_prop_data,

	block_setup,
	block_reset,
	block_cleanup,
	block_create,
	block_stretch_vert,
	block_stretch_horiz,
	block_move,
	block_delete,
	block_test,
	block_check,

	animation_setup,
	animation_reset,
	animation_cleanup,
	animation_create,
	animation_delete,
	animation_process,
	animation_draw,

	//&tile_data,
	tile_setup,
	tile_cleanup,
	tileset_load,
	hole_reset,
	hole_enemy_check,
	hole_create,
	hole_process,

	&hit_slope_up,
	&hit_slope_down,
	&hit_solid,
	entity_init,
	entity_set_pos,
	entity_check_size,
	entity_set_size,
	entity_set_gravity,
	entity_reset_slope,
	entity_gravity_rotate_right,
	entity_gravity_rotate_left,
	entity_check_direction,
	entity_move_internal,
	entity_move_simple,
	entity_move,
	entity_draw,

	//&water,
	environment_setup,
	environment_process,
	environment_draw,
	
	pipe_init,
	pipe_reset,
	pipe_cleanup,
	pipe_open,
	pipe_process,
	pipe_draw

DLL_IMPLEMENTATION
