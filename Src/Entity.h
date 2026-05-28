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
extern char hit_slope_up;
extern char hit_slope_down;
extern char hit_solid;
void entity_init(ENTITY *, short, short, ENTITY_SIZE *, char);
void entity_set_pos(ENTITY *, short, short);
char entity_check_size(ENTITY *, ENTITY_SIZE *);
void entity_set_size(ENTITY *, ENTITY_SIZE *);
void entity_set_gravity(ENTITY *, char);
void entity_reset_slope(ENTITY *);
void entity_gravity_rotate_right(ENTITY *);
void entity_gravity_rotate_left(ENTITY *);
char entity_check_direction(ENTITY *, char);
char entity_move_internal(ENTITY *, char, char, short);
char entity_move_simple(ENTITY *, char, char, short);
char entity_move(ENTITY *, char, short);
void entity_draw(ENTITY *, unsigned char *, char, char, char);
#endif

#endif
