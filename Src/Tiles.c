// C Source File
// Created 1/8/02; 10:56:43 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "extgraph.h"
#include "utility.h"
#include "tiles.h"
#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "door.h"
#include "map.h"
#include "shots.h"
#include "animation.h"
#include "clipsprites.h"

#define HOLE_MAX 40
#define HOLE_DURATION 1000
#define BREAK_SPEED_DURATION 20
#define BREAK_STAND_DURATION 43
#define BREAK_STAND_WAIT 8
#define BREAK_CHAIN_WAIT 3

void *tile_file;

void *tile_buffer;
TILE_DATA *tile_data;

HOLE *holes = NULL;

short first_hole;
short first_empty_hole;

unsigned char hole_type_gfx[] =
	{26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 29, 27, 28, 36, 29};

char tile_setup()
{
	if(!file_exists(SYMSTR("tiles"))) return FALSE;
	
	holes = malloc(sizeof(HOLE) * HOLE_MAX);
	if(holes == NULL) return FALSE;
	
	tile_file = file_pointer(SYMSTR("tiles"));
	
	if(!ttarchive_valid(tile_file)) return FALSE;
	
	tile_data = NULL;
	tile_buffer = NULL;
	
	return TRUE;
}

void tile_cleanup()
{
	if(tile_buffer != NULL) {
		free(tile_buffer);
		tile_buffer = NULL;
	}
	if(holes != NULL) {
		free(holes);
		holes = NULL;
	}
}

char tileset_load(short i)
{
	void *ptr = ttarchive_data(tile_file, i + 1);
	
	if(!ttunpack_valid(ptr)) return FALSE;
	
	if(tile_buffer != NULL) free(tile_buffer);
	
	tile_buffer = malloc(256 * sizeof(TILE_DATA));
	ttunpack_decompress(ptr, tile_buffer);

	ptr = ttarchive_data(tile_file, 0);
	if(!ttunpack_valid(ptr)) return FALSE;
	
	ttunpack_decompress(ptr, tile_buffer + MAX_TILES * sizeof(TILE_DATA));

	tile_data = (TILE_DATA *)(tile_buffer + 2);
	
	//ClrScr();
	//printf_xy(50, 50, "%d", tile_data[1].hit);
	//pause();
	
	return TRUE;
}

void hole_reset()
{
	short i;
	
	memset(holes, 0, sizeof(HOLE) * HOLE_MAX);
	for(i = 0 ; i < HOLE_MAX - 1 ; i++) holes[i].next = i + 1;
	holes[HOLE_MAX - 1].next = -1;
	
	first_hole = -1;
	first_empty_hole = 0;
}

char hole_create(short x, short y, unsigned short damage_type)
{
	unsigned char data = get_prop_data(x, y);
	short i = first_empty_hole;
	char kill_it = TRUE;
	
	if(data < BREAK_STAND || data > BREAK_SHOOT_ENERGY || damage_type == 0) return FALSE;
	
	if(data == BREAK_SUPERMISSILE && damage_type != DAMAGE_SUPERMISSILE) kill_it = FALSE;
	else if(data == BREAK_POWERBOMB && damage_type != DAMAGE_POWERBOMB) kill_it = FALSE;
	else if(data == BREAK_BOMB && damage_type != DAMAGE_SCREW &&
		damage_type != DAMAGE_BOMB && damage_type != DAMAGE_POWERBOMB) kill_it = FALSE;
	else if((data == BREAK_SPEED || data == BREAK_SPEED_REFORM) && damage_type != DAMAGE_SPEED)
		kill_it = FALSE;
	else if(data == BREAK_SCREW && damage_type != DAMAGE_SCREW) kill_it = FALSE;
	else if(data == BREAK_STAND && damage_type != DAMAGE_STAND) kill_it = FALSE;
	else if(damage_type == DAMAGE_STAND && data != BREAK_STAND) kill_it = FALSE;
	
	if(!kill_it && (damage_type == DAMAGE_BOMB || damage_type == DAMAGE_POWERBOMB))
		current_map.data[y * current_map.width + x] = hole_type_gfx[data - BREAK_STAND] + MAX_TILES;
	if(!kill_it) return FALSE;
	
	animation_create(x * 12 + 6, y * 12 + 6, BLOCK_EXPLODE, NONE);
	
	if(data >= BREAK_SHOOT_MISSILE && data <= BREAK_SHOOT_ENERGY) {
		current_map.data[y * current_map.width + x] = MAX_TILES + data - BREAK_SHOOT_MISSILE;
		current_map.prop_data[y * current_map.width + x] = BACKGROUND;
	} else if(data != BREAK_STAND) {
		current_map.data[y * current_map.width + x] = 0;
		current_map.prop_data[y * current_map.width + x] = EMPTY;
	}
	
	if(i < 0 || data > BREAK_SPEED_REFORM) return TRUE;
	
	first_empty_hole = holes[i].next;
	
	holes[i].next = first_hole;
	first_hole = i;
	
	holes[i].x = x;
	holes[i].y = y;
	holes[i].type = data;
	if(holes[i].type == BREAK_STAND)
		holes[i].counter = BREAK_STAND_DURATION;
	else if(data == BREAK_SPEED_REFORM)
		holes[i].counter = BREAK_SPEED_DURATION;
	else
		holes[i].counter = HOLE_DURATION;
		
	if(data == BREAK_SHOOT_LEFT || data == BREAK_SHOOT_UL_CORNER || data == BREAK_SHOOT_BL_CORNER)
		hole_create(x + 1, y, damage_type);
	if(data == BREAK_SHOOT_RIGHT || data == BREAK_SHOOT_UR_CORNER || data == BREAK_SHOOT_BR_CORNER)
		hole_create(x - 1, y, damage_type);
	if(data == BREAK_SHOOT_UP || data == BREAK_SHOOT_UL_CORNER || data == BREAK_SHOOT_UR_CORNER)
		hole_create(x, y + 1, damage_type);
	if(data == BREAK_SHOOT_DOWN || data == BREAK_SHOOT_BL_CORNER || data == BREAK_SHOOT_BR_CORNER)
		hole_create(x, y - 1, damage_type);
		
	return TRUE;
}

void hole_delete(short a)
{
	short i;
	
	if(a == first_hole)
		first_hole = holes[a].next;
	else
		for(i = first_hole ; i >= 0 ; i = holes[i].next)
			if(holes[i].next == a) {
				holes[i].next = holes[a].next;
				break;
			}
	
	holes[a].next = first_empty_hole;
	first_empty_hole = a;
}

void hole_process(PLAYER *p)
{
	short i;
	short next;
	short px = p->e.x + p->e.x_off;
	short py = p->e.y + p->e.y_off;
	short pw = p->e.width;
	short ph = p->e.hieght;
	
	for(i = first_hole ; i >= 0 ; i = next) {
		next = holes[i].next;
		holes[i].counter--;
		
		if(holes[i].type == BREAK_STAND && holes[i].counter == BREAK_STAND_DURATION - BREAK_STAND_WAIT) {
			current_map.data[holes[i].y * current_map.width + holes[i].x] = 0;
			current_map.prop_data[holes[i].y * current_map.width + holes[i].x] = EMPTY;
		}
		
		if(holes[i].type == BREAK_CHAIN && holes[i].counter == HOLE_DURATION - BREAK_CHAIN_WAIT) {
			if(get_prop_data(holes[i].x - 1, holes[i].y) == BREAK_CHAIN) hole_create(holes[i].x - 1, holes[i].y, DAMAGE_NORMAL);
			if(get_prop_data(holes[i].x + 1, holes[i].y) == BREAK_CHAIN) hole_create(holes[i].x + 1, holes[i].y, DAMAGE_NORMAL);
			if(get_prop_data(holes[i].x, holes[i].y - 1) == BREAK_CHAIN) hole_create(holes[i].x, holes[i].y - 1, DAMAGE_NORMAL);
			if(get_prop_data(holes[i].x, holes[i].y + 1) == BREAK_CHAIN) hole_create(holes[i].x, holes[i].y + 1, DAMAGE_NORMAL);
			hole_delete(i);
			continue;
		}
		
		if(holes[i].counter > 18) continue;
		
		if(rect_overlap(holes[i].x * 12, holes[i].y * 12, 12, 12, px, py, pw, ph)) {
			holes[i].counter = 28;
			continue;
		}
		
		if(holes[i].counter == 18) {
			animation_create(holes[i].x * 12 + 6, holes[i].y * 12 + 6, BLOCK_REFORM, NONE);
			current_map.prop_data[holes[i].y * current_map.width + holes[i].x] = SOLID;
		} else if(holes[i].counter == 0) {
			current_map.data[holes[i].y * current_map.width + holes[i].x] =
				hole_type_gfx[holes[i].type - BREAK_STAND] + MAX_TILES;
			current_map.prop_data[holes[i].y * current_map.width + holes[i].x] = holes[i].type;
			hole_delete(i);
		}
	}
}
