#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include "tiles.h"
#include "enemy.h"
#include "doors.h"
#include "screen.h"
#include "map.h"

#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_BLACK makecol(0, 0, 0)
#define COLOR_RED makecol(255, 0, 0)

typedef struct {
	short width;
	short hieght;
	short footer_hieght;
	short scroll_x;
	short scroll_y;
	short auto_x;
	short auto_y;
	const unsigned char *data;
	const unsigned char *footer;
} _BACKGROUND_HEADER;

const unsigned char cave_map[] = {
	1, 0, 1, 2, 1, 0, 1, 1,
	2, 1, 0, 1, 0, 1, 2, 3,
	1, 0, 0, 0, 0, 0, 1, 2,
	0, 0, 0, 1, 0, 0, 0, 1,
};

const unsigned char cave_shaft[] = {
	3, 4, 5, 6, 7, 8,
};

const unsigned char ship_map[] = {
	17,17,17,17,17,17,17,17,
	13,14,12,12,13,14,12,12,
	15,16,12,12,15,16,12,12,
	18,18,18,18,18,18,18,18,
	 7, 6, 7, 7, 7, 6, 7, 7,
	 4, 5, 8, 9, 4, 5, 8, 9,
	 4, 5,10,11, 4, 5,10,11,
	 7, 6, 7, 7, 7, 6, 7, 7,
};

const unsigned char special_map[] = {
	36, 37, 38, 39, 40, 41, 42, 43,
	48, 49, 50, 51, 52, 53, 54, 55,
	40, 41, 42, 43, 44, 45, 46, 47,
	52, 53, 54, 55, 56, 57, 58, 59,
	44, 45, 46, 47, 36, 37, 38, 39,
	56, 57, 58, 59, 48, 49, 50, 51,
};

const unsigned char temple_map[] = {
	19,20,21,22,23,24,25,26,27,
	28,29,30,31,23,32,33,34,35,
};

const unsigned char surface_map[] = {
	76,77,78,79,80,81,82,83,84,85,
	86,87,88,89,90,91,92,93,94,95,
	80,81,82,83,84,85,76,77,78,79,
	90,91,92,93,94,95,86,87,88,89,
};

_BACKGROUND_HEADER bg_list[] = {
	{8, 4, 0, 25, 25, 0, 0, cave_map, NULL},
	{8, 8, 0, 25, 25, 0, 0, ship_map, NULL},
	{10, 4, 3, 0, 25, 20, 0, surface_map, NULL},
	{8, 6, 0, 25, 25, 0, 0, special_map, NULL},
	{9, 2, 0, 25, 25, 0, 0, temple_map, NULL},
};

map_screen::map_screen(short m)
{
	data = new unsigned char[SCREEN_WIDTH * SCREEN_HIEGHT];
	memset(data, 0, SCREEN_WIDTH * SCREEN_HIEGHT * sizeof(char));
	prop_data = new char[SCREEN_WIDTH * SCREEN_HIEGHT];
	memset(prop_data, -1, SCREEN_WIDTH * SCREEN_HIEGHT * sizeof(char));
	map = m;
	scroll_flags = 0;
	map_room = 0;
}

map_screen::map_screen(map_screen *old)
{
	data = new unsigned char[SCREEN_WIDTH * SCREEN_HIEGHT];
	memcpy(data, old->data, SCREEN_WIDTH * SCREEN_HIEGHT * sizeof(char));
	prop_data = new char[SCREEN_WIDTH * SCREEN_HIEGHT];
	memcpy(prop_data, old->prop_data, SCREEN_WIDTH * SCREEN_HIEGHT * sizeof(char));
	map = old->map;
	scroll_flags = old->scroll_flags;
	map_room = old->map_room;
}

map_screen::~map_screen()
{
	delete data;
	delete prop_data;
}

bool map_screen::set_tile(int x, int y, char a, char mode)
{
	int x1, y1;
	
	if(x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HIEGHT) return false;
	if(mode == TILE_GFX || mode == TILE_GRAB) data[y * SCREEN_WIDTH + x] = a;
	if(mode == TILE_PROPS) prop_data[y * SCREEN_WIDTH + x] = a - 1;
	if(mode == TILE_UNIVERSAL) data[y * SCREEN_WIDTH + x] = a + MAX_TILES;
	if(a || mode == TILE_PROPS) return false;
	
	for(x1 = 0 ; x1 < SCREEN_WIDTH ; x1++)
		for(y1 = 0 ; y1 < SCREEN_HIEGHT ; y1++)
			if(data[y1 * SCREEN_WIDTH + x1]) return false;

	return true;
}

unsigned char map_screen::get_tile(int x, int y, char mode)
{
	if(mode == TILE_PROPS) return prop_data[y * SCREEN_WIDTH + x] + 1;
	else return data[y * SCREEN_WIDTH + x];
	return 0;
}

int map_screen::get_map()
{
	return map;
}

void map_screen::draw(BITMAP *buffer,
							tileset *tiles,
							tileset *bg_tiles,
							char tileset,
							char background,
							short current_map,
							int x, int y)
{
	_BACKGROUND_HEADER *b = bg_list + background;
	int x1, y1;
	int x_pos, y_pos;
	int tile;
	int set;
	
	x_pos = 0; y_pos = 0;
	for(x1 = 0; x1 < 12; x1++) {
		for(y1 = 0; y1 < 12; y1++) {
			tile = b->data[y_pos * b->width + x_pos];
			//textprintf(buffer, font, x1*16+x,y1*16+y, COLOR_WHITE, "%d", tile);
			if(current_map != map)
				draw_rle_sprite(buffer, bg_tiles->get_tile(tile),
					x1 * 16 + x, y1 * 16 + y);
			//else
			//	draw_lit_rle_sprite(buffer, bg_tiles->get_tile(tile),
			//		x1 * 16 + x, y1 * 16 + y, 64);
			y_pos++;
			if(y_pos == b->hieght) y_pos = 0;
		}
		y_pos = 0;
		x_pos++;
		if(x_pos == b->width) x_pos = 0;
	}

	for(x1 = 0 ; x1 < SCREEN_WIDTH ; x1++) {
		for(y1 = 0 ; y1 < SCREEN_WIDTH ; y1++) {
			tile = data[y1 * SCREEN_WIDTH + x1];
			if(tile >= 0) {
				if(tile < MAX_TILES) set = tileset + TILE_BASE;
				else {
					set = TILE_B;
					tile -= MAX_TILES;
				}

				if(current_map == map)
					draw_rle_sprite(buffer, tiles[set].get_tile(tile),
						x1 * 12 + x, y1 * 12 + y);
				else
					draw_lit_rle_sprite(buffer, tiles[set].get_tile(tile),
						x1 * 12 + x, y1 * 12 + y, 64);
			}
			tile = prop_data[y1 * SCREEN_WIDTH + x1];
			if(tile >= 0) {
				if(current_map == map)
					draw_rle_sprite(buffer, tiles[TILE_A].get_tile(tile + 1),
						x1 * 12 + x, y1 * 12 + y);
				else
					draw_lit_rle_sprite(buffer, tiles[TILE_A].get_tile(tile + 1),
						x1 * 12 + x, y1 * 12 + y, 50);
			}
				
		}
	}

	if(scroll_flags & SCREEN_LEFT)
		vline(buffer, x + 3, y + 3, y + SCREEN_HIEGHT * 12 - 3, COLOR_RED);
	if(scroll_flags & SCREEN_RIGHT)
		vline(buffer, x + SCREEN_WIDTH * 12 - 3, y + 3, y + SCREEN_HIEGHT * 12 - 3, COLOR_RED);
	if(scroll_flags & SCREEN_UP)
		hline(buffer, x + 3, y + 3, x + SCREEN_WIDTH * 12 - 3, COLOR_RED);
	if(scroll_flags & SCREEN_DOWN)
		hline(buffer, x + 3, y + SCREEN_HIEGHT * 12 - 3, x + SCREEN_WIDTH * 12 - 3, COLOR_RED);
}

void map_screen::properties(short map_number)
{
	short i;
	char maproom_char[5];
	char map_char[5];
	DIALOG d[] = {
		{d_box_proc, 0, 0, 300, 90, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, NULL, NULL, NULL},

		{d_text_proc, 5, 5, 85, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Map Room:", NULL, NULL},
		{d_edit_proc, 85, 5, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, maproom_char, NULL, NULL},

		{d_text_proc, 5, 15, 85, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Map:", NULL, NULL},
		{d_edit_proc, 85, 15, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, map_char, NULL, NULL},
		{d_text_proc, 5, 25, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Scroll boundry:", NULL, NULL},
		{d_check_proc, 5, 35, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Left", NULL, NULL},
		{d_check_proc, 5, 45, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Right", NULL, NULL},
		{d_check_proc, 5, 55, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Up", NULL, NULL},
		{d_check_proc, 5, 65, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Down", NULL, NULL},

		{d_button_proc, 5, 70, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "OK", NULL, NULL},
		{d_button_proc, 155, 70, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "NO", NULL, NULL},
		{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
	};

	sprintf(maproom_char, "%d", map_room);
	sprintf(map_char, "%d", map);

	for(i = 0 ; i < 4 ; i++)
		if(scroll_flags & (1 << i)) d[i + 6].flags |= D_SELECTED;

	centre_dialog(d);

	do_dialog(d, -1);

	if(d[13].flags & D_SELECTED) return;
	
	scroll_flags = 0;
	for(i = 0 ; i < 4 ; i++)
		if(d[i + 6].flags & D_SELECTED) scroll_flags |= (1 << i);
	//i = atoi(background_char);
	//if(i >= 0) bg = i;
	i = atoi(map_char);
	if(i >= 0 && i < map_number) map = i;
	i = atoi(maproom_char);
	if(i >= 0) map_room = i;
}

void map_screen::save_data(FILE *outfile)
{
	fwrite(&map, sizeof(short), 1, outfile);
	fwrite(&scroll_flags, sizeof(char), 1, outfile);
	fwrite(&map_room, sizeof(short), 1, outfile);
	fwrite(data, SCREEN_WIDTH * SCREEN_HIEGHT, sizeof(char), outfile);
	fwrite(prop_data, SCREEN_WIDTH * SCREEN_HIEGHT, sizeof(char), outfile);
	//fwrite(&bg, sizeof(unsigned char), 1, outfile);
}

void map_screen::load_data(FILE *infile)
{
	//char tmp;
	fread(&map, sizeof(short), 1, infile);
	fread(&scroll_flags, sizeof(char), 1, infile);
	fread(&map_room, sizeof(short), 1, infile);
	fread(data, SCREEN_WIDTH * SCREEN_HIEGHT, sizeof(char), infile);
	fread(prop_data, SCREEN_WIDTH * SCREEN_HIEGHT, sizeof(char), infile);
	//fread(&tmp, sizeof(unsigned char), 1, infile);
	if(map_room < 0 || map_room > 10) map_room = 0;
}

//short flip_short(short a)
//{
//	return ((a >> 8) & 0xff) | ((a & 0xff) << 8);
//}

unsigned char *map_screen::get_data()
{
	return data;
}

char *map_screen::get_prop_data()
{
	return prop_data;
}


