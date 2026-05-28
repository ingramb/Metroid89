// C Source File
// Created 1/8/02; 10:56:43 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "extgraph.h"
#include "dllutility.h"
#include "dlltiles.h"
#include "player.h"
#include "dllmap.h"
#include "shots.h"
#include "dllanimation.h"
#include "dlltestcollide.h"
#include "globals.h"

#define HOLE_DURATION 1000
#define BREAK_SPEED_DURATION 20
#define BREAK_STAND_DURATION 43
#define BREAK_STAND_WAIT 8
#define BREAK_CHAIN_WAIT 3

//void *tile_file;

//void *tile_buffer;
//TILE_DATA *tile_data;
//char universal_loaded;
//short tileset;

//HOLE *holes = NULL;


unsigned char hole_type_gfx[] = {9, 7, 8, 17, 9};

char tile_setup()
{
	if(!file_exists(SYMSTR("tiles"))) return FALSE;

	//holes = malloc(sizeof(HOLE) * HOLE_MAX);
	//if(holes == NULL) return FALSE;

	glbs->tile_file = file_pointer(SYMSTR("tiles"));

	if(!ttarchive_valid(glbs->tile_file)) return FALSE;

	glbs->tile_data = NULL;
	glbs->tile_buffer = NULL;
	glbs->universal_loaded = FALSE;
	glbs->tileset = -1;

	return TRUE;
}

void tile_cleanup()
{
	if(glbs->tile_buffer != NULL) {
		free(glbs->tile_buffer);
		glbs->tile_buffer = NULL;
	}
	//if(holes != NULL) {
	//	free(holes);
	//	holes = NULL;
	//}
}

//DO SOMETHING ABOUT TILE_NUMBER EMBEDED IN TILEFILE
char tileset_load(short i, char u)
{
	void *ptr = ttarchive_data(glbs->tile_file, i + 1);

	if(!ttunpack_valid(ptr)) return FALSE;

	if(glbs->tileset != i) {
		if(glbs->tile_buffer != NULL) free(glbs->tile_buffer);
		glbs->tile_buffer = malloc(ttunpack_size(ptr));
		decompress(ptr, glbs->tile_buffer);
		glbs->universal_loaded = FALSE;
	}

	if(u && !glbs->universal_loaded) {
		ptr = ttarchive_data(glbs->tile_file, 0);
		if(!ttunpack_valid(ptr)) return FALSE;

		glbs->tile_buffer = realloc(glbs->tile_buffer, 256 * sizeof(TILE_DATA));
		if(!glbs->tile_buffer) return FALSE;

		decompress(ptr, glbs->tile_buffer + MAX_TILES * sizeof(TILE_DATA));
	} else if(!u)
		glbs->tile_buffer = realloc(glbs->tile_buffer, ttunpack_size(ptr));

	glbs->tile_data = (TILE_DATA *)(glbs->tile_buffer + 2);
	glbs->tileset = i;
	glbs->universal_loaded = u;

	//ClrScr();
	//printf_xy(50, 50, "%d", glbs->tile_data[1].hit);
	//pause();

	return TRUE;
}

void hole_reset()
{
	short i;

	memset(glbs->holes, 0, sizeof(HOLE) * HOLE_MAX);
	for(i = 0 ; i < HOLE_MAX - 1 ; i++) glbs->holes[i].next = i + 1;
	glbs->holes[HOLE_MAX - 1].next = -1;

	glbs->first_hole = -1;
	glbs->first_empty_hole = 0;
}

void hole_enemy_check()
{
	short x, y;

	for(x = 0 ; x < glbs->current_map.width ; x++) {
		for(y = 0 ; y < glbs->current_map.hieght ; y++) {
			if(get_prop_data(x, y) == BREAK_ENEMY) hole_create(x, y, DAMAGE_ENEMY_KILL);
		}
	}
}

char hole_create(short x, short y, unsigned short damage_type)
{
	unsigned char data = get_prop_data(x, y), tile;
	short i = glbs->first_empty_hole, a;
	char kill_it = TRUE;

	if(data < BREAK_STAND_REFORM || data > BREAK_SHOOT_BOMB_SWITCH || damage_type == 0) return FALSE;

	if(damage_type == DAMAGE_ALL) goto skip_damage_check;

	if(data == BREAK_ENEMY && !(damage_type & DAMAGE_ENEMY_KILL))
		kill_it = FALSE;
	else if(data == BREAK_SUPERMISSILE && !(damage_type & DAMAGE_SUPERMISSILE))
		kill_it = FALSE;
	else if(data == BREAK_POWERBOMB && !(damage_type & DAMAGE_POWERBOMB))
		kill_it = FALSE;
	else if(data >= BREAK_BOMB && data <= BREAK_BOMB_CHAIN && !(damage_type & DAMAGE_SCREW) &&
		!(damage_type & DAMAGE_BOMB) && !(damage_type & DAMAGE_POWERBOMB)) kill_it = FALSE;
	else if((data == BREAK_SPEED || data == BREAK_SPEED_REFORM) && !(damage_type & DAMAGE_SPEED))
		kill_it = FALSE;
	else if(data == BREAK_SCREW && !(damage_type & DAMAGE_SCREW))
		kill_it = FALSE;
	else if(data >= BREAK_STAND_REFORM && data <= BREAK_STAND_CHAIN && !(damage_type & DAMAGE_STAND))
		kill_it = FALSE;
	else if(damage_type == DAMAGE_STAND && (data < BREAK_STAND_REFORM || data > BREAK_STAND_CHAIN))
		kill_it = FALSE;

	if(!kill_it && (damage_type == DAMAGE_BOMB || damage_type == DAMAGE_POWERBOMB)) {
		if(data <= BREAK_STAND_CHAIN) tile = 18;
		else if(data <= BREAK_BOMB_CHAIN) tile = 6;
		else tile = hole_type_gfx[data - BREAK_SPEED_REFORM];
		glbs->current_map.data[y * glbs->current_map.width + x] = tile + MAX_TILES;
	}
	if(!kill_it) return FALSE;

skip_damage_check:

	animation_create(x * 12 + 6, y * 12 + 6, BLOCK_EXPLODE, NONE);

	if(data == BREAK_SHOOT_BOMB_SWITCH) {
		glbs->current_map.data[y * glbs->current_map.width + x] = MAX_TILES + 51;
		glbs->current_map.prop_data[y * glbs->current_map.width + x] = EMPTY | 0x80;
	} else if(data < BREAK_STAND_REFORM || data > BREAK_STAND_CHAIN) {
		glbs->current_map.data[y * glbs->current_map.width + x] = 0;
		glbs->current_map.prop_data[y * glbs->current_map.width + x] = EMPTY;
	}

	if(i < 0 || data > BREAK_SPEED_REFORM)
		return TRUE;

	glbs->first_empty_hole = glbs->holes[i].next;

	glbs->holes[i].next = glbs->first_hole;
	glbs->first_hole = i;

	glbs->holes[i].x = x;
	glbs->holes[i].y = y;
	glbs->holes[i].type = data;
	if(data >= BREAK_STAND_REFORM && data <= BREAK_STAND_CHAIN) {
		glbs->holes[i].counter = BREAK_STAND_DURATION;
		glbs->current_map.prop_data[y * glbs->current_map.width + x] = SOLID;
	} else if(data == BREAK_SPEED_REFORM)
		glbs->holes[i].counter = BREAK_SPEED_DURATION;
	else
		glbs->holes[i].counter = HOLE_DURATION;

	for(a = 0 ; a < 4 ; a++) {
		unsigned char left = BREAK_STAND_LEFT_REFORM + 12 * a;
		unsigned char ul = BREAK_STAND_UL_CORNER_REFORM + 12 * a;
		unsigned char bl = BREAK_STAND_BL_CORNER_REFORM + 12 * a;
		unsigned char right = BREAK_STAND_RIGHT_REFORM + 12 * a;
		unsigned char ur = BREAK_STAND_UR_CORNER_REFORM + 12 * a;
		unsigned char br = BREAK_STAND_BR_CORNER_REFORM + 12 * a;
		unsigned char up = BREAK_STAND_UP_REFORM + 12 * a;
		unsigned char down = BREAK_STAND_DOWN_REFORM + 12 * a;
		unsigned char mid_horiz = BREAK_STAND_MIDDLE_HORIZ_REFORM + 12 * a;
		unsigned char mid_vert = BREAK_STAND_MIDDLE_VERT + 12 * a;

		if(data == left || data == ul || data == bl || data == mid_horiz)
			hole_create(x + 1, y, DAMAGE_ALL);
		if(data == right || data == ur || data == br || data == mid_horiz)
			hole_create(x - 1, y, DAMAGE_ALL);
		if(data == up || data == ul || data == ur || data == mid_vert)
			hole_create(x, y + 1, DAMAGE_ALL);
		if(data == down || data == bl || data == br || data == mid_vert)
			hole_create(x, y - 1, DAMAGE_ALL);
	}

	return TRUE;
}

void hole_delete(short a)
{
	short i;

	if(a == glbs->first_hole)
		glbs->first_hole = glbs->holes[a].next;
	else
		for(i = glbs->first_hole ; i >= 0 ; i = glbs->holes[i].next)
			if(glbs->holes[i].next == a) {
				glbs->holes[i].next = glbs->holes[a].next;
				break;
			}

	glbs->holes[a].next = glbs->first_empty_hole;
	glbs->first_empty_hole = a;
}

void hole_process()
{
	short i;
	short next;
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short pw = glbs->player.e.width;
	short ph = glbs->player.e.hieght;
	unsigned char tile, type;

	for(i = glbs->first_hole ; i >= 0 ; i = next) {
		next = glbs->holes[i].next;
		glbs->holes[i].counter--;
		type = glbs->holes[i].type;

		if((type >= BREAK_STAND_REFORM && type <= BREAK_STAND_CHAIN) &&
			glbs->holes[i].counter == BREAK_STAND_DURATION - BREAK_STAND_WAIT) {
				glbs->current_map.data[glbs->holes[i].y * glbs->current_map.width + glbs->holes[i].x] = 0;
				glbs->current_map.prop_data[glbs->holes[i].y * glbs->current_map.width + glbs->holes[i].x] = EMPTY;
				if(glbs->holes[i].type >= BREAK_STAND) {
					hole_delete(i);
					continue;
				}
		}

		if((type == BREAK_SHOOT_CHAIN || type == BREAK_BOMB_CHAIN ||
			type == BREAK_STAND_CHAIN || type == BREAK_STAND_CHAIN_REFORM) &&
			glbs->holes[i].counter == HOLE_DURATION - BREAK_CHAIN_WAIT) {

				tile = get_prop_data(glbs->holes[i].x - 1, glbs->holes[i].y);
				if(tile == type) hole_create(glbs->holes[i].x - 1, glbs->holes[i].y, DAMAGE_ALL);

				tile = get_prop_data(glbs->holes[i].x + 1, glbs->holes[i].y);
				if(tile == type) hole_create(glbs->holes[i].x + 1, glbs->holes[i].y, DAMAGE_ALL);

				tile = get_prop_data(glbs->holes[i].x, glbs->holes[i].y - 1);
				if(tile == type) hole_create(glbs->holes[i].x, glbs->holes[i].y - 1, DAMAGE_ALL);

				tile = get_prop_data(glbs->holes[i].x, glbs->holes[i].y + 1);
				if(tile == type) hole_create(glbs->holes[i].x, glbs->holes[i].y + 1, DAMAGE_ALL);

				hole_delete(i);
			continue;
		}

		if(glbs->holes[i].counter > 18) continue;

		if(rect_overlap(glbs->holes[i].x * 12, glbs->holes[i].y * 12, 12, 12, px, py, pw, ph)) {
			glbs->holes[i].counter = 28;
			continue;
		}

		if(glbs->holes[i].counter == 18) {
			animation_create(glbs->holes[i].x * 12 + 6, glbs->holes[i].y * 12 + 6, BLOCK_REFORM, NONE);
			glbs->current_map.prop_data[glbs->holes[i].y * glbs->current_map.width + glbs->holes[i].x] = SOLID;
		} else if(glbs->holes[i].counter == 0) {
			if(glbs->holes[i].type <= BREAK_STAND_BR_CORNER) tile = 18;
			else if(glbs->holes[i].type <= BREAK_BOMB_CHAIN) tile = 6;
			else tile = hole_type_gfx[glbs->holes[i].type - BREAK_SPEED_REFORM];

			glbs->current_map.data[glbs->holes[i].y * glbs->current_map.width + glbs->holes[i].x] = tile + MAX_TILES;
			glbs->current_map.prop_data[glbs->holes[i].y * glbs->current_map.width + glbs->holes[i].x] = type;
			hole_delete(i);
		}
	}
}
