// C Source File
// Created 1/8/02; 9:28:44 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "entity.h"
#include "tiles.h"
#include "utility.h"
#include "map.h"
#include "block.h"
#include "shots.h"
#include "clipsprites.h"

char hit_slope_up;
char hit_slope_down;
char hit_solid;

short internal_x_off;
short internal_y_off;

short tile_x0;
short tile_x1;
short tile_y0;
short tile_y1;
char tile_xoff0;
char tile_xoff1;
char tile_yoff0;
char tile_yoff1;

void entity_init(ENTITY *e, short x, short y, ENTITY_SIZE *size, char simple)
{
	memset(e, 0, sizeof(ENTITY));
	
	e->simple_movement = simple;
	e->size = size;
	e->x = x;
	e->y = y;
	entity_set_gravity(e, DOWN);
}

void entity_set_pos(ENTITY *e, short x, short y)
{
	e->x = x;
	e->y = y;
	
	if(e->simple_movement) {
		short x0 = e->x + e->x_off + e->width / 2;
		short y0 = e->y + e->y_off + e->hieght / 2;
		e->tile_x0 = x0 / 12;
		e->tile_y0 = y0 / 12;
		e->tile_xoff0 = x0 % 12;
		e->tile_yoff0 = y0 % 12;
		return;
	}
	
	e->tile_x0 = (e->x + e->x_off) / 12;
	e->tile_y0 = (e->y + e->y_off) / 12;
	e->tile_x1 = (e->x + e->x_off + e->width - 1) / 12;
	e->tile_y1 = (e->y + e->y_off + e->hieght - 1) / 12;
	
	e->tile_xoff0 = (e->x + e->x_off) % 12;
	e->tile_yoff0 = (e->y + e->y_off) % 12;
	e->tile_xoff1 = (e->x + e->x_off + e->width - 1) % 12;
	e->tile_yoff1 = (e->y + e->y_off + e->hieght - 1) % 12;
}

char entity_check_size(ENTITY *e, ENTITY_SIZE *size)
{
	char y_off;
	short _tile_x0;
	short _tile_x1;
	short _tile_y0;
	short _tile_y1;
	short x, y;
	unsigned char tile;
	
	if(e->gravity == DOWN) y_off = size->y_off;
	else y_off = size->frame_hieght - size->hieght - size->y_off;
	
	_tile_x0 = (e->x + size->x_off) / 12;
	_tile_x1 = (e->x + size->x_off + size->width - 1) / 12;
	_tile_y0 = (e->y + y_off) / 12;
	_tile_y1 = (e->y + y_off + size->hieght - 1) / 12;
	
	for(x = _tile_x0 ; x <= _tile_x1 ; x++)
		for(y = _tile_y0 ; y <= _tile_y1 ; y++) {
			tile = get_prop_data(x, y);
			if(tile >= SOLID && tile < SLOPE_RIGHT_UP) return FALSE;
		}
		
	return TRUE;
}


void entity_set_size(ENTITY *e, ENTITY_SIZE *size)
{
	e->size = size;
	
	if(e->gravity == DOWN) {
		e->x_off = e->size->x_off;
		e->y_off = e->size->y_off;
		e->width = e->size->width;
		e->hieght = e->size->hieght;
	} else if(e->gravity == UP) {
		e->x_off = e->size->x_off;
		e->y_off = e->size->frame_hieght - e->size->hieght - e->size->y_off;
		e->width = e->size->width;
		e->hieght = e->size->hieght;
	} else if(e->gravity == RIGHT) {
		if(size->gravity_rotate) {
			e->x_off = size->y_off;
			e->y_off = size->frame_width - size->width - size->x_off;
			e->width = size->hieght;
			e->hieght = size->width;
		} else {
			e->x_off = size->frame_width - size->width - size->x_off;
			e->y_off = size->y_off;
			e->width = size->width;
			e->hieght = size->hieght;
		}
	} else if(e->gravity == LEFT) {
		if(size->gravity_rotate) {
			e->x_off = size->frame_hieght - size->hieght - size->y_off;
			e->y_off = size->x_off;
			e->width = size->hieght;
			e->hieght = size->width;
		} else {
			e->x_off = size->x_off;
			e->y_off = size->y_off;
			e->width = size->width;
			e->hieght = size->hieght;
		}
	}
	
	entity_set_pos(e, e->x, e->y);
}

void entity_set_gravity(ENTITY *e, char gravity)
{
	ENTITY_SIZE *size = e->size;
	
	e->gravity = gravity;
	
	if(gravity == DOWN) {
		e->slope_left_up = SLOPE_LEFT_UP;
		e->slope_right_up = SLOPE_RIGHT_UP;
	} else if(gravity == UP) {
		e->slope_left_up = SLOPE_LEFT_DOWN;
		e->slope_right_up = SLOPE_RIGHT_DOWN;
	} else if(gravity == RIGHT) {
		e->slope_left_up = SLOPE_RIGHT_DOWN;
		e->slope_right_up = SLOPE_RIGHT_UP;
	} else if(gravity == LEFT) {
		e->slope_left_up = SLOPE_LEFT_DOWN;
		e->slope_right_up = SLOPE_LEFT_UP;
	}
	
	e->x_speed_counter = 0;
	e->y_speed_counter = 0;
	
	entity_set_size(e, size);
}

void entity_gravity_rotate_right(ENTITY *e)
{
	short x = e->x + e->x_off;
	short y = e->y + e->y_off;
	
	if(e->gravity == DOWN) {
		x = x + e->width - e->hieght;
		y = y - e->hieght + e->width;
		entity_set_gravity(e, RIGHT);
	} else if(e->gravity == RIGHT) {
		x = x - e->hieght + e->width;
		entity_set_gravity(e, UP);
	} else if(e->gravity == UP) {	
		entity_set_gravity(e, LEFT);
	} else if(e->gravity == LEFT) {
		y = y + e->hieght - e->width;
		entity_set_gravity(e, DOWN);
	}
		
	entity_set_pos(e, x - e->x_off, y - e->y_off);
}

void entity_gravity_rotate_left(ENTITY *e)
{
	short x = e->x + e->x_off;
	short y = e->y + e->y_off;
	
	if(e->gravity == DOWN) {
		y++;
		entity_set_gravity(e, LEFT);
	} else if(e->gravity == LEFT) {
		x = x - e->hieght + e->width - 1;
		entity_set_gravity(e, UP);
	} else if(e->gravity == UP) {
		x = x + e->width - e->hieght;
		y = y + e->hieght - e->width - 1;
		entity_set_gravity(e, RIGHT);
	} else if(e->gravity == RIGHT) {
		x++;
		y = y + e->hieght - e->width;
		entity_set_gravity(e, DOWN);
	}
		
	entity_set_pos(e, x - e->x_off, y - e->y_off);
}

void entity_draw(ENTITY *e, unsigned char *gfx, char flash, char use_offset, char wmask)
{	
	SPRITE_HEADER *header = e->sprite;
	short x = e->x + header->x_offset - camera.x;
	short y = e->y - camera.y;
	unsigned short offset = header->offset;
	
	if(!use_offset) offset = 0;
	
	if(wmask) {
		y += header->y_offset;
		GraySpriteClipX8_WMASK(x, y, header->hieght,
			(gfx + offset), header->width, flash);
	} else if(e->gravity == UP) {
		y += e->size->frame_hieght - header->hieght - header->y_offset;
		GraySpriteClipVFlipX8_MASK(x, y, header->hieght,
			(gfx + offset), header->width, flash);
	} else {
		y += header->y_offset;
		GraySpriteClipX8_MASK(x, y, header->hieght,
			(gfx + offset), header->width, flash);
	}
	
	/*if(1) {
		SCR_RECT r = (SCR_RECT){{e->x + e->x_off - camera.x, e->y + e->y_off - camera.y,
			e->x + e->x_off + e->width - 1 - camera.x, e->y + e->y_off + e->hieght - 1 - camera.y}};
		SCR_RECT clip = (SCR_RECT){{0, 0, 239, 127}};
		PortSet(light_buffer, 239, 127);
		ScrRectFill(&r, &clip, A_NORMAL);
		PortSet(dark_buffer, 239, 127);
		ScrRectFill(&r, &clip, A_NORMAL);
	}*/
}

void check_slope_down(ENTITY *e, short tile_x, short tile_y, char x_dir)
{
	char data = get_prop_data(tile_x, tile_y);

	if((data == e->slope_right_up && x_dir < 0) || (data == e->slope_left_up && x_dir > 0))
		hit_slope_down = TRUE;
}
void check_slope_up(ENTITY *e, short tile_x, short tile_y, char x_dir)
{
	char data = get_prop_data(tile_x, tile_y);
	
	if((x_dir < 0 && data == e->slope_left_up) || (x_dir > 0 && data == e->slope_right_up))
			hit_slope_up = TRUE;
}

char entity_check_tile_horiz(ENTITY *e, short tile_x, short tile_y)
{
	char data = get_prop_data(tile_x, tile_y);
	short y_off;
	short x_off;

	if(data < SOLID || data == BACKGROUND_SOLID) return FALSE;
	if(data >= BREAK_SHOOT && data <= BREAK_SHOOT_ENERGY)
		return !hole_create(tile_x, tile_y, e->damage_type);
	else if(data < SLOPE_RIGHT_UP) return TRUE;
	
	if(e->gravity == RIGHT) x_off = tile_xoff1;
	else if(e->gravity == LEFT) x_off = 11 - tile_xoff0;
	else return FALSE;
	
	if(data == e->slope_right_up) {
		if(tile_y1 > tile_y) y_off = 11;
		else y_off = tile_yoff1;
	} else if(data == e->slope_left_up) {
		if(tile_y0 < tile_y) y_off = 0;
		else y_off = 11 - tile_yoff0;
	} else
		return FALSE;
		
	
	if(y_off + x_off < 11) return FALSE;
	
	return TRUE;
}

char entity_check_tile_vert(ENTITY *e, short tile_x, short tile_y, char y_dir)
{
	char data = get_prop_data(tile_x, tile_y);
	short y_off;
	short x_off;

	if(data < SOLID) return FALSE;
	if(data == BACKGROUND_SOLID) return (y_dir > 0 && e->tile_yoff1 == 11);
	
	if((e->damage_type & DAMAGE_STAND) && data == BREAK_STAND && y_dir == 1)
		hole_create(tile_x, tile_y, DAMAGE_STAND);

	if(data >= BREAK_SHOOT && data <= BREAK_SHOOT_ENERGY)
		return !hole_create(tile_x, tile_y, e->damage_type);
	else if(data < SLOPE_RIGHT_UP) return TRUE;
	
	if(e->gravity == DOWN) y_off = tile_yoff1;
	else if(e->gravity == UP) y_off = 11 - tile_yoff0;
	else return FALSE;
	
	if(data == e->slope_right_up) {
		if(tile_x1 > tile_x) x_off = 11;
		else x_off = tile_xoff1;
	} else if(data == e->slope_left_up) { 
		if(tile_x0 < tile_x) x_off = 0;
		else x_off = 11 - tile_xoff0;
	} else
		return FALSE;
	
	if(y_off + x_off < 11) return FALSE;
	
	return TRUE;
}

char entity_check_move(ENTITY *e, char x_dir, char y_dir, char check_blocks)
{
	char move_ok = TRUE;
	short tile_below;
	
	register short x = 0, y = 0;
	
	tile_x0 = e->tile_x0;
	tile_x1 = e->tile_x1;
	tile_y0 = e->tile_y0;
	tile_y1 = e->tile_y1;
	tile_xoff0 = e->tile_xoff0;
	tile_xoff1 = e->tile_xoff1;
	tile_yoff0 = e->tile_yoff0;
	tile_yoff1 = e->tile_yoff1;
	
	hit_slope_up = FALSE;
	hit_slope_down = FALSE;
	hit_solid = FALSE;
	
	if(check_blocks) {
		if(block_test(e->x + e->x_off + x_dir, e->y + e->y_off + y_dir, e->width, e->hieght)) return FALSE;
	}
	
	if(x_dir) {
		if(x_dir > 0) {
			tile_xoff0++;
			tile_xoff1++;
			if(tile_xoff0 == 12) {
				tile_xoff0 = 0;
				tile_x0++;
			}
			if(tile_xoff1 == 12) {
				tile_xoff1 = 0;
				tile_x1++;
			}

			x = tile_x1;
		} else {
			tile_xoff0--;
			tile_xoff1--;
			if(tile_xoff0 < 0) {
				tile_xoff0 = 11;
				tile_x0--;
			}
			if(tile_xoff1 < 0) {
				tile_xoff1 = 11;
				tile_x1--;
			}
		
			x = tile_x0;
		}
		
		for(y = tile_y0 ; y <= tile_y1 ; y++)
			if(entity_check_tile_horiz(e, x, y))
				move_ok = FALSE;
				
		if(e->gravity == DOWN) {
			tile_below = (e->y + y_dir + e->y_off + e->hieght) / 12;
			check_slope_up(e, x, tile_y1, x_dir);
			if(x_dir > 0) x = e->tile_x0;
			else x = e->tile_x1;
			check_slope_down(e, x, tile_below, x_dir);
		} else if(e->gravity == UP) {
			tile_below = (e->y + y_dir + e->y_off - 1) / 12;
			check_slope_up(e, x, tile_y0, x_dir);
			if(x_dir > 0) x = e->tile_x0;
			else x = e->tile_x1;
			check_slope_down(e, x, tile_below, x_dir);
		}
	}
	
	if(y_dir) {
		if(y_dir > 0) {
			tile_yoff0++;
			tile_yoff1++;
			if(tile_yoff0 == 12) {
				tile_yoff0 = 0;
				tile_y0++;
			}
			if(tile_yoff1 == 12) {
				tile_yoff1 = 0;
				tile_y1++;
			}

			y = tile_y1;
		} else {
			tile_yoff0--;
			tile_yoff1--;
			if(tile_yoff0 < 0) {
				tile_yoff0 = 11;
				tile_y0--;
			}
			if(tile_yoff1 < 0) {
				tile_yoff1 = 11;
				tile_y1--;
			}
		
			y = tile_y0;
		}
		
		for(x = tile_x0 ; x <= tile_x1 ; x++)
			if(entity_check_tile_vert(e, x, y, y_dir))
				move_ok = FALSE;
				
		if(e->gravity == RIGHT) {
			tile_below = (e->x + x_dir + e->x_off + e->width) / 12;
			check_slope_up(e, tile_x1, y, y_dir);
			if(y_dir > 0) y = e->tile_y0;
			else y = e->tile_y1;
			check_slope_down(e, tile_below, y, y_dir);
		} else if(e->gravity == LEFT) {
			tile_below = (e->x + x_dir + e->x_off - 1) / 12;
			check_slope_up(e, tile_x0, y, y_dir);
			if(y_dir > 0) y = e->tile_y0;
			else y = e->tile_y1;
			check_slope_down(e, tile_below, y, y_dir);
		}
	}
		
	return move_ok;
}

char entity_check_direction(ENTITY *e, char direction, char check_blocks)
{
	char x_dir = 0;
	char y_dir = 0;
	char a;
	
	if(direction & LEFT) x_dir = -1;
	if(direction & RIGHT) x_dir = 1;
	if(direction & UP) y_dir = -1;
	if(direction & DOWN) y_dir = 1;
	
	if(e->gravity == UP) {
		x_dir *= -1;
		y_dir *= -1;
	} else if(e->gravity == RIGHT) {
		a = x_dir;
		x_dir = y_dir;
		y_dir = -a;
	} else if(e->gravity == LEFT) {
		a = x_dir;
		x_dir = -y_dir;
		y_dir = a;
	}
	
	return entity_check_move(e, x_dir, y_dir, check_blocks);
}

char entity_move(ENTITY *e, char direction, short speed, char check_blocks)
{
	char x_dir = 0;
	char y_dir = 0;
	char a;
	
	if(direction & LEFT) x_dir = -1;
	if(direction & RIGHT) x_dir = 1;
	if(direction & UP) y_dir = -1;
	if(direction & DOWN) y_dir = 1;
	
	if(e->gravity == UP) {
		x_dir *= -1;
		y_dir *= -1;
	} else if(e->gravity == RIGHT) {
		a = x_dir;
		x_dir = y_dir;
		y_dir = -a;
	} else if(e->gravity == LEFT) {
		a = x_dir;
		x_dir = -y_dir;
		y_dir = a;
	}
	
	if(e->simple_movement) return entity_move_simple(e, x_dir, y_dir, speed);
	else return entity_move_internal(e, x_dir, y_dir, speed, TRUE, check_blocks);
}

char entity_move_internal(ENTITY *e, char x_dir, char y_dir, short speed, char slope_down, char check_blocks)
{
	char moved = TRUE;
	char xdir = x_dir;
	char ydir = y_dir;
	
	if(xdir) e->x_speed_counter += speed;
	if(ydir) e->y_speed_counter += speed;
	e->x_moved = e->y_moved = 0;
	
	while(e->x_speed_counter >= 100 || e->y_speed_counter >= 100) {
		
		if(e->x_speed_counter >= 100) {
			if(entity_check_move(e, xdir, 0, check_blocks)) {
				e->x += xdir;
				e->tile_x0 = tile_x0;
				e->tile_x1 = tile_x1;
				e->tile_xoff0 = tile_xoff0;
				e->tile_xoff1 = tile_xoff1;
				if(hit_slope_up && y_dir == 0) {
					if(e->gravity == DOWN) ydir = -1;
					else if(e->gravity == UP) ydir = 1;
					e->y_speed_counter += 100;
				} else if(hit_slope_down && y_dir == 0 && slope_down) {
					if(e->gravity == DOWN) ydir = 1;
					else if(e->gravity == UP) ydir = -1;
					e->y_speed_counter += 100;
				}
				
				e->x_moved += xdir;
				
			} else
				moved = FALSE;
			
			e->x_speed_counter -= 100;	
		}
		
		if(e->y_speed_counter >= 100) {
			if(entity_check_move(e, 0, ydir, check_blocks)) {
				e->y += ydir;
				e->tile_y0 = tile_y0;
				e->tile_y1 = tile_y1;
				e->tile_yoff0 = tile_yoff0;
				e->tile_yoff1 = tile_yoff1;
				if(hit_slope_up && x_dir == 0) {
					if(e->gravity == RIGHT) xdir = -1;
					else if(e->gravity == LEFT) xdir = 1;
					e->x_speed_counter += 100;
				} else if(hit_slope_down && x_dir == 0 && slope_down) {
					if(e->gravity == RIGHT) xdir = 1;
					else if(e->gravity == LEFT) xdir = -1;
					e->x_speed_counter += 100;
				}
				
				e->y_moved += ydir;
				
			} else
				moved = FALSE;
			
			e->y_speed_counter -= 100;	
		}
		
	}
	
	return moved;
}

char entity_move_simple(ENTITY *e, char x_dir, char y_dir, short speed)
{
	unsigned char data;
	
	if(x_dir) e->x_speed_counter += speed;
	if(y_dir) e->y_speed_counter += speed;
	
	while(e->x_speed_counter >= 100) {
		e->x += x_dir;
		e->tile_xoff0 += x_dir;
		
		if(e->tile_xoff0 < 0) {
			e->tile_xoff0 = 11;
			e->tile_x0--;
		} else if(e->tile_xoff0 > 11) {
			e->tile_xoff0 = 0;
			e->tile_x0++;
		}
		e->x_speed_counter -= 100;
	}
	
	while(e->y_speed_counter >= 100) {
		e->y += y_dir;
		e->tile_yoff0 += y_dir;
		
		if(e->tile_yoff0 < 0) {
			e->tile_yoff0 = 11;
			e->tile_y0--;
		} else if(e->tile_yoff0 > 11) {
			e->tile_yoff0 = 0;
			e->tile_y0++;
		}
		e->y_speed_counter -= 100;
	}
	
	data = get_prop_data(e->tile_x0, e->tile_y0);
	
	return (data < SOLID || data >= BACKGROUND_SOLID);
}