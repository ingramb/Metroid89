// C Source File
// Created 1/17/02; 11:01:07 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "extgraph.h"
#include "utility.h"
#include "tiles.h"
#include "entity.h"
#include "enemy.h"
#include "door.h"
#include "pipe.h"
#include "map.h"
#include "player.h"
#include "shots.h"
#include "bomb.h"
#include "animation.h"
#include "bar.h"
#include "blur.h"
#include "block.h"
#include "powerup.h"
#include "environment.h"
#include "system.h"
#include "miscgfx.h"
#include "clipsprites.h"
#include "globals.h"

extern void screen_draw();

char game_name[9];

char item_flash;
char item_flash_counter;

const unsigned char map_room_table[] = {
	85,
	86, 86,
	87, 87,
	88, 88, 88, 88,
	89, 89,
	90, 90, 90, 90,
	91, 91, 91, 91,
	92, 92, 92, 92, 92, 92, 92, 92,
	93, 93,
	94, 94, 94, 94,
	95, 95, 95, 95,
	96, 96, 96, 96, 96, 96, 96, 96,
	97, 97, 97, 97,
	98, 98, 98, 98, 98, 98, 98, 98,
	99, 99, 99, 99, 99, 99, 99, 99,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	81,
	101, 102, 103,
};

/*char cnt_table[][] = {
	{16, 4,  8,  12, 16, 4,  8,  12, 16, 4,  8,  12, 16, 4,  8,  12, 16, 4},
	{17, 5,  9,  13, 1,  5,  9,  13, 1,  5,  9,  13, 1,  5,  9,  13, 1,  5},
	{18, 6,  10, 14, 2,  6,  10, 14, 2,  6,  10, 14, 2,  6,  10, 14, 2,  6},
	{19, 7,  11, 15, 3,  7,  11, 15, 3,  7,  11, 15, 3,  7,  11, 15, 3,  7},
	{20, 8,  12, 16, 4,  8,  12, 16, 4,  8,  12, 16, 4,  8,  12, 16, 4,  8},
	{21, 9,  13, 1,  5,  9,  13, 1,  5,  9,  13, 1,  5,  9,  13, 1,  5,  9},
	{22, 10, 14, 2,  6,  10, 14, 2,  6,  10, 14, 2,  6,  10, 14, 2,  6,  10},
	{23, 11, 15, 3,  7,  11, 15, 3,  7,  11, 15, 3,  7,  11, 15, 3,  7,  11},
	{24, 12, 16, 4,  8,  12, 16, 4,  8,  12, 16, 4,  8,  12, 16, 4,  8,  12},
	{25, 13, 1,  5,  9,  13, 1,  5,  9,  13, 1,  5,  9,  13, 1,  5,  9,  13},
	{26, 14, 2,  6,  10, 14, 2,  6,  10, 14, 2,  6,  10, 14, 2,  6,  10, 14},
	{27, 15, 3,  7,  11, 15, 3,  7,  11, 15, 3,  7,  11, 15, 3,  7,  11, 15}
};*/

#define BG_WAVE_DELAY 5
#define BG_RIPPLE_DELAY 12

ZONE_HEADER *zone_header;
char *zone_name;
MAP_HEADER *map_header;
//unsigned char *global_map;
short global_map_width;
short global_map_hieght;
short map_number;
short screen_number;
short zone_number;
#define MAPFILE_MAX 10
void *map_data[MAPFILE_MAX];

ITEM *item_data;
short item_number;
short saved_enemy_number;
short saved_door_number;
short current_item_number;

unsigned char items_found[FOUND_ITEM_SIZE];

const char* const item_names[] = {
	"CHARGE",
	"ICE BEAM",
	"WAVE",
	"SPAZER",
	"PLASMA",
	"VARIA SUIT",
	"GRAVITY SUIT",
	"MORPHING BALL",
	"BOMB",
	"RUBBER BALL",
	"SPRING BALL",
	"HI-JUMP BOOTS",
	"SPEED BOOTS",
	"SPACE JUMP",
	"SCREW ATTACK",
	"HEAT SEAKERS",
	"ENERGY LEECH",
	"FORCE SHIELD",
	"REVERSE GRAVITY",
};

const unsigned char special_gfx_ext[54] = {
	//sprite 0
	0xe1,0xbf,0xa6,0xf9,0xa6,0xf9,0xa6,0xbf,0xe1,
	0xe1,0xff,0xff,0xa6,0xa0,0xa6,0xff,0xff,0xe1,
	0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,
	//sprite 1
	0x87,0xfd,0x65,0x9f,0x65,0x9f,0x65,0xfd,0x87,
	0x87,0xff,0xff,0x65,0x05,0x65,0xff,0xff,0x87,
	0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,
};

/*
//=========================================================================================
//START OF TEST

char *bg_light;
char *bg_dark;
short old_bg_x;

const short TEST_bg_width = 8;
const short TEST_bg_hieght = 4;

const unsigned char TEST_bg_map[] = {
	1, 0, 1, 2, 1, 0, 1, 1,
	2, 1, 0, 1, 0, 1, 2, 3,
	1, 0, 0, 0, 0, 0, 1, 2,
	0, 0, 0, 1, 0, 0, 0, 1,
};

void scroll_left(unsigned short* buffer,unsigned short lines) {
    register short* tmpbuffer = buffer;
    register short  tmplines  = lines;

    tmpbuffer += (tmplines<<3) + (tmplines<<2);
    tmplines--;

    asm volatile ("0:\n"
        "lsl.w  -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0)\n"
        "roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0)\n"
        "dbf %1,0b"
        : "=a" (tmpbuffer), "=d" (tmplines)
        : "0"  (tmpbuffer), "1"  (tmplines));
}

void scroll_right(unsigned short* buffer,unsigned short lines) {
    register short* tmpbuffer = buffer;
    register short  tmplines  = lines;

    tmplines--;

    asm volatile ("0:\n"
        "lsr.w  (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+\n"
        "roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+\n"
        "dbf %1,0b"
        : "=a" (tmpbuffer), "=d" (tmplines)
        : "0"  (tmpbuffer), "1"  (tmplines));
}

void sprite_fill(short col)
{
	short *light, *dark, *sprite0, *sprite1;
	short i, y;
	short map_pos = glbs->camera.bg_tile_x + col - 1;

	while(map_pos >= TEST_bg_width) map_pos -= TEST_bg_width;
	while(map_pos < 0) map_pos += TEST_bg_width;
	//map_pos += glbs->camera.bg_tile_y * TEST_bg_width;

	light = (short *)bg_light + col;
	dark = (short *)bg_dark + col;

	for(y = 0 ; y < TEST_bg_hieght ; y++, map_pos += TEST_bg_width) {
		sprite0 = (short *)(bg_tile + 64 * TEST_bg_map[map_pos]);
		sprite1 = sprite0 + 16;
		for(i = 0 ; i < 16 ; i++) {
			*light = *sprite0++;
			*dark = *sprite1++;
			light = (void *)((char *)light + (24)); dark = (void *)((char *)dark + (24));
		}
	}
}

void TEST_bg_setup()
{
	short x;

	bg_light = malloc(TEST_bg_hieght * 16 * 24 * 2);
	bg_dark = bg_light + TEST_bg_hieght * 16 * 24;

	for(x = 0 ; x < 12 ; x++) sprite_fill(x);
	for(x = 0 ; x < glbs->camera.bg_x_off ; x++) {
		scroll_left((short *)bg_light, TEST_bg_hieght * 16);
		scroll_left((short *)bg_dark, TEST_bg_hieght * 16);
	}

	old_bg_x = 0;
}

void TEST_bg_draw()
{
	short i = 0, y = 0;
	long *light_src, *dark_src, *light, *dark;

	if(old_bg_x != glbs->camera.bg_x_off) {

		if(glbs->camera.bg_x_moved > 0) {
			for(i = 0 ; i < glbs->camera.bg_x_moved ; i++) {
				scroll_left((short *)bg_light, TEST_bg_hieght * 16);
				scroll_left((short *)bg_dark, TEST_bg_hieght * 16);
				old_bg_x++; if(old_bg_x > 15) old_bg_x = 0;
				if(old_bg_x == 0) sprite_fill(11);
			}
		} else {
			for(i = 0 ; i < -glbs->camera.bg_x_moved ; i++) {
				scroll_right((short *)bg_light, TEST_bg_hieght * 16);
				scroll_right((short *)bg_dark, TEST_bg_hieght * 16);
				old_bg_x--; if(old_bg_x < 0) old_bg_x = 15;
				if(old_bg_x == 0) sprite_fill(0);
			}
		}
		glbs->camera.bg_x_moved = 0;
		old_bg_x = glbs->camera.bg_x_off;
	}

	i = 16 * glbs->camera.bg_tile_y + glbs->camera.bg_y_off;
	light = glbs->light_buffer; dark = glbs->dark_buffer;
	light_src = (long *)(bg_light + i * 24 + 2);
	dark_src = (long *)(bg_dark + i * 24 + 2);
	while(y < 100) {
		while(i < TEST_bg_hieght * 16 && y < 100) {
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light = *light_src; *dark = *dark_src;
			light = (void *)((char *)light + (14)); dark = (void *)((char *)dark + (14));
			light_src = (void *)((char *)light_src + (8)); dark_src = (void *)((char *)dark_src + (8));
			i++; y++;
		}
		i = 0; light_src = (long *)(bg_light + 2); dark_src = (long *)(bg_dark + 2);
	}
}

void TEST_bg_cleanup()
{
	free(bg_light);
}

//=========================================================================================
//END OF TEST
*/

/*unsigned short rle_compress(unsigned char *src, unsigned char *dst, unsigned short size)
{
	unsigned short rle_size = 0;
	unsigned short i = 0;
	unsigned char run_length;
	unsigned char run_char;
	unsigned char flag = 255;

	while(i < size){
		run_length = 1;
		run_char = *src;

		while(*(++src) == run_char && run_length < 255){
			run_length++;
			if(i + run_length >= size) break;
		}

		if(run_length == 2){
			run_length--;
			src--;
		}

		if(run_length > 1 || run_char == flag){
			*dst++ = flag;
			*dst++ = run_char;
			*dst++ = run_length;
			rle_size += 3;
		} else {
			*dst++ = run_char;
			rle_size++;
		}

		i += run_length;
	}

	return rle_size;
}

void rle_decompress(unsigned char *scr, unsigned char *dst, unsigned short size)
{
	unsigned short i = 0;
	short j;
	unsigned char flag = 255;
	unsigned char run_char;
	unsigned char run_length;

	while(i < size){
		run_char = *scr++;

		if(run_char == flag){
			run_char = *scr++;
			run_length = *scr++;
		} else
			run_length = 1;

		for(j = 0 ; j < run_length ; j++)
			*dst++ = run_char;

		i += run_length;
	}
}*/

void CHECKPOINT(short i)
{
	clrscr();
	printf("CHECKPOINT%d", i);
	pause();
}

void put_screen(SCREEN_HEADER *hdr, SCREEN_DATA *screen, short width)
{
	short x, y;
	unsigned char tile;
	char prop;

	for(y = 0 ; y < SCREEN_HIEGHT ; y++) {
		memcpy(glbs->current_map.data + (hdr->y_pos * SCREEN_HIEGHT + y) * width + hdr->x_pos * SCREEN_WIDTH,
			screen->data + y * SCREEN_WIDTH, SCREEN_WIDTH);

		for(x = 0 ; x < SCREEN_WIDTH ; x++) {
			tile = screen->data[y * SCREEN_WIDTH + x];
			prop = screen->prop_data[y * SCREEN_WIDTH + x];

			if(prop < 0)
				glbs->current_map.prop_data[(hdr->y_pos * SCREEN_HIEGHT + y) * width +
					hdr->x_pos * SCREEN_WIDTH + x] = glbs->tile_data[tile].hit;
			else {
				if(prop > SLOPE_LEFT_DOWN) prop = (prop - SLOPE_LEFT_DOWN) | 0x80;
				glbs->current_map.prop_data[(hdr->y_pos * SCREEN_HIEGHT+ y) * width +
					hdr->x_pos * SCREEN_WIDTH + x] = prop;
			}
		}
	}

	glbs->current_map.screen_data[hdr->y_pos * glbs->current_map.screen_width + hdr->x_pos].scroll_flags =
		hdr->scroll_flags;
}

char map_setup(char *map_file)
{
	short i;
	short file_number;
	char current_file[10];
	unsigned short map_offset;
	unsigned short zone_offset;

	strcpy(current_file, map_file);
	memset(map_data, 0, sizeof(void *) * MAPFILE_MAX);

	map_data[0] = file_pointer(SYMSTR(map_file));
	if(map_data[0] == NULL) return FALSE;

	file_number = *(short *)(map_data[0] + 0);
	zone_number = *(short *)(map_data[0] + 2);
	map_number = *(short *)(map_data[0] + 4);
	screen_number = *(short *)(map_data[0] + 6);
	item_number = *(short *)(map_data[0] + 8);
	saved_enemy_number = *(short *)(map_data[0] + 10);
	saved_door_number = *(short *)(map_data[0] + 12);
	zone_offset = *(short *)(map_data[0] + 14);
	map_offset = *(short *)(map_data[0] + 16);

	map_header = (MAP_HEADER *)(map_data[0] + map_offset);
	zone_header = (ZONE_HEADER *)(map_data[0] + zone_offset);

	glbs->bg_list = (BACKGROUND_HEADER *)(glbs->metpack_base + glbs->metpack_header->bg_list_offset);

	current_file[strlen(map_file)] = '1';
	current_file[strlen(map_file) + 1] = 0;
	for(i = 1 ; i < file_number ; i++) {
		//clrscr();
		//printf(current_file);
		//pause();

		map_data[i] = file_pointer(SYMSTR(current_file));
		if(map_data[i] == NULL) return FALSE;
		current_file[strlen(map_file)]++;
	}

	//for(i = 0 ; i < 6 ; i++) {
	//ClrScr();
	//printf_xy(0, 0, "%ld %ld", metdata_offset, (unsigned long)metdata->jump_table[i]);
	//pause();
	//}

	/*for(i = 0 ; i < map_number ; i++) {
	ClrScr();
	printf_xy(0, 0, "width:%d hieght:%d", map_header[i].width, map_header[i].hieght);
	printf_xy(0, 8, "Enum:%d", map_header[i].enemy_number);
	printf_xy(0, 16, "Dnum:%d", map_header[i].door_number);
	printf_xy(0, 24, "Doff:%d", map_header[i].door_offset);
	printf_xy(0, 32, "Eoff:%d", map_header[i].enemy_offset);
	printf_xy(0, 40, "Soff:%d", map_header[i].screen_offset);
	//printf_xy(0, 48, "Compsize:%d", ttunpack_size(map_data + map_header[i].offset));
	printf_xy(0, 56, "Snum:%d", map_header[i].screen_number);
	ngetchx();
	}*/

	//tile_list = malloc(sizeof(TILE_NODE) * TILE_LIST_SIZE);
	//if(tile_list == NULL) return FALSE;
	//tile_list_clipped = malloc(sizeof(TILE_NODE_CLIPPED) * TILE_LIST_CLIPPED_SIZE);
	//if(tile_list_clipped == NULL) {
	//	free(tile_list);
	//	tile_list = NULL;
	//	return FALSE;
	//}

	zone_name = "";   // was NULL; set_zone strcmp's it (NULL deref is fatal on host, not on calc)
	//memset(&current_map, 0, sizeof(CURRENT_MAP));
	glbs->current_map.data = NULL;
	glbs->current_map.prop_data = NULL;
	glbs->current_map.screen_data = NULL;

	item_flash = 0;
	item_flash_counter = 0;

	memset(items_found, 0, sizeof(char) * FOUND_ITEM_SIZE);
	memset(opened_doors, 0, sizeof(char) * OPENED_DOORS_SIZE);
	memset(killed_enemies, 0, sizeof(char) * KILLED_ENEMIES_SIZE);

	return TRUE;
}

void map_cleanup()
{
	short i;

	//if(tile_list != NULL) {
	//	free(tile_list);
	//	tile_list = NULL;
	//}
	//if(tile_list_clipped != NULL) {
	//	free(tile_list_clipped);
	//	tile_list_clipped = NULL;
	//}
	map_unload();

	for(i = 0 ; i < MAPFILE_MAX ; i++) {
		if(map_data[i] != NULL) HeapUnlock(HeapPtrToHandle(map_data[i]));
	}
}

void map_unload()
{
	if(glbs->current_map.data != NULL) {
		free(glbs->current_map.data);
		glbs->current_map.data = NULL;
	}
	if(glbs->current_map.prop_data != NULL) {
		free(glbs->current_map.prop_data);
		glbs->current_map.prop_data = NULL;
	}
	if(glbs->current_map.screen_data != NULL) {
		free(glbs->current_map.screen_data);
		glbs->current_map.screen_data = NULL;
	}
}

void map_room_active(short z, unsigned short room)
{
	SCREEN_HEADER *hdr;
	short m, i, x, y;

	for(m = 0 ; m < map_number ; m++) {
		if(map_header[m].zone != z) continue;
		hdr = map_data[map_header[m].file] + map_header[m].screen_offset;
		for(i = 0 ; i < map_header[m].screen_number ; i++) {
		//ClrScr();printf_xy(0, 0, "%u", map_header[m].screen_offset);pause();
			if(hdr->map_room == room) {
				x = map_header[m].x_pos + hdr->x_pos;
				y = map_header[m].y_pos + hdr->y_pos;
				if(explored_map[z][y * zone_header[z].width + x] == 81)
					explored_map[z][y * zone_header[z].width + x] = map_room_table[hdr->map_tile & 0x7f];
			}
			hdr++;
		}
	}
}

void set_zone(short z)
{
	if(strcmp(zone_name, zone_header[z].name) == 0) return;

	global_map_width = zone_header[z].width;
	global_map_hieght = zone_header[z].hieght;
	//global_map = (map_data + zone_header[z].offset);
	zone_name = zone_header[z].name;
	zone_name_activate();
}

void set_map(short m)
{
	SCREEN_DATA *screens;
	void *packed_data;
	//DOOR_HEADER *dheader = map_data + map_header[m].door_offset;
	short i;
	short x, y;
	short file = map_header[m].file;
	//unsigned char tile;

	if(m >= map_number || m < 0) return;

	enemy_reset(map_data[file] + map_header[m].enemy_offset, map_header[m].enemy_number);
	door_reset();
	pipe_reset();
	shot_reset();
	bomb_reset();
	animation_reset();
	hole_reset();
	blur_reset();
	block_reset();
	powerup_reset();

	map_unload();

	//ClrScr();
	//printf_xy(0, 8, "AVAIL:%lu", HeapAvail());
	//printf_xy(0, 16, "BLOCK:%lu", HeapMax());
	//pause();

	glbs->current_map.index = m;
	glbs->current_map.zone = map_header[m].zone;
	glbs->current_map.width = map_header[m].width * SCREEN_WIDTH;
	glbs->current_map.hieght = map_header[m].hieght * SCREEN_HIEGHT;
	glbs->current_map.x_pos = map_header[m].x_pos;
	glbs->current_map.y_pos = map_header[m].y_pos;
	glbs->current_map.background = map_header[m].background;
	glbs->current_map.bg_wave = map_header[m].bg_wave;
	glbs->current_map.special_x = map_header[m].special_x;
	glbs->current_map.special_y = map_header[m].special_y;
	glbs->current_map.map_room = map_header[m].map_room;
	glbs->current_map.special_type = map_header[m].special_type;
	glbs->current_map.screen_width = map_header[m].width;
	glbs->current_map.screen_hieght = map_header[m].hieght;
	glbs->current_map.screen_number = map_header[m].screen_number;

	//clrscr();
	//printf("screen_offset:%d", map_header[m].file);
	//printf("hieght:%d", glbs->current_map.hieght);
	//pause();

	set_zone(map_header[m].zone);

	tileset_load(map_header[m].tileset, map_header[m].load_universal);

	glbs->current_map.data = malloc(glbs->current_map.width * glbs->current_map.hieght);
	glbs->current_map.prop_data = malloc(glbs->current_map.width * glbs->current_map.hieght);
	glbs->current_map.screen_data =
		malloc(glbs->current_map.screen_width * glbs->current_map.screen_hieght * sizeof(SCREEN_TILE));
	memset(glbs->current_map.data, 1, glbs->current_map.width * glbs->current_map.hieght);
	memset(glbs->current_map.prop_data, SOLID, glbs->current_map.width * glbs->current_map.hieght);
	memset(glbs->current_map.screen_data, 0,
		glbs->current_map.screen_width * glbs->current_map.screen_hieght * sizeof(SCREEN_TILE));

	item_data = map_data[file] + map_header[m].item_offset;
	item_number = map_header[m].item_number;

	//ClrScr();
	//printf_xy(0, 8, "AVAIL:%lu", HeapAvail());
	//printf_xy(0, 16, "BLOCK:%lu", HeapMax());
	//pause();

	glbs->current_map.screen_header = map_data[file] + map_header[m].screen_offset;
	packed_data = glbs->current_map.screen_header + map_header[m].screen_number;
	screens = malloc(ttunpack_size(packed_data));
	i = decompress(packed_data, (char *)screens);

	for(i = 0 ; i < map_header[m].screen_number ; i++) {

		//ClrScr();
		//printf_xy(0, 0, "x:%d y:%d m:%d",
		//	glbs->current_map.screen_header[i].x_pos,
		//	glbs->current_map.screen_header[i].y_pos,
		//	glbs->current_map.screen_header[i].map_tile);
		//pause();

		put_screen(glbs->current_map.screen_header + i, screens + i, glbs->current_map.width);
	}

	free(screens);

	memset(&glbs->special, 0, sizeof(MAP_SPECIAL));
	if(glbs->current_map.special_type == SPECIAL_SAVE) {
		set_prop_data(glbs->current_map.special_x, glbs->current_map.special_y, SOLID);
		set_prop_data(glbs->current_map.special_x + 1, glbs->current_map.special_y, SOLID);
		set_prop_data(glbs->current_map.special_x, glbs->current_map.special_y + 5, SOLID);
		set_prop_data(glbs->current_map.special_x + 1, glbs->current_map.special_y + 5, SOLID);
	} else if(glbs->current_map.special_type > SPECIAL_SAVE) {
		for(x = glbs->current_map.special_x ; x < glbs->current_map.special_x + 2 ; x++)
		for(y = glbs->current_map.special_y ; y < glbs->current_map.special_y + 3 ; y++)
			set_prop_data(x, y, SOLID);
	}

	//current_item_number = 0;

	/*for(i = 0 ; i < item_number ; i++) {
		if(item_data[i].map != m) continue;

		ClrScr();
		printf_xy(0, 0, "map:%d", item_data[i].map);
		printf_xy(0, 8, "x:%d", item_data[i].x_pos);
		printf_xy(0, 16, "y:%d", item_data[i].y_pos);
		printf_xy(0, 24, "item:%d", item_data[i].item);
		printf_xy(0, 32, "hidden:%d", item_data[i].hidden_flag);
		pause();

		if(items_found[i / 8] & (1 << (i % 8))) continue;

		x = item_data[i].x_pos;
		y = item_data[i].y_pos;
		tile = item_data[i].item;
		if(item_data[i].hidden_flag) {
			glbs->current_map.prop_data[y * glbs->current_map.width + x] = tile;
		} else {
			glbs->current_map.data[y * glbs->current_map.width + x] = tile;
			glbs->current_map.prop_data[y * glbs->current_map.width + x] = BACKGROUND;
		}
		current_item_number++;
	}*/

	enemy_init(map_data[file] + map_header[m].enemy_offset, map_header[m].enemy_number);
	door_init(map_data[file] + map_header[m].door_offset, map_header[m].door_number);
	pipe_init(map_data[file] + map_header[m].pipe_offset, map_header[m].pipe_number);

	environment_setup(map_header[m].environment, map_header[m].water_type,
		map_header[m].water_start, map_header[m].water_end, map_header[m].water_speed);

	/*for(i = 0 ; i < glbs->current_map.door_number ; i++) {
		PortSet(GetPlane(1), 239, 99);
		ClrScr();
		printf_xy(0, 0, "x:%d, y:%d", doors[i].x, doors[i].y);
		printf_xy(0, 8, "type:%d", doors[i].type);
		printf_xy(0, 16,"target_door:%d", doors[i].target_door);
		printf_xy(0, 24,"target_map:%d", doors[i].target_map);
		pause();
	}*/

	/*for(i = 0 ; i < glbs->current_map.enemy_number ; i++) {
		PortSet(GetPlane(1), 239, 99);
		ClrScr();
		printf_xy(0, 0, "x:%d, y:%d", glbs->current_map.enemies[i].e.x, glbs->current_map.enemies[i].e.y);
		pause();
	}*/

	//PortSet(GetPlane(1), 239, 99);
	//ClrScr();
	//printf_xy(0, 0, "%d", glbs->current_map.enemy_number);
	//pause();

	glbs->game_counter = 1;
}

void check_items()
{
	short i;
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short pw = glbs->player.e.width;
	short ph = glbs->player.e.hieght;
	unsigned char item;
	short index;

	for(i = 0 ; i < item_number ; i++) {
		index = item_data[i].index;
		if(items_found[index / 8] & (1 << (index % 8)) ||
			get_data(item_data[i].x, item_data[i].y) != EMPTY) continue;
		if(rect_overlap(px, py, pw, ph, item_data[i].x * 12 - 1, item_data[i].y * 12 - 1, 14, 14)) {

			load_metsys();
			item = item_data[i].type;

			if(item == 0) {
				glbs->player.ammunition_max[0] += 5;
				glbs->player.ammunition[0] += 5;
				message("MISSILES", glbs->font, 11);
			} else if(item == 1) {
				glbs->player.ammunition_max[1] += 5;
				glbs->player.ammunition[1] += 5;
				message("SUPER MISSILES", glbs->font, 11);
			} else if(item == 2) {
				glbs->player.ammunition_max[2] += 5;
				glbs->player.ammunition[2] += 5;
				message("POWER BOMBS", glbs->font, 11);
			} else if(item == 3) {
				glbs->player.hp_max += 100;
				glbs->player.hp = glbs->player.hp_max;
				message("ENERGY TANK", glbs->font, 11);
			} else if(item == 4) {
				message("METROID DNA", glbs->font, 11);
			}	else {
				glbs->player.items_found |= (1L << (unsigned long)(item - 5));
				glbs->player.items_equiped |= (1L << (unsigned long)(item - 5));
				message(item_names[item - 5], glbs->font, 11);
			}
			pause();

			items_found[index / 8] |= (1 << (index % 8));
			load_metgame();
			bar_update();
		}
	}
}

void draw_items()
{
	short i, index;

	for(i = 0 ; i < item_number ; i++) {
		index = item_data[i].index;
		if(items_found[index / 8] & (1 << (index % 8)) ||
			get_data(item_data[i].x, item_data[i].y) != EMPTY) continue;
		GraySpriteClipX8_MASK(item_data[i].x * 12 - glbs->camera.x, item_data[i].y * 12 - glbs->camera.y,
			12, glbs->item_gfx + item_data[i].type * 72, 2, item_flash);
	}
}

void draw_special(char p)
{
	unsigned char *frame;
	short i;
	short xpos = glbs->current_map.special_x;
	short ypos = glbs->current_map.special_y;

	if(glbs->current_map.special_type == SPECIAL_SAVE) {
		if(p == 0) {
			for(i = ypos + 1 ; i < ypos + 5 ; i++)
				GraySpriteClipX8_MASK(xpos * 12 - glbs->camera.x, i * 12 - glbs->camera.y, 12,
					glbs->save_room_gfx + 108 + (glbs->special.save.frame & 1) * 108 * 3, 3, FALSE);
		} else {
			GraySpriteClipX8_MASK(xpos * 12 - glbs->camera.x, ypos * 12 - glbs->camera.y, 12,
				glbs->save_room_gfx + (glbs->special.save.frame & 1) * 108 * 3, 3, FALSE);
			GraySpriteClipX8_MASK(xpos * 12 - glbs->camera.x, ypos * 12 + 60 - glbs->camera.y, 12,
				glbs->save_room_gfx + 216 + (glbs->special.save.frame & 1) * 108 * 3, 3, FALSE);
			if(glbs->special.save.saving)
				GraySpriteClipX8_MASK(xpos * 12 - glbs->camera.x,
				(ypos + glbs->special.save.frame) * 12 + 12 - glbs->camera.y, 12,
				glbs->save_room_gfx + 108 * 6, 3, FALSE);
		}
	} else if(glbs->current_map.special_type > SPECIAL_SAVE && p == 1) {
		GraySpriteClipX8_MASK(xpos * 12 - 4 - glbs->camera.x, ypos * 12 - 2 - glbs->camera.y, 38,
			glbs->special_gfx_base, 4, FALSE);
		frame = glbs->special_gfx_anim + glbs->special.other.frame * 16;;
		if(glbs->current_map.special_type == SPECIAL_MAP) frame += 48;
		GraySpriteClip8_OR(xpos * 12 + 8 - glbs->camera.x, ypos * 12 + 12 - glbs->camera.y, 8, frame);
		if(glbs->special.other.counter > 0) {
			if(glbs->player.e.x < xpos * 12)
				GraySpriteClip8_MASK(xpos * 12 - 10 - glbs->camera.x, ypos * 12 + 11 - glbs->camera.y, 9,
					special_gfx_ext, glbs->light_buffer, glbs->dark_buffer);
			else
				GraySpriteClip8_MASK(xpos * 12 + 27 - glbs->camera.x, ypos * 12 + 11 - glbs->camera.y, 9,
					special_gfx_ext + 27, glbs->light_buffer, glbs->dark_buffer);
		}
	}
}

void check_special()
{
	MENU *m;
	short i;
	short xpos = glbs->current_map.special_x;
	short ypos = glbs->current_map.special_y;
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short pw = glbs->player.e.width;
	short ph = glbs->player.e.hieght;
	short left_thresh = xpos * 12 - 12 - pw;
	short right_thresh = xpos * 12 + 36;

	if(glbs->current_map.special_type == SPECIAL_SAVE) {
		if(glbs->special.save.saving) {
			glbs->special.save.frame_counter++;
			glbs->special.save.counter--;
			if(glbs->special.save.frame_counter == 6) {
				glbs->special.save.frame_counter = 0;
				glbs->special.save.frame++;
				if(glbs->special.save.frame == 6) glbs->special.save.frame = 0;
			}
			if(glbs->special.save.counter == 3) {
				glbs->special.save.saving = FALSE;
				glbs->special.save.frame = 0;
				player_set_action(ACT_FRONT_READY);
				screen_draw();
				load_metsys();
				message("SAVE COMPLETE", glbs->font, 11);
				load_metgame();
				pause();
			}
		}
		if(glbs->special.save.active < 2 &&
			px >= xpos * 12 && px + pw <= xpos * 12 + 24 && py >= ypos * 12 && py + ph <= ypos * 12 + 72) {
				if(glbs->special.save.active == 0) {
					glbs->special.save.active = 1;
					glbs->special.save.counter = 30;
				}
				glbs->special.save.counter--;
				if(glbs->special.save.counter == 0) {
					load_metsys();
					message("SAVE GAME?", glbs->font, 21);
					m = menu_create(2);
					menu_add_choice(m, 58, 48, BUTTON_DISABLE, "YES");
					menu_add_choice(m, 81, 48, BUTTON_DISABLE, "NO");
					menu_process(m, glbs->font);
					load_metgame();
					if(m->selected == 0) {
						glbs->special.save.saving = TRUE;
						glbs->special.save.counter = 120;
						player_set_action(ACT_FRONT_READY);
						player_set_position(xpos * 12 - 4, glbs->player.e.y);
						game_write(&glbs->respawn_point);
						player_set_action(ACT_FRONT_WAIT);
						bar_update();
						// Native port: commit the save to disk now (was only on
						// clean quit) so a crash can't lose it.
						game_save();
					}
					free(m);
					glbs->special.save.active = 2;
				}
		} else if(glbs->special.save.active == 1) glbs->special.save.active = 0;
	} else if(glbs->current_map.special_type > SPECIAL_SAVE) {
		glbs->special.other.frame_counter++;
		if(glbs->special.other.frame_counter == 7) {
			glbs->special.other.frame_counter = 0;
			glbs->special.other.frame++;
			if(glbs->special.other.frame == 3) glbs->special.other.frame = 0;
		}
		if(glbs->special.other.counter == 0 && px >= left_thresh && px <= right_thresh &&
			py + ph - 1 == ypos * 12 + 35) {
				glbs->special.other.counter = 120;
				player_set_action(ACT_SIDE_WAIT);
				if(px < xpos * 12) {
					player_set_position(left_thresh - glbs->player.e.x_off, glbs->player.e.y);
					glbs->player.direction = RIGHT;
				} else {
					player_set_position(right_thresh - glbs->player.e.x_off, glbs->player.e.y);
					glbs->player.direction = LEFT;
				}
		} else if(glbs->special.other.counter > 0) {
			glbs->special.other.counter--;
			if(glbs->special.other.counter == 0) {
				glbs->special.other.counter = -1;
				player_set_action(ACT_STAND);
				if(glbs->current_map.special_type == SPECIAL_RECHARGE) {
					glbs->player.hp = glbs->player.hp_max;
					for(i = 0 ; i < 3 ; i++) glbs->player.ammunition[i] = glbs->player.ammunition_max[i];
					bar_update();
					load_metsys();
					message("RECHARGE COMPLETE", glbs->font, 11);
					load_metgame();
					pause();
				} else {
					map_room_active(glbs->current_map.zone, glbs->current_map.map_room);
					map_update();
					load_metsys();
					message("MAP DATA DOWNLOADED", glbs->font, 11);
					load_metgame();
					pause();
					status_screen();
				}

			}
		}
	}
}

/*void draw_special()
{
	short x = glbs->current_map.special_x * 12;
	short y = glbs->current_map.special_y * 12;
	unsigned char *sprite;

	if(glbs->current_map.special_type == SPECIAL_SAVE_ROOM) {
		sprite = */


void bg_horiz(short d)
{
	glbs->camera.bg_x_counter += d;
	while(glbs->camera.bg_x_counter <= -100) {
		glbs->camera.bg_x_counter += 100;
		glbs->camera.bg_x_off--;
		glbs->camera.bg_x_moved--;
		if(glbs->camera.bg_x_off < 0) {
			glbs->camera.bg_x_off = 15;
			glbs->camera.bg_tile_x--;
			if(glbs->camera.bg_tile_x < 0)
				glbs->camera.bg_tile_x = glbs->bg_list[glbs->current_map.background].width - 1;
		}
	}
	while(glbs->camera.bg_x_counter >= 100) {
		glbs->camera.bg_x_counter -= 100;
		glbs->camera.bg_x_off++;
		glbs->camera.bg_x_moved++;
		if(glbs->camera.bg_x_off > 15) {
			glbs->camera.bg_x_off = 0;
			glbs->camera.bg_tile_x++;
			if(glbs->camera.bg_tile_x >= glbs->bg_list[glbs->current_map.background].width)
				glbs->camera.bg_tile_x = 0;
		}
	}
}

void bg_vert(short d)
{
	glbs->camera.bg_y_counter += d;
	while(glbs->camera.bg_y_counter <= -100) {
		glbs->camera.bg_y_counter += 100;
		glbs->camera.bg_y_off--;
		if(glbs->camera.bg_y_off < 0) {
			glbs->camera.bg_y_off = 15;
			glbs->camera.bg_tile_y--;
			if(glbs->camera.bg_tile_y < 0)
				glbs->camera.bg_tile_y = glbs->bg_list[glbs->current_map.background].hieght - 1;
		}
		glbs->camera.bg_wave = (glbs->camera.bg_wave - 1) & 15;
	}
	while(glbs->camera.bg_y_counter >= 100) {
		glbs->camera.bg_y_counter -= 100;
		glbs->camera.bg_y_off++;
		if(glbs->camera.bg_y_off > 15) {
			glbs->camera.bg_y_off = 0;
			glbs->camera.bg_tile_y++;
			if(glbs->camera.bg_tile_y >= glbs->bg_list[glbs->current_map.background].hieght)
				glbs->camera.bg_tile_y = 0;
		}
		glbs->camera.bg_wave = (glbs->camera.bg_wave + 1) & 15;
	}
}

void focus_camera()
{
	short x = glbs->player.e.x - 64;
	short y = glbs->player.e.y - 20;
	short i;

	if(x < 0) x = 0;
	if(y < 0) y = 0;

	memset(&glbs->camera, 0, sizeof(CAMERA));

	glbs->camera.x = x;
	glbs->camera.y = y;
	glbs->camera.tile_x = x / 12;
	glbs->camera.tile_y = y / 12;
	glbs->camera.x_off = x % 12;
	glbs->camera.y_off = y % 12;

	for(i = 0 ; i < x ; i++) bg_horiz(glbs->bg_list[glbs->current_map.background].scroll_x);
	for(i = 0 ; i < y ; i++) bg_vert(glbs->bg_list[glbs->current_map.background].scroll_y);

	while(camera_process(100));

	bg_reset();
	bg_setup();

	//ClrScr();
	//printf_xy(0, 0, "x:%d y:%d", glbs->camera.x, glbs->camera.y);
	//printf_xy(0, 8, "bg_x:%d bg_y:%d", glbs->camera.bg_tile_x, glbs->camera.bg_tile_y);
	//printf_xy(0, 16, "bg_xo:%d bg_yo:%d", glbs->camera.bg_x_off, glbs->camera.bg_y_off);
	//pause();
}

char camera_process(char speed)
{
	char flags =
		glbs->current_map.screen_data[glbs->player.screen_y * glbs->current_map.screen_width + glbs->player.screen_x].scroll_flags;
	char direction = 0;
	short i;

	for(i = 0 ; i < speed &&
		glbs->camera.x > (glbs->player.screen_x + 1) * SCREEN_WIDTH * 12 - glbs->display_width && (flags & SCREEN_RIGHT) ; i++) {
			camera_left();
			direction = direction | LEFT;
	}
	for(i = 0 ; i < speed &&
		glbs->camera.x < glbs->player.screen_x * SCREEN_WIDTH * 12 && (flags & SCREEN_LEFT) ; i++) {
			camera_right();
			direction = direction | RIGHT;
	}
	for(i = 0 ; i < speed &&
		glbs->camera.y > (glbs->player.screen_y + 1) * SCREEN_HIEGHT * 12 - glbs->display_hieght && (flags & SCREEN_DOWN) ; i++) {
			camera_up();
			direction = direction | UP;
	}
	for(i = 0 ; i < speed &&
		glbs->camera.y < glbs->player.screen_y * SCREEN_HIEGHT * 12 && (flags & SCREEN_UP) ; i++) {
			camera_down();
			direction = direction | DOWN;
	}

	return direction;
}


void camera_process_left(short bound, char speed)
{
	short i = 0;
	char flags =
		glbs->current_map.screen_data[glbs->player.screen_y * glbs->current_map.screen_width + glbs->player.screen_x].scroll_flags;

	while(glbs->player.e.x - glbs->camera.x < bound && i < speed &&
		(glbs->camera.x != glbs->player.screen_x * SCREEN_WIDTH * 12 || !(flags & SCREEN_LEFT))) {
				camera_left();
				i++;
	}
}

void camera_process_right(short bound, char speed)
{
	short i = 0;
	char flags =
		glbs->current_map.screen_data[glbs->player.screen_y * glbs->current_map.screen_width + glbs->player.screen_x].scroll_flags;

	while(glbs->player.e.x - glbs->camera.x > bound && i < speed &&
		(glbs->camera.x != (glbs->player.screen_x + 1) * SCREEN_WIDTH * 12 - glbs->display_width || !(flags & SCREEN_RIGHT))) {
				camera_right();
				i++;
	}
}

void camera_process_up(short bound, char speed)
{
	short i = 0;
	char flags =
		glbs->current_map.screen_data[glbs->player.screen_y * glbs->current_map.screen_width + glbs->player.screen_x].scroll_flags;

	while(glbs->player.e.y - glbs->camera.y < bound && i < speed &&
		(glbs->camera.y != glbs->player.screen_y * SCREEN_HIEGHT * 12 || !(flags & SCREEN_UP))) {
				camera_up();
				i++;
	}
}

void camera_process_down(short bound, char speed)
{
	short i = 0;
	char flags =
		glbs->current_map.screen_data[glbs->player.screen_y * glbs->current_map.screen_width + glbs->player.screen_x].scroll_flags;

	while(glbs->player.e.y - glbs->camera.y > bound && i < speed &&
		(glbs->camera.y != (glbs->player.screen_y + 1) * SCREEN_HIEGHT * 12 - glbs->display_hieght || !(flags & SCREEN_DOWN))) {
				camera_down();
				i++;
	}
}

void camera_left()
{
	glbs->camera.x_off--;
	glbs->camera.x--;
	if(glbs->camera.x_off < 0) {
		glbs->camera.x_off = 11;
		glbs->camera.tile_x--;
	}

	bg_horiz(-glbs->bg_list[glbs->current_map.background].scroll_x);
}

void camera_right()
{
	glbs->camera.x_off++;
	glbs->camera.x++;
	if(glbs->camera.x_off > 11) {
		glbs->camera.x_off = 0;
		glbs->camera.tile_x++;
	}

	bg_horiz(glbs->bg_list[glbs->current_map.background].scroll_x);
}


void camera_up()
{
	glbs->camera.y_off--;
	glbs->camera.y--;
	if(glbs->camera.y_off < 0) {
		glbs->camera.y_off = 11;
		glbs->camera.tile_y--;
	}

	bg_vert(-glbs->bg_list[glbs->current_map.background].scroll_y);
}



void camera_down()
{
	glbs->camera.y_off++;
	glbs->camera.y++;
	if(glbs->camera.y_off > 11) {
		glbs->camera.y_off = 0;
		glbs->camera.tile_y++;
	}

	bg_vert(glbs->bg_list[glbs->current_map.background].scroll_y);
}

void map_process()
{
	item_flash_counter--;
	if(item_flash_counter < 0) {
		item_flash = !item_flash;
		item_flash_counter = 8 - item_flash * 6;
	}
	glbs->camera.bg_wave_counter++;
	if(glbs->camera.bg_wave_counter >= BG_WAVE_DELAY) {
		glbs->camera.bg_wave = (glbs->camera.bg_wave + 1) & 15;
		glbs->camera.bg_wave_counter = 0;
	}

	glbs->camera.bg_ripple_counter++;
	if(glbs->camera.bg_ripple_counter >= BG_RIPPLE_DELAY) {
		glbs->camera.bg_ripple = (glbs->camera.bg_ripple + 1) & 7;
		glbs->camera.bg_ripple_counter = 0;
	}

	bg_horiz(glbs->bg_list[glbs->current_map.background].auto_x);
	bg_vert(glbs->bg_list[glbs->current_map.background].auto_y);

	check_items();
	check_special();
}

/*void draw_background()
{
	TILE_NODE_CLIPPED *tile_ptr_clipped = tile_list_clipped;
	TILE_NODE *tile_ptr = tile_list;
	short x_pos;
	short y_pos;
	short x = glbs->camera.tile_x;
	short map_loc, bg_loc;
	short map_loc_start = glbs->camera.tile_y * glbs->current_map.width + glbs->camera.tile_x;
	short bg_loc_start = ((glbs->camera.tile_y & 15) << 4);
	unsigned char tile1;
	unsigned char tile2;
	short h;
	unsigned short* sprite0;
	unsigned short cnt;
	short screen_pos_start = (glbs->camera.tile_y >> 4) * glbs->current_map.screen_width + (glbs->camera.tile_x >> 4);
	short screen_pos;
	unsigned char bg;

	long dark;
	long light;
	long light2;
	long dark2;
	short offset;

	for(x_pos = -glbs->camera.x_off ; x_pos < glbs->display_width ; x++, x_pos += 12, map_loc_start++) {
		y_pos = -glbs->camera.y_off;
		offset = 0;
		screen_pos = screen_pos_start;

		if(x_pos < 0)
			cnt = -x_pos+16;
    else {
    	cnt = 16-(x_pos&15);
    	offset += ((x_pos>>3)&0x1e);
    }

    light = (long)glbs->light_buffer + offset;
    dark = (long)glbs->dark_buffer + offset;

    map_loc = map_loc_start;
    bg_loc = bg_loc_start + (x & 15);
    if((x & 15) == 15) screen_pos_start++;

    //=======================================================================
    //FIRST BLOCK (Clipping support)
    //=======================================================================
    bg = glbs->current_map.screen_data[screen_pos].background;
    tile1 = glbs->current_map.bg_data[bg_loc + (bg << 8)];
		tile2 = glbs->current_map.prop_data[map_loc];

		if(tile1 > 0 && !(tile2 & 0x80)) {
			sprite0 = (unsigned short*)(bg_tiles) + tile1 * 24 - 24 - (y_pos << 1);

			light2 = light;
			dark2 = dark;
			h = 12 + y_pos;

			for (; h ; h--, light2 += 30, dark2 += 30) {
				*(long*)light2|=(long)(*sprite0++)<<cnt;
				*(long*)dark2|=(long)(*sprite0++)<<cnt;
			}
		}

		tile1 = glbs->current_map.data[map_loc];
		tile2 = tile2 & 0x7f;

		if(tile1 > 0) {
			sprite0 = glbs->tile_data[tile1].data - y_pos * 3;

			h = 12 + y_pos;

			if(tile2 != BACKGROUND && tile2 != BACKGROUND_SOLID) {
				tile_ptr_clipped->tile = sprite0;
				tile_ptr_clipped->offset = offset;
				tile_ptr_clipped->cnt = cnt;
				tile_ptr_clipped->h = h;
				tile_ptr_clipped++;

				light += 30 * h;
				dark += 30 * h;
				offset += 30 * h;
			} else {
				offset += 30 * h;

				if(tile1 >= MAX_TILES && tile1 < MAX_TILES + MAX_ITEMS && item_flash) {
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt;
						sprite0++;
					}
				} else {
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
					}
				}
			}

		} else {
			light += 30 * (12 + y_pos);
			dark += 30 * (12 + y_pos);
			offset += 30 * (12 + y_pos);
		}

		y_pos += 12;
		map_loc += glbs->current_map.width;
		bg_loc += 16;
		if(bg_loc > 255) {
			bg_loc &= 255;
			screen_pos += glbs->current_map.screen_width;
		}

		//=======================================================================
    //MIDDLE BLOCKS (No clipping)
    //=======================================================================
		for( ; y_pos < glbs->display_hieght - 12 ; y_pos += 12, map_loc += glbs->current_map.width) {

			bg = glbs->current_map.screen_data[screen_pos].background;
    	tile1 = glbs->current_map.bg_data[bg_loc + (bg << 8)];
			tile2 = glbs->current_map.prop_data[map_loc];

			if(tile1 > 0 && !(tile2 & 0x80)) {
				sprite0 = (unsigned short*)(bg_tiles) + tile1 * 24 - 24;

				*(long*)(light + 0)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 0)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 30)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 30)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 60)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 60)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 90)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 90)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 120)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 120)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 150)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 150)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 180)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 180)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 210)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 210)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 240)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 240)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 270)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 270)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 300)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 300)|=(long)(*sprite0++)<<cnt;
				*(long*)(light + 330)|=(long)(*sprite0++)<<cnt;
				*(long*)(dark + 330)|=(long)(*sprite0)<<cnt;

			}

			tile1 = glbs->current_map.data[map_loc];

			if(tile1 > 0) {
				tile2 = tile2 & 0x7f;

				if(tile2 != BACKGROUND && tile2 != BACKGROUND_SOLID) {
					tile_ptr->tile = tile1;
					tile_ptr->offset = offset;
					tile_ptr->cnt = cnt;
					tile_ptr++;

					light += 30 * 12;
					dark += 30 * 12;
					offset += 30 * 12;
				} else {
					sprite0 = glbs->tile_data[tile1].data;
					offset += 30 * 12;

					if(tile1 >= MAX_TILES && tile1 < MAX_TILES + MAX_ITEMS && item_flash) {
						for (h = 12 ; h ; h--, light += 30, dark += 30) {
							*(long*)light &= ~((long)(*sprite0++)<<cnt);
							*(long*)light |= (long)(*sprite0++)<<cnt;
							sprite0++;
						}
					} else {
						for (h = 12 ; h ; h--, light += 30, dark += 30) {
							*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
							*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						}
					}
				}

			} else {
				light += 30 * 12;
				dark += 30 * 12;
				offset += 30 * 12;
			}

			bg_loc += 16;
			if(bg_loc > 255) {
				bg_loc &= 255;
				screen_pos += glbs->current_map.screen_width;
			}
		}

		//=======================================================================
    //LAST BLOCK (Clipping support)
    //=======================================================================
    bg = glbs->current_map.screen_data[screen_pos].background;
    tile1 = glbs->current_map.bg_data[bg_loc + (bg << 8)];
		tile2 = glbs->current_map.prop_data[map_loc];

		if(tile1 > 0 && !(tile2 & 0x80)) {
			sprite0 = (unsigned short*)(bg_tiles) + tile1 * 24 - 24;
			light2 = light;
			dark2 = dark;
			h = (glbs->display_hieght - y_pos);

			for (; h ; h--, light2 += 30, dark2 += 30) {
				*(long*)light2|=(long)(*sprite0++)<<cnt;
				*(long*)dark2|=(long)(*sprite0++)<<cnt;
			}
		}

		tile1 = glbs->current_map.data[map_loc];
		tile2 = tile2 & 0x7f;

		if(tile1 > 0) {
			sprite0 = glbs->tile_data[tile1].data;

			h = (glbs->display_hieght - y_pos);

			if(tile2 != BACKGROUND && tile2 != BACKGROUND_SOLID) {
				tile_ptr_clipped->tile = sprite0;
				tile_ptr_clipped->offset = offset;
				tile_ptr_clipped->cnt = cnt;
				tile_ptr_clipped->h = h;
				tile_ptr_clipped++;
			} else {

				if(tile1 >= MAX_TILES && tile1 < MAX_TILES + MAX_ITEMS && item_flash) {
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt;
						sprite0++;
					}
				} else {
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
					}
				}
			}

		}
	}

	tile_ptr_clipped->offset = -1;
	tile_ptr->offset = -1;
}*/

/*void draw_map()
{
	register void *tile_ptr = tile_list;
	register short cnt;
	register unsigned short *sprite0;
	short h;
	register short offset = tile_list[0].offset;

	register long dark;
	register long light;


	while(offset >= 0) {
		sprite0 = glbs->tile_data[((TILE_NODE *)tile_ptr)->tile].data;
		cnt = ((TILE_NODE *)tile_ptr)->cnt;
		light = (long)glbs->light_buffer + offset;
		dark = (long)glbs->dark_buffer + offset;

		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		light += 30; dark += 30;
		*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
		*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0)<<cnt;

		tile_ptr += sizeof(TILE_NODE);
		offset = ((TILE_NODE *)tile_ptr)->offset;
	}

	tile_ptr = tile_list_clipped;
	offset = tile_list_clipped[0].offset;

	while(offset >= 0) {
		sprite0 = ((TILE_NODE_CLIPPED *)tile_ptr)->tile;
		cnt = ((TILE_NODE_CLIPPED *)tile_ptr)->cnt;
		light = (long)glbs->light_buffer + offset;
		dark = (long)glbs->dark_buffer + offset;
		h = ((TILE_NODE_CLIPPED *)tile_ptr)->h;

		for ( ; h ; h--, light += 30, dark += 30) {
			*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
			*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
		}

		tile_ptr += sizeof(TILE_NODE_CLIPPED);
		offset = ((TILE_NODE_CLIPPED *)tile_ptr)->offset;
	}
}*/

/*void draw_map()
{
	short x_pos = -glbs->camera.x_off;
	register short y_pos;
	register short map_loc;
	short map_loc_start = glbs->camera.tile_y * glbs->current_map.width + glbs->camera.tile_x;
	unsigned char tile1;
	register short h;
	register unsigned short* sprite0;
	unsigned short* sprite1;
	unsigned short* mask;
	register unsigned short cnt;

	register long dark;
	register long light;
	short offset;

	for( ; x_pos < glbs->display_width ; x_pos += 12, map_loc_start++) {
		y_pos = -glbs->camera.y_off;
		offset = 0;

		if(x_pos < 0)
			cnt = -x_pos+16;
    else {
    	cnt = 16-(x_pos&15);
    	offset += ((x_pos>>3)&0x1e);
    }

    light = (long)glbs->light_buffer + offset;
    dark = (long)glbs->dark_buffer + offset;

    map_loc = map_loc_start;

    //=======================================================================
    //FIRST BLOCK (Clipping support)
    //=======================================================================

		tile1 = glbs->current_map.data[map_loc];

		if(tile1 > 0) {
			sprite0 = glbs->tile_data[tile1].data;

			h = 12 + y_pos;

			for (; h ; h--, light += 30, dark += 30) {
				*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
				*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
			}

		} else {
			light += 30 * (12 + y_pos);
			dark += 30 * (12 + y_pos);
			offset += 30 * (12 + y_pos);
		}

		y_pos += 12;
		map_loc += glbs->current_map.width;

		//=======================================================================
    //MIDDLE BLOCKS (No clipping)
    //=======================================================================
		for( ; y_pos < glbs->display_hieght - 12 ; y_pos += 12, map_loc += glbs->current_map.width) {

			tile1 = glbs->current_map.data[map_loc];

			if(tile1 > 0) {
				sprite0 = glbs->tile_data[tile1].data;

				for (h = 12; h ; h--, light += 30, dark += 30) {
					*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
					*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
				}

			} else {
				light += 30 * 12;
				dark += 30 * 12;
				offset += 30 * 12;
			}
		}

		//=======================================================================
    //LAST BLOCK (Clipping support)
    //=======================================================================

		tile1 = glbs->current_map.data[map_loc];

		if(tile1 > 0) {
			sprite0 = glbs->tile_data[tile1].data;

			h = (glbs->display_hieght - y_pos);

			for (; h ; h--, light += 30, dark += 30) {
				*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
				*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
			}
		}
	}
}*/

void game_write(SAVE_GAME *dest)
{
	SAVE_GAME g;

	g.map_index = glbs->current_map.index;
	g.player_x = glbs->player.e.x;
	g.player_y = glbs->player.e.y;
	g.player_action = glbs->player.action;
	g.player_direction = glbs->player.direction;
	g.weapon_selected = glbs->player.weapon_selected;
	g.player_hp = glbs->player.hp;
	g.player_hp_max = glbs->player.hp_max;
	memcpy(g.ammo, glbs->player.ammunition, 3);
	memcpy(g.ammo_max, glbs->player.ammunition_max, 3);
	g.items_found = glbs->player.items_found;
	g.items_equiped = glbs->player.items_equiped;
	g.game_time = glbs->seconds;
	memcpy(g.map_items_found, items_found, FOUND_ITEM_SIZE);
	memcpy(g.opened_doors, opened_doors, OPENED_DOORS_SIZE);
	memcpy(g.killed_enemies, killed_enemies, KILLED_ENEMIES_SIZE);

	memcpy(dest, &g, sizeof(SAVE_GAME));
}

void game_unwrite(SAVE_GAME *src)
{
	player_set_action(src->player_action);
	glbs->player.direction = src->player_direction;
	glbs->player.weapon_selected = src->weapon_selected;
	glbs->player.hp = src->player_hp;
	glbs->player.hp_max = src->player_hp_max;
	memcpy(glbs->player.ammunition, src->ammo, 3);
	memcpy(glbs->player.ammunition_max, src->ammo_max, 3);
	glbs->player.items_found = src->items_found;
	glbs->player.items_equiped = src->items_equiped;
	glbs->seconds = src->game_time;
	memcpy(items_found, src->map_items_found, FOUND_ITEM_SIZE);
	memcpy(opened_doors, src->opened_doors, OPENED_DOORS_SIZE);
	memcpy(killed_enemies, src->killed_enemies, KILLED_ENEMIES_SIZE);
	set_map(src->map_index);
	player_set_position(src->player_x, src->player_y);
	focus_camera();
	if(glbs->current_map.special_type == SPECIAL_SAVE) glbs->special.save.active = 2;
	bar_update();
	map_update();
}

void respawn_player()
{
	player_init();

	game_unwrite(&glbs->respawn_point);
	game_write(&glbs->save_game);
}

#define GLOBAL_MAP(z, x, y) explored_map[z][(y) * zone_header[z].width + (x)]

char game_save()
{
	SYM_ENTRY *sym_ptr;
	SCREEN_HEADER *hdr;
	char *data;
	unsigned char tile;
	short z, m, i, x, y;
	short map_pos;
	//unsigned char map_save[screen_number / 4 + 1];
	unsigned short size;
	//unsigned short offset;
	//unsigned short compressed_size[zone_number];
	//HANDLE compressed_data[zone_number];

	sym_ptr = DerefSym(SymAdd(SYMSTR(game_name)));

	if(sym_ptr == NULL) return FALSE;

	size = 2 + (2 * sizeof(SAVE_GAME)) + (screen_number / 4 + 1) + 7;
	sym_ptr->handle = HeapAlloc(size);// + 2 * zone_number);
	data = HLock(sym_ptr->handle);
	memset(data, 0, size);
	*(unsigned short *)data = size - 2;
	memcpy(data + 2, &glbs->save_game, sizeof(SAVE_GAME));
	data += 2 + sizeof(SAVE_GAME);
	memcpy(data, &glbs->respawn_point, sizeof(SAVE_GAME));
	data += sizeof(SAVE_GAME);

	//memset(map_save, 0, screen_number / 4 + 1);
	map_pos = 0;
	for(m = 0 ; m < map_number ; m++) {
		hdr = map_data[map_header[m].file] + map_header[m].screen_offset;
		x = map_header[m].x_pos; y = map_header[m].y_pos; z = map_header[m].zone;
		for(i = 0 ; i < map_header[m].screen_number; i++) {
			tile = GLOBAL_MAP(z, x + hdr[i].x_pos, y + hdr[i].y_pos);
			if(tile < 81) data[map_pos / 4] |= (0b01 << ((map_pos % 4) * 2));
			else if(tile > 81) data[map_pos / 4] |= (0b10 << ((map_pos % 4) * 2));
			map_pos++;
		}
	}

	//memcpy(data, map_save, screen_number / 4 + 1);
	data += screen_number / 4 + 1;

	/*offset = 2 + 2 * sizeof(SAVE_GAME) + 2 * zone_number;
	//ClrScr();
	for(i = 0 ; i < zone_number ; i++) {
		size = zone_header[i].width * zone_header[i].hieght;
		compressed_data[i] = HeapAlloc(size);
		//printf_xy(0, i * 8, "size: %d alloc:%d", size, *(unsigned short *)(HeapDeref(compressed_data[i]) - 2));
		compressed_size[i] = rle_compress(explored_map[i], HeapDeref(compressed_data[i]), size);
		*(unsigned short *)data = offset;
		data += 2;
		offset += compressed_size[i];
	}

	HeapUnlock(sym_ptr->handle);
	//printf_xy(0, 90, "size:%d", offset + 7); pause();
	sym_ptr->handle = HeapRealloc(sym_ptr->handle, offset + 7);
	//printf_xy(60, 90, "ok"); pause();
	data = HLock(sym_ptr->handle);


	*(unsigned short *)data = offset + 7 - 2;

	data += 2 + 2 * sizeof(SAVE_GAME) + 2 * zone_number;

	for(i = 0 ; i < zone_number ; i++) {
		//printf_xy(0, i * 8 + zone_number * 8, "free:%d", *(unsigned short *)(HeapDeref(compressed_data[i]) - 2));
		//pause();
		memcpy(data, HeapDeref(compressed_data[i]), compressed_size[i]);
		data += compressed_size[i];
		HeapFree(compressed_data[i]);
	}*/

	HeapUnlock(sym_ptr->handle);

	*(unsigned char *)(data++) = 0;
	*(unsigned char *)(data++) = 'M';
	*(unsigned char *)(data++) = 'S';
	*(unsigned char *)(data++) = 'A';
	*(unsigned char *)(data++) = 'V';
	*(unsigned char *)(data++) = 0;
	*(unsigned char *)(data) = OTH_TAG;

	// Native port: the save "variable" lives in RAM; persist it to disk so it
	// survives exit/restart (and shows up in the title save-slot menu).
	ti_persist_var(SYMSTR(game_name));

	return TRUE;
}

char game_load()
{
	SCREEN_HEADER *hdr;
	unsigned char tile;
	short z, m, i, x, y;
	short map_pos;
	//unsigned short offset;
	//short i;
	char *data;
	//char *base;

	data = file_pointer(SYMSTR(game_name));
	if(data == NULL) return FALSE;
	//base = data - 2;
	memcpy(&glbs->save_game, data, sizeof(SAVE_GAME));
	data += sizeof(SAVE_GAME);
	memcpy(&glbs->respawn_point, data, sizeof(SAVE_GAME));
	data += sizeof(SAVE_GAME);

	game_unwrite(&glbs->save_game);

	for(i = 0 ; i < zone_number ; i++)
		memset(explored_map[i], 81, zone_header[i].width * zone_header[i].hieght);

	map_pos = 0;
	for(m = 0 ; m < map_number ; m++) {
		hdr = map_data[map_header[m].file] + map_header[m].screen_offset;
		x = map_header[m].x_pos; y = map_header[m].y_pos; z = map_header[m].zone;
		for(i = 0 ; i < map_header[m].screen_number; i++) {
			tile = (data[map_pos / 4] >> ((map_pos % 4) * 2)) & 0b00000011;
			if(tile == 0b01) GLOBAL_MAP(z, x + hdr[i].x_pos, y + hdr[i].y_pos) = hdr[i].map_tile;
			else if(tile == 0b10) GLOBAL_MAP(z, x + hdr[i].x_pos, y + hdr[i].y_pos) =
				map_room_table[hdr[i].map_tile & 0x7f];
			map_pos++;
		}
	}

	//for(i = 0 ; i < zone_number ; i++) {
	//	offset = *(unsigned short *)data;
	//	rle_decompress(base + offset, explored_map[i], zone_header[i].width * zone_header[i].hieght);
	//	data += 2;
	//}

	unlock_file(game_name);

	return TRUE;
}