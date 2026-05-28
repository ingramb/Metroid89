// C Header File
// Created 1/8/02; 9:25:27 PM

#ifndef __ENTITY__
#define __ENTITY__

#include "dllexports.h"

typedef struct {
	unsigned short offset;
	char x_offset;
	char y_offset;
	unsigned char width;
	unsigned char hieght;
} SPRITE_HEADER;

typedef struct {
	char x_off;
	char y_off;
	short width;
	short hieght;
	short frame_width;
	short frame_hieght;
	char gravity_rotate;
} ENTITY_SIZE;

typedef struct {
	unsigned char frame_number;
	unsigned char repeat_frame;
	const unsigned char frames[];
} ENTITY_ANIMATION;

typedef struct {
	short x;
	short y;

	short tile_x0;
	char tile_xoff0;
	short tile_y0;
	char tile_yoff0;
	short tile_x1;
	char tile_xoff1;
	short tile_y1;
	char tile_yoff1;

	char gravity;
	short x_speed_counter;
	short y_speed_counter;
	short x_moved;
	short y_moved;
	unsigned char slope_right_up;
	unsigned char slope_left_up;
	unsigned char slope_right_down;
	unsigned char slope_left_down;
	char x_off;
	char y_off;
	char width;
	char hieght;
	char simple_movement;
	char water_walking;
	unsigned short damage_type;
	ENTITY_SIZE *size;
	SPRITE_HEADER *sprite;
} ENTITY;

#ifndef __IN_DLL__
#define hit_slope_up _DLL_glbvar(char, HIT_SLOPE_UP)
#define hit_slope_down _DLL_glbvar(char, HIT_SLOPE_DOWN)
#define hit_solid _DLL_glbvar(char, HIT_SOLID)
#define entity_init _DLL_call(void, (ENTITY *, short, short, ENTITY_SIZE *, char), ENTITY_INIT)
#define entity_set_pos _DLL_call(void, (ENTITY *, short, short), ENTITY_SET_POS)
#define entity_check_size _DLL_call(char ,(ENTITY *, ENTITY_SIZE *), ENTITY_CHECK_SIZE)
#define entity_set_size _DLL_call(void ,(ENTITY *, ENTITY_SIZE *), ENTITY_SET_SIZE)
#define entity_set_gravity _DLL_call(void ,(ENTITY *, char), ENTITY_SET_GRAVITY)
#define entity_reset_slope _DLL_call(void ,(ENTITY *), ENTITY_RESET_SLOPE)
#define entity_gravity_rotate_right _DLL_call(void ,(ENTITY *), ENTITY_GRAVITY_ROTATE_RIGHT)
#define entity_gravity_rotate_left _DLL_call(void ,(ENTITY *), ENTITY_GRAVITY_ROTATE_LEFT)
#define entity_check_direction _DLL_call(char ,(ENTITY *, char), ENTITY_CHECK_DIRECTION)
#define entity_move_internal _DLL_call(char ,(ENTITY *, char, char, short), ENTITY_MOVE_INTERNAL)
#define entity_move_simple _DLL_call(char ,(ENTITY *, char, char, short), ENTITY_MOVE_SIMPLE)
#define entity_move _DLL_call(char ,(ENTITY *, char, short), ENTITY_MOVE)
#define entity_draw _DLL_call(void ,(ENTITY *, unsigned char *, char, char, char), ENTITY_DRAW)
#endif

#endif
