// C Header File
// Created 1/22/2003; 3:32:33 PM

#ifndef __DLL_ENTITY__
#define __DLL_ENTITY__

#define __IN_DLL__
#include "entity.h"
#include "map.h"

extern char hit_slope_up;
extern char hit_slope_down;
extern char hit_solid;

void entity_init(ENTITY *e, short x, short y, ENTITY_SIZE *size, char simple);

void entity_set_pos(ENTITY *e, short x, short y);
char entity_check_size(ENTITY *e, ENTITY_SIZE *size);
void entity_set_size(ENTITY *e, ENTITY_SIZE *size);

void entity_set_gravity(ENTITY *e, char gravity);
void entity_reset_slope(ENTITY *e);
void entity_gravity_rotate_right(ENTITY *e);
void entity_gravity_rotate_left(ENTITY *e);

char entity_check_direction(ENTITY *e, char direction);
char entity_move_internal(ENTITY *e, char x_dir, char y_dir, short speed);
char entity_move_simple(ENTITY *e, char x_dir, char y_dir, short speed);
char entity_move(ENTITY *e, char direction, short speed);

void entity_draw(ENTITY *e, unsigned char *gfx, char flash, char use_offset, char wmask);

#endif
