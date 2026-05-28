#include <CRTDBG.H>
#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "tiles.h"
#include "enemy.h"
#include "doors.h"
#include "screen.h"
#include "map.h"
#include "bin2oth.h"

#define EMBEDDED_USE
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#include "tt.h"          // generic defines
#include "ttversion.h"   // tigcc tools suite version info
#include "revtools.h"    // used for id displaying
#include "packhead.h"    // compressed header definition

#include "ttpack.c"  // thats the the important one!

#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_BLACK makecol(0, 0, 0)
#define COLOR_SELECTED makecol(255, 128, 255)

//#define FIRST_HIDDEN_ITEM 60
//#define LAST_HIDDEN_ITEM 64

void pause()
{
	//textout(screen, font, "LKJSDFLKASJDLKAj", 0, 0, COLOR_WHITE);
	while(!key[KEY_SPACE]);
	while(key[KEY_SPACE]);
	//clear(screen);
}

map::map()
{
	int i;
	char tiles_file[1000];
	BITMAP *tiles_bmp;
	RGB pal[256];

	item_display = item_counter = 0;
	map_number = 0;
	data = 0;
	header = 0;
	new_map();
	width = 0;
	hieght = 0;
	x_offset = 0;
	y_offset = 0;
	camera_x = 0;
	camera_y = 0;
	item_number = 0;
	tile_mode = TILE_GFX;
	screen_number = 0;
	player_start_x = player_start_y = player_start_map = -1;
	memset(zone_names, 0, 50 * 20);

	doors = new door();

	tiles_bmp = load_bitmap("c:/calculator/metroid gfx/tileA.bmp", pal);
	if(tiles_bmp) {
		tiles[TILE_A].load_tiles(tiles_bmp, 12, 12);
		destroy_bitmap(tiles_bmp);
		prop_bg_offset = tiles[TILE_A].tile_number - 1;
	}

	tiles_bmp = load_bitmap("c:/calculator/metroid gfx/tileA_bg.bmp", pal);
	if(tiles_bmp) {
		tileset *tile_bg = new tileset();
		tile_bg->load_tiles(tiles_bmp, 12, 12);
		tiles[TILE_A].add_tileset(tile_bg, 1);
		destroy_bitmap(tiles_bmp);
		delete tile_bg;
	}

	tiles_bmp = load_bitmap("c:/calculator/metroid gfx/tileB_gray.bmp", pal);
	if(tiles_bmp) {
		tiles[TILE_B].load_tiles(tiles_bmp, 12, 12);
		destroy_bitmap(tiles_bmp);
	}

	tiles_bmp = load_bitmap("c:/calculator/metroid gfx/items.bmp", pal);
	if(tiles_bmp) {
		tiles[TILE_ITEM_BASE].load_tiles(tiles_bmp, 12, 12);
		destroy_bitmap(tiles_bmp);
	}

	tiles_bmp = load_bitmap("c:/calculator/metroid gfx/bg_tile3.bmp", pal);
	if(tiles_bmp) {
		bg_tiles.load_tiles(tiles_bmp, 16, 16, true);
		destroy_bitmap(tiles_bmp);
	}
	
	for(i = 0 ; i < MAX_TILESET ; i++) {
		sprintf(tiles_file, "c:/calculator/metroid gfx/tile%d_gray.bmp", i);
		tiles_bmp = load_bitmap(tiles_file, pal);
		if(tiles_bmp == 0) break;
		
		tiles[i + TILE_BASE].load_tiles(tiles_bmp, 12, 12);
	}
}

map::~map()
{
	destroy_data();
}

void map::destroy_data()
{
	short i, x, y;

	for(x = 0 ; x < width ; x++)
		for(y = 0 ; y < hieght ; y++)
			if(data[y * width + x]) delete data[y * width + x];

	if(data) delete data;
	if(doors) delete doors;
	
	for(i = 0 ; i < map_number ; i++) {
		delete header[i].enemies;
		delete header[i].items;
		delete header[i].pipes;
	}
	if(header) delete header;
}

void map::new_map()
{
	MAP_HEADER *new_header;
	//int i;

	new_header = new MAP_HEADER[map_number + 1];

	//for(i = 0 ; i < map_number ; i++)
	memcpy(new_header, header, sizeof(MAP_HEADER) * map_number);
	
	if(header) delete header;
	header = new_header;
	
	memset(header + map_number, 0, sizeof(MAP_HEADER));
	map_number++;
	header[map_number - 1].enemies = new enemy();
	header[map_number - 1].items = new item();
	header[map_number - 1].pipes = new pipe();
	header[map_number - 1].map_room = 2;

	current_map = map_number - 1;
}

void map::next_map()
{
	current_map++;
	if(current_map >= map_number) current_map = 0;
}

void map::prev_map()
{
	current_map--;
	if(current_map < 0) current_map = map_number - 1;
}

void map::resize_grid(int x_off, int y_off, int x_shrink, int y_shrink, int new_width, int new_hieght)
{
	int width1 = width < new_width ? width : new_width;
	int hieght1 = hieght < new_hieght ? hieght : new_hieght;
	int x;
	int y;
	
	map_screen **new_data;

	new_data = new map_screen *[new_width * new_hieght];
	memset(new_data, 0, new_width * new_hieght * sizeof(map_screen *));

	for(x = 0 ; x < width1 ; x++)
		for(y = 0 ; y < hieght1 ; y++)
			new_data[(y + y_off) * new_width + x + x_off] =
				data[(y + y_shrink) * width + x + x_shrink];

	if(data) delete data;
	data = new_data;
	width = new_width;
	hieght = new_hieght;

	camera_x -= x_shrink * SCREEN_WIDTH;
	camera_y -= y_shrink * SCREEN_HIEGHT;

	x_offset += (x_off - x_shrink) * SCREEN_WIDTH;
	y_offset += (y_off - y_shrink) * SCREEN_HIEGHT;
}

void map::crop_grid()
{
	int x_off = 0;
	int y_off = 0;
	int new_width = width - 1;
	int new_hieght = hieght - 1;
	bool loop = true;
	int x;
	int y;

	while(loop) {
		for(x = 0 ; x < width ; x++)
			if(data[y_off * width + x]) loop = false;
		if(loop) y_off++;
		
		if(y_off == hieght) {
			width = 0;
			hieght = 0;
			if(data) {
				delete data;
				data = 0;
			}
			return;
		}
	}
	
	loop = true;
	while(loop) {
		for(y = 0 ; y < hieght ; y++)
			if(data[y * width + x_off]) loop = false;
		if(loop) x_off++;
	}

	loop = true;
	while(loop) {
		for(x = 0 ; x < width ; x++)
			if(data[new_hieght * width + x]) loop = false;
		if(loop) new_hieght--;
	}
	new_hieght += 1 - y_off;
	
	loop = true;
	while(loop) {
		for(y = 0 ; y < hieght ; y++)
			if(data[y * width + new_width]) loop = false;
		if(loop) new_width--;
	}
	new_width += 1 - x_off;

	if(x_off || y_off || new_width != width || new_hieght != hieght)
		resize_grid(0, 0, x_off, y_off, new_width, new_hieght);
}

void map::set_tile(char a, short x, short y)
{
	int screen_x = x / SCREEN_WIDTH;
	int screen_y = y / SCREEN_HIEGHT;
	int x_pos;
	int y_pos;
	int new_width;
	int new_hieght;
	int x_off;
	int y_off;
	bool empty;
	unsigned char old_tile;
//	short i;

	if(tile_mode == TILE_ITEMS) {
		if(a == 0) delete_item(x - camera_x, y - camera_y);
		else new_item(x - camera_x, y - camera_y, a);
		return;
	}

	if(x < 0 && (x % SCREEN_WIDTH)) screen_x--;
	if(y < 0 && (y % SCREEN_HIEGHT)) screen_y--;

	if(width == 0 && hieght == 0) {
		
		if(!a) return;

		camera_x -= screen_x * SCREEN_WIDTH;
		camera_y -= screen_y * SCREEN_HIEGHT;
		
		screen_x = 0;
		screen_y = 0;

		x %= SCREEN_WIDTH;
		y %= SCREEN_HIEGHT;

		resize_grid(0, 0, 0, 0, 1, 1);
	
	} else if(screen_x < 0 || screen_y < 0 || screen_x >= width || screen_y >= hieght) {
		
		if(!a) return;

		if(screen_x >= 0) {
			new_width = screen_x + 1 > width ? screen_x + 1 : width;
			x_off = 0;
		} else if(screen_x < 0) {
			new_width = width - screen_x;
			x_off = -screen_x;
			screen_x = 0;
			camera_x += x_off * SCREEN_WIDTH;
			x = SCREEN_WIDTH + (x % SCREEN_WIDTH);
		}
		
		if(screen_y >= 0) {
			new_hieght = screen_y + 1 > hieght ? screen_y + 1 : hieght;
			y_off = 0;
		} else if(screen_y < 0) {
			new_hieght = hieght - screen_y;
			y_off = -screen_y;
			screen_y = 0;
			camera_y += y_off * SCREEN_HIEGHT;
			y = SCREEN_HIEGHT + (y % SCREEN_HIEGHT);
		}

		resize_grid(x_off, y_off, 0, 0, new_width, new_hieght);
	}

	x_pos = x % SCREEN_WIDTH;
	y_pos = y % SCREEN_HIEGHT;

	if(!data[screen_y * width + screen_x]) {
		if(!a) return;
		data[screen_y * width + screen_x] = new map_screen(current_map);
		screen_number++;
	}

	if(data[screen_y * width + screen_x]->get_map() != current_map) return;
	
	old_tile = data[screen_y * width + screen_x]->get_tile(x_pos, y_pos, tile_mode);
	
	//if(tile_mode != TILE_PROPS) {
	//	if(old_tile >= MAX_TILES && old_tile < MAX_TILES + MAX_ITEMS) item_number--;
	//	if(tile_mode == TILE_UNIVERSAL && a < MAX_ITEMS) item_number++;
	//} else {
	//	if(a >= FIRST_HIDDEN_ITEM && a <= LAST_HIDDEN_ITEM) item_number++;
	//	if(old_tile >= FIRST_HIDDEN_ITEM && old_tile <= LAST_HIDDEN_ITEM) item_number--;
	//}

	empty = data[screen_y * width + screen_x]->set_tile(x_pos, y_pos, a, tile_mode);

	if(empty) {
		delete data[screen_y * width + screen_x];
		data[screen_y * width + screen_x] = 0;
		screen_number--;
		crop_grid();
	}
}

RLE_SPRITE *map::get_tile(short i)
{
	if(tile_mode == TILE_GFX) return tiles[header[current_map].tileset + TILE_BASE].get_tile(i);
	else if(tile_mode == TILE_PROPS) return tiles[TILE_A].get_tile(i);
	else if(tile_mode == TILE_UNIVERSAL) return tiles[TILE_B].get_tile(i);
	else if(tile_mode == TILE_ITEMS) return tiles[TILE_ITEM_BASE].get_tile(i);
	else if(tile_mode == TILE_GRAB) {
		if(i < MAX_TILES) return tiles[header[current_map].tileset + TILE_BASE].get_tile(i);
		else return tiles[TILE_B].get_tile(i - MAX_TILES);
	}
	else return 0;
}

unsigned char map::read_tile(short x, short y)
{
	int screen_x = x / SCREEN_WIDTH;
	int screen_y = y / SCREEN_HIEGHT;
	int x_pos = x % SCREEN_WIDTH;
	int y_pos = y % SCREEN_HIEGHT;
	unsigned char t;
	short tileset;
	//short i;

	if(tile_mode == TILE_ITEMS) {
		short map = find_map(x - camera_x, y - camera_y);
	
		if(map < 0) return 0;
		else return header[map].items->get_item(x - x_offset, y - y_offset);
	}

	if(x < 0 || y < 0 || screen_x >= width || screen_y >= hieght) return 0;
	if(!data[screen_y * width + screen_x]) return 0;


	tileset = header[data[screen_y * width + screen_x]->get_map()].tileset;

	if(tile_mode == TILE_UNIVERSAL || tile_mode == TILE_GFX) tile_mode = TILE_GRAB;

	t = data[screen_y * width + screen_x]->get_tile(x_pos, y_pos, tile_mode);

	if(tile_mode == TILE_GRAB && t < MAX_TILES && tileset != header[current_map].tileset) t = 0;

	return t;
}

short map::get_tile_number()
{
	if(tile_mode == TILE_GFX || tile_mode == TILE_GRAB)
		return tiles[header[current_map].tileset + TILE_BASE].tile_number;
	if(tile_mode == TILE_PROPS) return tiles[TILE_A].tile_number;
	if(tile_mode == TILE_UNIVERSAL) return tiles[TILE_B].tile_number;
	if(tile_mode == TILE_ITEMS) return tiles[TILE_ITEM_BASE].tile_number;
	return 0;
}

void map::change_tile_mode()
{
	tile_mode++;
	if(tile_mode > TILE_GRAB) tile_mode = 1;
	else if(tile_mode == TILE_GRAB) tile_mode = 0;
}

MAP_HEADER map::get_header(short i)
{
	MAP_HEADER a;

	memcpy(&a, &header[current_map], sizeof(MAP_HEADER));

	return a;
}

void map::select_map(short tile_x, short tile_y)
{
	short map = find_map(tile_x, tile_y);
	if(map >= 0) current_map = map;
}

void map::center_view()
{
	short x0 = width + 1, y0 = hieght + 1, x, y;

	for(x = 0 ; x < width ; x++)
		for(y = 0 ; y < hieght ; y++)
			if(data[y * width + x])
				if(data[y * width + x]->get_map() == current_map) {
					if(x < x0) x0 = x;
					if(y < y0) y0 = y;
				}

	camera_x = x0 * SCREEN_WIDTH;
	camera_y = y0 * SCREEN_HIEGHT;
}

short map::find_map(short x, short y)
{
	map_screen *s;
	short screen_x, screen_y;

	if(!data) return -1;
	
	screen_x = (x + camera_x) / SCREEN_WIDTH;
	screen_y = (y + camera_y) / SCREEN_HIEGHT;

	if(x + camera_x < 0 || screen_x >= width || y + camera_y < 0 || screen_y >= hieght) return -1;
	s = data[screen_y * width + screen_x];
	if(!s) return -1;

	//clear(screen);
	//textprintf(screen, font, 0, 0, COLOR_WHITE, "%d %d %d", screen_x, screen_y, s->get_map());
	//while(!key[KEY_SPACE]);

	return s->get_map();
}

void map::new_pipe(short x, short y)
{
	short map = find_map(x, y);
	if(map < 0) return;

	header[map].pipes->new_pipe(x + camera_x - x_offset, y + camera_y - y_offset);
}

void map::new_enemy(short x, short y, short type)
{
	short map = find_map(x, y);
	if(map < 0) return;

	if(type == 0) {
		if(player_start_x >= 0) header[player_start_map].enemies->delete_ship();
		player_start_x = x + camera_x;
		player_start_y = y + camera_y;
		player_start_map = map;
	}

	header[map].enemies->new_enemy(x + camera_x - x_offset, y + camera_y - y_offset, type);
}

void map::modify_enemy(short x, short y)
{
	short map = find_map(x, y);
	if(map < 0) return;

	header[map].enemies->modify_enemy(x + camera_x - x_offset, y + camera_y - y_offset);
}

void map::new_door(short x, short y, char type)
{
	short map = find_map(x, y);
	if(map < 0) return;

	doors->new_door(x + camera_x - x_offset, y + camera_y - y_offset, map, type);
}

void map::door_properties(short x, short y)
{
	doors->door_properties(x + camera_x - x_offset, y + camera_y - y_offset);
}

void map::new_item(short x, short y, unsigned char type)
{
	short map = find_map(x, y);
	
	if(map < 0) return;
	if(header[map].items->new_item(x + camera_x - x_offset, y + camera_y - y_offset, type))
		item_number++;
}

void map::delete_item(short x, short y)
{
	short map = find_map(x, y);
	
	if(map < 0) return;
	if(header[map].items->delete_item(x + camera_x - x_offset, y + camera_y - y_offset))
		item_number--;
}

void map::draw_maps(BITMAP *bmp, tileset *pics, tileset *pipe_pics)
{
	short map;
	int x, x_pos;
	int y, y_pos;
	int x_start = camera_x / SCREEN_WIDTH;
	int y_start = camera_y / SCREEN_HIEGHT;
	short maps[20];
	short i = 0, j;
	
	for(x = x_start ; x < x_start + 4 ; x++) {
		for(y = y_start ; y < y_start + 4 ; y++) {
			
			if(x >= width || y >= hieght || x < 0 || y < 0) continue;

			if(data[y * width + x]) {
				map = data[y * width + x]->get_map();
				x_pos = x * 12 * SCREEN_WIDTH - camera_x * 12;
				y_pos = y * 12 * SCREEN_HIEGHT - camera_y * 12;
				
				data[y * width + x]->draw(bmp, tiles, &bg_tiles,
					header[map].tileset, header[map].background, current_map, x_pos, y_pos);
				for(j = 0 ; j < i ; j++) {
					if(maps[j] == map) break;
				}
				if(j == i) {
					maps[i] = map;
					i++;
				}

				textprintf(bmp, font, x_pos + 2, y_pos + 2, COLOR_SELECTED, "%d", map + 1);

			}
		}
	}

	item_counter--;
	if(item_counter < 0) {
		item_display = !item_display;
		item_counter = 5 + 5 * item_display;
	}
	
	for(j = 0 ; j < i ; j++) {
		header[maps[j]].enemies->draw(bmp, camera_x - x_offset, camera_y - y_offset, pics);
		header[maps[j]].pipes->draw(bmp, camera_x - x_offset, camera_y - y_offset, pipe_pics);
		if(item_display)
			header[maps[j]].items->draw(bmp, camera_x - x_offset, camera_y - y_offset, &tiles[TILE_ITEM_BASE]);
	}

	doors->draw_doors(bmp, camera_x - x_offset, camera_y - y_offset);

	drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);

	x = ((SCREEN_WIDTH - camera_x) % SCREEN_WIDTH) * 12;
	
	while(x < 480) {
		line(bmp, x, 0, x, 480, COLOR_WHITE);
		x += SCREEN_WIDTH * 12;
	}

	y = ((SCREEN_HIEGHT - camera_y) % SCREEN_HIEGHT) * 12;
	
	while(y < 480) {
		line(bmp, 0, y, 480, y, COLOR_WHITE);
		y += SCREEN_HIEGHT * 12;
	}

	//textprintf(bmp, font, 0, 0, COLOR_WHITE, "TILE:%08X",
	//	(unsigned long)bg_tiles->get_tile(0));

	drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
}

void map::printdim(BITMAP *bmp, int x, int y, int color)
{
	textprintf(bmp, font, x, y, color, "Width: %d, Hieght: %d", width, hieght);
	textprintf(bmp, font, x, y + 10, color, "Screens: %d", screen_number);
	textprintf(bmp, font, x, y + 20, color, "Items: %d", item_number);
	textprintf(bmp, font, x + 100, y + 10, color, "Doors: %d", doors->door_number);
	textprintf(bmp, font, x + 200, y + 10, color, "Pipes: %d", header[current_map].pipes->number);
	textprintf(bmp, font, x, y + 30, color, "x:%d y:%d map:%d", player_start_x, player_start_y, player_start_map);

	//textprintf(bmp, font, x, y + 40, color, "%d", buffer);
}

void map::draw_tiles(BITMAP *bmp, int width, int selected_tile)
{
	if(tile_mode == TILE_GFX || tile_mode == TILE_GRAB)
		tiles[header[current_map].tileset + TILE_BASE].draw(bmp, width, selected_tile, 0, 0);
	else if(tile_mode == TILE_PROPS) tiles[TILE_A].draw(bmp, width, selected_tile, 0, 0);
	else if(tile_mode == TILE_UNIVERSAL) tiles[TILE_B].draw(bmp, width, selected_tile, 0, 0);
	else if(tile_mode == TILE_ITEMS) tiles[TILE_ITEM_BASE].draw(bmp, width, selected_tile, 0, 0);
}

void map::draw_doors_select(BITMAP *bmp, short x, short y, short selected_door)
{
	doors->draw_doors_select(bmp, x, y, selected_door);
}

void map::camera_up()
{
	camera_y--;
}

void map::camera_down()
{
	camera_y++;
}

void map::camera_left()
{
	camera_x--;
}

void map::camera_right()
{
	camera_x++;
}

void map::auto_save()
{
	FILE *outfile;
	short i, j;
	char file_name[128];
	char buffer[128];
	
	_strtime(buffer);
	memset(file_name, 0, 128);
	sprintf(file_name, "./auto/auto");
	i = 11; j = 0;
	while(buffer[j] != 0) {
		if(buffer[j] != ':') {
			file_name[i] = buffer[j];
		} else {
			file_name[i] = '_';
		}
		i++; j++;
	}

	file_name[i++] = '.';
	file_name[i++] = 'm';
	file_name[i++] = 'm';
	file_name[i++] = 'p';

	outfile = fopen(file_name, "wb");
	save_data(outfile);
	fclose(outfile);
}

void map::save_data(FILE *outfile)
{
	short i, x, y;
	char sig[4] = {MAGIC_A, MAGIC_B, MAGIC_C, MAGIC_D};
	
	fwrite(sig, sizeof(char), 4, outfile);
	fwrite(&map_number, sizeof(short), 1, outfile);
	fwrite(&screen_number, sizeof(short), 1, outfile);
	fwrite(&item_number, sizeof(short), 1, outfile);
	fwrite(&width, sizeof(short), 1, outfile);
	fwrite(&hieght, sizeof(short), 1, outfile);
	fwrite(&x_offset, sizeof(int), 1, outfile);
	fwrite(&y_offset, sizeof(int), 1, outfile);
	fwrite(&camera_x, sizeof(short), 1, outfile);
	fwrite(&camera_y, sizeof(short), 1, outfile);
	fwrite(&current_map, sizeof(short), 1, outfile);
	fwrite(&player_start_x, sizeof(short), 1, outfile);
	fwrite(&player_start_y, sizeof(short), 1, outfile);
	fwrite(&player_start_map, sizeof(short), 1, outfile);
	fwrite(&tile_mode, sizeof(char), 1, outfile);

	for(i = 0 ; i < map_number ; i++) {
		fwrite(&header[i].zone, sizeof(unsigned char), 1, outfile);
		fwrite(&header[i].tileset, sizeof(char), 1, outfile);

		fwrite(&header[i].background, sizeof(unsigned char), 1, outfile);
		fwrite(&header[i].bg_wave, sizeof(char), 1, outfile);

		fwrite(&header[i].special_x, sizeof(short), 1, outfile);
		fwrite(&header[i].special_y, sizeof(short), 1, outfile);
		fwrite(&header[i].special_type, sizeof(char), 1, outfile);
		fwrite(&header[i].map_room, sizeof(short), 1, outfile);

		fwrite(&header[i].environment, sizeof(char), 1, outfile);
		fwrite(&header[i].water_start, sizeof(short), 1, outfile);
		fwrite(&header[i].water_end, sizeof(short), 1, outfile);
		fwrite(&header[i].water_speed, sizeof(short), 1, outfile);
		fwrite(&header[i].water_type, sizeof(char), 1, outfile);
		header[i].enemies->save_data(outfile);
		header[i].items->save_data(outfile);
		header[i].pipes->save_data(outfile);
	}

	for(x = 0 ; x < width ; x++)
		for(y = 0 ; y < hieght ; y++) {
			if(!data[y * width + x]) continue;
			fwrite(&x, sizeof(short), 1, outfile);
			fwrite(&y, sizeof(short), 1, outfile);
			data[y * width + x]->save_data(outfile);
		}
	
	doors->save_data(outfile);
	//bg->write_data(outfile);

	fwrite(zone_names, 20, 50, outfile);
}

void map::load_data(FILE *infile)
{
	short i, x, y;
	char sig[4];
	//int w, h;

	fread(sig, sizeof(char), 1, infile);
	fread(sig + 1, sizeof(char), 1, infile);
	fread(sig + 2, sizeof(char), 1, infile);
	fread(sig + 3, sizeof(char), 1, infile);

	if(sig[0] != MAGIC_A || sig[1] != MAGIC_B || sig[2] != MAGIC_C || sig[3] != MAGIC_D) return;
	
	destroy_data();
	
	fread(&map_number, sizeof(short), 1, infile);
	fread(&screen_number, sizeof(short), 1, infile);
	fread(&item_number, sizeof(short), 1, infile);
	fread(&width, sizeof(short), 1, infile);
	fread(&hieght, sizeof(short), 1, infile);
	fread(&x_offset, sizeof(int), 1, infile);
	fread(&y_offset, sizeof(int), 1, infile);
	fread(&camera_x, sizeof(short), 1, infile);
	fread(&camera_y, sizeof(short), 1, infile);
	fread(&current_map, sizeof(short), 1, infile);
	fread(&player_start_x, sizeof(short), 1, infile);
	fread(&player_start_y, sizeof(short), 1, infile);
	fread(&player_start_map, sizeof(short), 1, infile);
	fread(&tile_mode, sizeof(char), 1, infile);

	//width = w;
	//hieght = h;

	//clear(screen);
	//textprintf(screen, font, 0, 0, COLOR_WHITE, "map_number:%d", map_number);
	//textprintf(screen, font, 0, 10, COLOR_WHITE, "screen_number:%d", screen_number);
	//textprintf(screen, font, 0, 20, COLOR_WHITE, "width:%d hieght:%d", width, hieght);
	//textprintf(screen, font, 0, 30, COLOR_WHITE, "camera_x:%d camera_y:%d", camera_x, camera_y);
	//textprintf(screen, font, 0, 40, COLOR_WHITE, "x_offset:%d y_offset:%d", x_offset, y_offset);
	//textprintf(screen, font, 0, 50, COLOR_WHITE, "current_map:%d",current_map);
	//while(!key[KEY_SPACE]);

	data = new map_screen *[width * hieght];
	memset(data, 0, width * hieght * sizeof(map_screen *));
	header = new MAP_HEADER[map_number];
	item_number = 0;

	for(i = 0 ; i < map_number ; i++) {
		fread(&header[i].zone, sizeof(unsigned char), 1, infile);
		fread(&header[i].tileset, sizeof(char), 1, infile);

		fread(&header[i].background, sizeof(unsigned char), 1, infile);
		fread(&header[i].bg_wave, sizeof(char), 1, infile);

		fread(&header[i].special_x, sizeof(short), 1, infile);
		fread(&header[i].special_y, sizeof(short), 1, infile);
		fread(&header[i].special_type, sizeof(char), 1, infile);
		fread(&header[i].map_room, sizeof(short), 1, infile);

		fread(&header[i].environment, sizeof(char), 1, infile);
		fread(&header[i].water_start, sizeof(short), 1, infile);
		fread(&header[i].water_end, sizeof(short), 1, infile);
		fread(&header[i].water_speed, sizeof(short), 1, infile);
		fread(&header[i].water_type, sizeof(char), 1, infile);

		header[i].enemies = new enemy();
		header[i].enemies->load_data(infile);
		header[i].items = new item();
		header[i].items->load_data(infile);
		header[i].pipes = new pipe();
		header[i].pipes->load_data(infile);

		item_number += header[i].items->number;
		
		//switch(header[i].tileset) {
		//	case 0: header[i].zone = 0; break;
		//	case 1: header[i].zone = 1; break;
		//	case 4: header[i].zone = 2; break;
		//}


	}

	//clear(screen);

//#define SHIFT_ZONE_3

	for(i = 0 ; i < screen_number ; i++) {
		map_screen *temp_screen;
		fread(&x, sizeof(short), 1, infile);
		fread(&y, sizeof(short), 1, infile);
		temp_screen = new map_screen((short)0);
		temp_screen->load_data(infile);
#ifdef SHIFT_ZONE_3
		if(header[temp_screen->get_map()].zone == 3 && x != 0) data[y * width + x - 1] = temp_screen;
		else data[y * width + x] = temp_screen;
#else
		data[y * width + x] = temp_screen;
#endif

		//textprintf(screen, font, 0, i * 10, COLOR_WHITE, "%d, %d", x, y);

		//data[y * width + x] = new map_screen((short)0);
		//data[y * width + x]->load_data(infile);
		/*if(header[data[y * width + x]->get_map()].zone == 3) {
			data[y * width + x - 1] = data[y * width + x];
			data[y * width + x] = 0;
		}*/
	}
	
	//pause();

	doors = new door();
	doors->load_data(infile);

	fread(zone_names, 50, 20, infile);

#ifdef SHIFT_ZONE_3
	short e;
	for(i = 0 ; i < map_number ; i++) {
		if(header[i].zone != 3) continue;
		for(e = 0 ; e < header[i].enemies->number ; e++)
			header[i].enemies->data[e].x -= 16;
		for(e = 0 ; e < header[i].items->number ; e++)
			header[i].items->data[e].x -= 16;
		for(e = 0 ; e < header[i].pipes->number ; e++)
			header[i].pipes->data[e].x -= 16;
	}
	for(i = 0 ; i < doors->door_number ; i++) {
		if(header[doors->data[i].map].zone == 3) doors->data[i].x -= 16;
	}
#endif

	/*for(y = 0 ; y < hieght ; y++) {
		for(x = 1 ; x < width ; x++) {
			if(data[y * width + x] == 0) continue;
			if(header[data[y * width + x]->get_map()].zone != 3) continue;
			data[y * width + x - 1] = data[y * width + x];
			data[y * width + x] = 0;
		}
	}*/
	
	if(0) {
		//unsigned char convert[33] = {0, 1, 2, 3, 6, 7, 25, 26, 28, 29, 31, 32, 33, 34, 35, 45, 46, 36,
		//	47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 60, 61, 62};
		unsigned char tile, prop;
		short x, y, x1, y1, num = 0;

		//item_number = 0;

		for(x = 0 ; x < width ; x++) {
			for(y = 0 ; y < hieght ; y++) {
				if(data[y * width + x] == 0) continue;
				for(x1 = 0 ; x1 < SCREEN_WIDTH ; x1++) {
					for(y1 = 0 ; y1 < SCREEN_HIEGHT ; y1++) {
						prop = data[y * width + x]->get_tile(x1, y1, TILE_PROPS);
						if(prop == 2 || prop == 61) prop += 62;
						//if(prop == 60) prop += 62;
						else {
							if(prop > 61) prop--;
							if(prop > 2) prop--;
						}
						data[y * width + x]->set_tile(x1, y1, prop, TILE_PROPS);
					}
				}
			}
		}
		//if(num != item_number) {
		//	clear(screen);
		//	textprintf(screen, font, 0, 0, COLOR_WHITE, "found:%d, old:%d", num, item_number);
		//	pause();
		//	item_number = num;
		//}
	}
}

/*short rle_compress(unsigned char *src, unsigned char *dst, short size)
{
	short rle_size = 0;
	short i = 0;
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
}*/

short map::get_screen_map(short x, short y)
{
	map_screen *s;
	
	if(x < 0 || y < 0 || x >= width || y >= hieght) return -1;
	
	s = data[y * width + x];
	if(!s) return -1;
	return s->get_map();
}

void map::screen_properties(short tile_x, short tile_y)
{
	short x = tile_x / SCREEN_WIDTH;
	short y = tile_y / SCREEN_HIEGHT;
	
	//clear(screen);
	//textprintf(screen, font, 0, 0, COLOR_WHITE, "%d %d", x, y);
	//pause();

	if(x < 0 || x >= width || y < 0 || y >= hieght) return;

	if(data[y * width + x] == 0) return;

	data[y * width + x]->properties(map_number);
}

enum MAP_D {
	MD_BOX,
	MD_ZONE_LABLE,
	MD_ZONE_INPUT,
	MD_TILESET_LABLE,
	MD_TILESET_INPUT,
	//MD_BG_LABLE,
	//MD_BG_INPUT,
	MD_BG_WAVE,
	MD_EVRN_LABLE,
	MD_EVRN_NONE,
	MD_EVRN_DARK,
	MD_EVRN_SNOW,
	MD_WATER_MIN_LABLE,
	MD_WATER_MIN_INPUT,
	MD_WATER_MAX_LABLE,
	MD_WATER_MAX_INPUT,
	MD_WATER_SPEED_LABLE,
	MD_WATER_SPEED_INPUT,
	MD_WATER_NONE,
	MD_WATER_NORMAL,
	MD_WATER_LAVA,
	MD_WATER_ACID,
	MD_SPECIAL_TYPE_LABLE,
	MD_SPECIAL_NONE,
	MD_SPECIAL_SAVE,
	MD_SPEICAL_RECHARGE,
	MD_SPECIAL_MAP,
	MD_SPECIAL_X_LABLE,
	MD_SPECIAL_X,
	MD_SPECIAL_Y_LABLE,
	MD_SPECIAL_Y,
	MD_SPECIAL_MAPROOM_LABLE,
	MD_SPECIAL_MAPROOM,
	MD_BG_LABEL,
	MD_OK,
	MD_NO,
	MD_BG_START,
};

#define BG_NUMBER 5

void map::map_properties()
{
	char zone_char[5];
	char tileset_char[5];
	char water_speed_char[5];
	char water_start_char[5];
	char water_end_char[5];
	//char background_char[5];
	char special_x_char[5];
	char special_y_char[5];
	char special_map_char[5];
	short i;

	char *bg_names[BG_NUMBER] = {
		"Cave",
		"Wrecked Ship",
		"Surface",
		"Special",
		"Temple",
	};

	DIALOG bg_choice =
		{d_radio_proc, 325, 15, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 99, 0, "", NULL, NULL};
	
	DIALOG d[MD_BG_START+BG_NUMBER+1] = {
		{d_box_proc, 0, 0, 500, 190, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, NULL, NULL, NULL},

		{d_text_proc, 5, 5, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Zone:", NULL, NULL},
		{d_edit_proc, 95, 5, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, zone_char, NULL, NULL},

		{d_text_proc, 5, 15, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Tileset:", NULL, NULL},
		{d_edit_proc, 95, 15, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, tileset_char, NULL, NULL},

		//{d_text_proc, 5, 25, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Background:", NULL, NULL},
		//{d_edit_proc, 95, 25, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, background_char, NULL, NULL},

		{d_check_proc, 5, 35, 150, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Background Wave", NULL, NULL},

		{d_text_proc, 5, 55, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Environment:", NULL, NULL},
		{d_radio_proc, 5, 65, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "None", NULL, NULL},
		{d_radio_proc, 5, 75, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Dark", NULL, NULL},
		{d_radio_proc, 5, 85, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Snow", NULL, NULL},

		{d_text_proc, 5, 95, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Water_start:", NULL, NULL},
		{d_edit_proc, 105, 95, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, water_start_char, NULL, NULL},

		{d_text_proc, 5, 105, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Water_end:", NULL, NULL},
		{d_edit_proc, 105, 105, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, water_end_char, NULL, NULL},

		{d_text_proc, 5, 115, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Water_speed:", NULL, NULL},
		{d_edit_proc, 105, 115, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, water_speed_char, NULL, NULL},
		
		{d_radio_proc, 5, 125, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "None", NULL, NULL},
		{d_radio_proc, 5, 135, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Water", NULL, NULL},
		{d_radio_proc, 5, 145, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Lava", NULL, NULL},
		{d_radio_proc, 5, 155, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 2, 0, "Acid", NULL, NULL},

		{d_text_proc, 165, 5, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Special:", NULL, NULL},
		{d_radio_proc, 165, 15, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "None", NULL, NULL},
		{d_radio_proc, 165, 25, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Save", NULL, NULL},
		{d_radio_proc, 165, 35, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Recharge", NULL, NULL},
		{d_radio_proc, 165, 45, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Map", NULL, NULL},

		{d_text_proc, 165, 55, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Special X:", NULL, NULL},
		{d_edit_proc, 245, 55, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, special_x_char, NULL, NULL},

		{d_text_proc, 165, 65, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Special Y:", NULL, NULL},
		{d_edit_proc, 245, 65, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, special_y_char, NULL, NULL},

		{d_text_proc, 165, 75, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Map Room:", NULL, NULL},
		{d_edit_proc, 245, 75, 40, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, special_map_char, NULL, NULL},

		{d_text_proc, 325, 5, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Background:", NULL, NULL},
		
		{d_button_proc, 5, 170, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "OK", NULL, NULL},
		{d_button_proc, 155, 170, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "NO", NULL, NULL},
		{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
	};
	
	sprintf(zone_char, "%d", header[current_map].zone);
	sprintf(tileset_char, "%d", header[current_map].tileset);
	sprintf(water_start_char, "%d", header[current_map].water_start);
	sprintf(water_end_char, "%d", header[current_map].water_end);
	sprintf(water_speed_char, "%d", header[current_map].water_speed);
	//sprintf(background_char, "%d", header[current_map].background);
	sprintf(special_x_char, "%d", header[current_map].special_x);
	sprintf(special_y_char, "%d", header[current_map].special_y);
	sprintf(special_map_char, "%d", header[current_map].map_room);

	d[MD_EVRN_NONE + header[current_map].environment].flags |= D_SELECTED;
	d[MD_SPECIAL_NONE + header[current_map].special_type].flags |= D_SELECTED;
	d[MD_WATER_NONE + header[current_map].water_type].flags |= D_SELECTED;

	if(header[current_map].bg_wave) d[MD_BG_WAVE].flags |= D_SELECTED;

	for(i = 0; i < BG_NUMBER; i++) {
		bg_choice.dp = bg_names[i];
		memcpy(d + MD_BG_START + i, &bg_choice, sizeof(DIALOG));
		bg_choice.y += 10;
	}
	d[MD_BG_START + header[current_map].background].flags |= D_SELECTED;

	d[MD_BG_START + i].proc = NULL;

	centre_dialog(d);

	do_dialog(d, 2);

	if(d[MD_NO].flags & D_GOTFOCUS) return;

	i = atoi(zone_char);
	if(i >= 0) header[current_map].zone = i;

	i = atoi(tileset_char);
	if(i >= 0 && i <= 5) header[current_map].tileset = i;

	i = atoi(water_start_char);
	header[current_map].water_start = i;

	i = atoi(water_end_char);
	header[current_map].water_end = i;

	i = atoi(water_speed_char);
	if(i >= 0) header[current_map].water_speed = i;

	//i = atoi(background_char);
	//if(i >= 0) header[current_map].background = i;

	i = atoi(special_x_char);
	if(i >= 0) header[current_map].special_x = i;

	i = atoi(special_y_char);
	if(i >= 0) header[current_map].special_y = i;

	i = atoi(special_map_char);
	if(i >= 2) header[current_map].map_room = i;

	if(d[MD_BG_WAVE].flags & D_SELECTED) header[current_map].bg_wave = TRUE;
	else header[current_map].bg_wave = FALSE;

	for(i = 0 ; i < 3 ; i++)
		if(d[MD_EVRN_NONE + i].flags & D_SELECTED) {
			header[current_map].environment = i;
			break;
		}

	for(i = 0 ; i < 4 ; i++)
		if(d[MD_SPECIAL_NONE + i].flags & D_SELECTED) {
			header[current_map].special_type = i;
			break;
		}

	for(i = 0 ; i < 4 ; i++)
		if(d[MD_WATER_NONE + i].flags & D_SELECTED) {
			header[current_map].water_type = i;
			break;
		}
	for(i = 0; i < BG_NUMBER; i++)
		if(d[MD_BG_START + i].flags & D_SELECTED) {
			header[current_map].background = i;
			break;
		}
}
	
void map::set_zone_names()
{
	DIALOG box = {d_box_proc, 0, 0, 300, 530, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, NULL, NULL, NULL};
	DIALOG lable = {d_text_proc, 5, 0, 80, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Zone0:", NULL, NULL};
	DIALOG input = {d_edit_proc, 95, 0, 200, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 19, 0, NULL, NULL, NULL};
	DIALOG ok = {d_button_proc, 5, 512, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "OK", NULL, NULL};
	DIALOG no = {d_button_proc, 155, 512, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "NO", NULL, NULL};
	DIALOG *d;
	short i;

	d = new DIALOG[104];
	memset(d, 0, sizeof(DIALOG) * 102);
	memcpy(d, &box, sizeof(DIALOG));
	for(i = 0 ; i < 50 ; i++) {
		//lable.dp[4] = (char)i;
		lable.y = i * 10 + 5;
		input.dp = zone_names + 20 * i;
		input.y = i * 10 + 5;
		d[i * 2 + 1] = lable;
		d[i * 2 + 2] = input;
		//memcpy(d + i * 2 + 1, &lable, sizeof(DIALOG));
		//memcpy(d + i * 2 + 2, &input, sizeof(DIALOG));
	}
	d[101] = ok;
	d[102] = no;
	d[103].proc = NULL;
	
	centre_dialog(d);

	//clear(screen);
	//textprintf(screen, font, 0, 0, COLOR_WHITE, "%s", d[2].dp);
	//pause();

	do_dialog(d, 1);

	delete d;
}

short flip_short(short a)
{
#ifdef MAPCONV_NATIVE
	// Native port: the consumer (game) now runs little-endian, same as this
	// host, so emit native byte order -- no calculator big-endian flip.
	return a;
#else
	return ((a >> 8) & 0xff) | ((a & 0xff) << 8);
#endif
}

#define CLEFT 1
#define CRIGHT 2
#define CUP 4
#define CDOWN 8

enum {
	LEFT,
	DOWN,
	RIGHT,
	UP
};

unsigned char map::get_door_flags(short x, short y)
{
	DOOR *d;
	short i;
	short map = data[y * width + x]->get_map();
	unsigned char flags = 0;

	for(i = 0 ; i < doors->door_number ; i++) {
		d = doors->data + i;
		if(d->map != map) continue;
		if(d->x + x_offset < x * SCREEN_WIDTH || d->x + x_offset >= (x + 1) * SCREEN_WIDTH ||
			d->y + y_offset < y * SCREEN_HIEGHT || d->y + y_offset >= (y + 1) * SCREEN_HIEGHT) continue;
		if(d->type == DOOR_RIGHT) flags |= SCREEN_LEFT;
		if(d->type == DOOR_LEFT) flags |= SCREEN_RIGHT;
		if(d->type == DOOR_DOWN) flags |= SCREEN_UP;
		if(d->type == DOOR_UP) flags |= SCREEN_DOWN;
	}

	return flags;
}

void set_map_tile(short x, short y, unsigned char tile, CALC_SCREEN_HEADER *hdr, short screen_number)
{
	short i;

	x = flip_short(x);
	y = flip_short(y);

	for(i = 0 ; i < screen_number ; i++) {
		if(x == hdr[i].x_pos && y == hdr[i].y_pos) {
			hdr[i].map_tile = tile;
			break;
		}
	}
}

void set_map_item(short x, short y, CALC_SCREEN_HEADER *hdr, short screen_number)
{
	short i;

	x = flip_short(x);
	y = flip_short(y);

	for(i = 0 ; i < screen_number ; i++) {
		if(x == hdr[i].x_pos && y == hdr[i].y_pos) {
			hdr[i].map_tile |= 0x80;
			break;
		}
	}
}

#define SHORT_NUMBER 9
void map::export_data(char *outfile_name)
{
	ZONE_HEADER *zones = 0;
	CALC_ZONE *calc_zones = 0;
	CALC_HEADER *calc_header = 0;
	CALC_DOOR **door_data = 0;
	//ITEM_LOCATION *item_data = 0;
	ENEMY **enemy_data = 0;
	PIPE **pipe_data = 0;
	ITEM **item_data = 0;
	short zone_number;
	unsigned char *screen_num;
	//unsigned char **calc_map = 0;
	//unsigned short calc_map_size;
	CALC_SCREEN_HEADER **screen_hdr;
	CALC_SCREEN_DATA tmp_screen;
	FILE *temp = 0;
	char file_name[1000];
	char output_name[1000];
	char *tool_params[3];  // mimics "*argv[]"
	short **map_doors = 0;
	unsigned char *door_number = 0;
	short *door_map_index = 0;
	short *pack_size = 0;
	short i, e, z, s, map, x, y, x1, y1, sx, sy;
	short x_pos0, y_pos0, x_pos1, y_pos1;
	//short current_item = 0;
	//unsigned char screen_num;
	unsigned char flags, door_flags;

	unsigned long offset = 0;
	unsigned long screen_offset;
	unsigned long enemy_offset;
	unsigned long pipe_offset;
	unsigned long item_offset;
	unsigned long door_offset;
	unsigned long old_offset;

	unsigned char tile, prop;
	unsigned char **buffer = 0;
	unsigned int oth_length;
	unsigned char *oth_buffer = 0;
	short current_door = 0, current_enemy = 0, current_item = 0;
	short current_file = 0;
	unsigned short file_size[10];
	char current_file_name[100];
	char oncalc_name[10];
	//unsigned char map_offset_row[] = {0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120};
	unsigned char map_offsets[] = {
		 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 1,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 3,  0,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 5,  6,  7,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		 9,  0,  0,  0, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		11, 12,  0,  0, 13, 14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		15,  0, 16,  0, 17,  0, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,
		19, 20, 21, 22, 23, 24, 25, 26,  0,  0,  0,  0,  0,  0,  0,  0,
		27,  0,  0,  0,  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,
		29, 30,  0,  0,  0,  0,  0,  0, 31, 32,  0,  0,  0,  0,  0,  0,
		33,  0, 34,  0,  0,  0,  0,  0, 35,  0, 36,  0,  0,  0,  0,  0,
		37, 38, 39, 40,  0,  0,  0,  0, 41, 42, 43, 44,  0,  0,  0,  0,
		45,  0,  0,  0, 46,  0,  0,  0, 47,  0,  0,  0, 48,  0,  0,  0,
		49, 50,  0,  0, 51, 52,  0,  0, 53, 54,  0,  0, 55, 56,  0,  0,
		57,  0, 58,  0, 59,  0, 60,  0, 61,  0, 62,  0, 63,  0, 64,  0,
		65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80
	};

	textprintf(screen, font, 0, 560, COLOR_WHITE, "EXPORTING DATA...");

	zone_number = 0;
	for(i = 0 ; i < map_number ; i++) {
		if(header[i].zone >= zone_number) zone_number = header[i].zone + 1;
	}
	zones = new ZONE_HEADER[zone_number];
	calc_zones = new CALC_ZONE[zone_number];
	//calc_map = new unsigned char *[zone_number]; memset(calc_map, 0, sizeof(unsigned char *) * zone_number);
	
	screen_num = new unsigned char[map_number];
	calc_header = new CALC_HEADER[map_number];
	enemy_data = new ENEMY *[map_number]; memset(enemy_data, 0, sizeof(ENEMY *) * map_number);
	pipe_data = new PIPE *[map_number] ; memset(pipe_data, 0, sizeof(PIPE *) * map_number);
	item_data = new ITEM *[map_number]; memset(item_data, 0, sizeof(ITEM *) * map_number);
	door_data = new CALC_DOOR *[map_number]; memset(door_data, 0, sizeof(CALC_DOOR *) * map_number);
	screen_hdr = new CALC_SCREEN_HEADER *[map_number]; memset(screen_hdr, 0, sizeof(CALC_SCREEN_HEADER *) * map_number);
	door_number = new unsigned char[map_number];
	pack_size = new short[map_number];
	door_map_index = new short[doors->door_number];
	map_doors = new short *[map_number]; memset(map_doors, 0, sizeof(short *) * map_number);

	memset(door_number, 0, sizeof(unsigned char) * map_number);
	for(i = 0 ; i < doors->door_number ; i++)
		door_number[doors->data[i].map]++;

	for(i = 0 ; i < map_number ; i++) {
		map_doors[i] = new short[door_number[i]];
		//clear(screen);
		//textprintf(screen, font, 0, 0, COLOR_WHITE, "map:%d, #:%d", i, door_number[i]);
		//pause();
	}
	
	memset(door_number, 0, sizeof(unsigned char) * map_number);
	memset(door_map_index, 0, sizeof(short) * doors->door_number);
	
	for(i = 0 ; i < doors->door_number ; i++) {
		map = doors->data[i].map;
		map_doors[map][door_number[map]] = i;
		door_map_index[i] = door_number[map];
		door_number[map]++;
	}

	for(i = 0 ; i < zone_number ; i++) {
		zones[i].x_pos0 = width + 1;
		zones[i].x_pos1 = -1;
		zones[i].y_pos0 = hieght + 1;
		zones[i].y_pos1 = -1;
	}
	
	for(x = 0 ; x < width ; x++) {
		for(y = 0 ; y < hieght ; y++) {
			if(!data[y * width + x]) continue;
			z = header[data[y * width + x]->get_map()].zone;
			if(x < zones[z].x_pos0) zones[z].x_pos0 = x;
			if(x > zones[z].x_pos1) zones[z].x_pos1 = x;
			if(y < zones[z].y_pos0) zones[z].y_pos0 = y;
			if(y > zones[z].y_pos1) zones[z].y_pos1 = y;
		}
	}
	
	offset =
		sizeof(short) * SHORT_NUMBER +
		sizeof(CALC_ZONE) * zone_number +
		//sizeof(ITEM_LOCATION) * item_number +
		sizeof(CALC_HEADER) * map_number;

	//calc_map_size = 0;

	for(i = 0 ; i < zone_number ; i++) {
		if(zones[i].x_pos1 >= 0) {
			zones[i].width = zones[i].x_pos1 - zones[i].x_pos0 + 1;
			zones[i].hieght = zones[i].y_pos1 - zones[i].y_pos0 + 1;
			calc_zones[i].width = flip_short(zones[i].width);
			calc_zones[i].hieght = flip_short(zones[i].hieght);
			//calc_zones[i].offset = flip_short(offset);
			memcpy(calc_zones[i].name, zone_names + i * 20, 20);
			//calc_map[i] = new unsigned char [zones[i].width * zones[i].hieght];
			//memset(calc_map[i], 81, zones[i].width * zones[i].hieght);
			
			//calc_map_size += (zones[i].width * zones[i].hieght);
			//offset += (zones[i].width * zones[i].hieght);
		}
	}

	for(i = 0 ; i < map_number ; i++) {
		char load_universal = 0;
		
		//clear(screen);
		//textprintf(screen, font, 0, 0, COLOR_WHITE, "map%d, doors:%d", i, door_number[i]);
		//pause();

		x_pos1 = y_pos1 = 0;
		x_pos0 = y_pos0 = -1;
		screen_num[i] = 0;
		z = header[i].zone;

		for(x = 0 ; x < width ; x++)
			for(y = 0 ; y < hieght ; y++) {
				if(!data[y * width + x]) continue;
				if(data[y * width + x]->get_map() != i) continue;
				
				if(x_pos0 < 0) x_pos0 = x;
				if(y_pos0 < 0) y_pos0 = y;
				if(x < x_pos0) x_pos0 = x;
				if(y < y_pos0) y_pos0 = y;
				if(x > x_pos1) x_pos1 = x;
				if(y > y_pos1) y_pos1 = y;
				screen_num[i]++;
			}

		sprintf(file_name, "temp%d.bin", i);
		sprintf(output_name, "temp%d.pck", i);
		temp = fopen(file_name, "wb");

		screen_hdr[i] = new CALC_SCREEN_HEADER[screen_num[i]];
		s = 0;

		for(x = x_pos0 ; x <= x_pos1 ; x++) {
			for(y = y_pos0 ; y <= y_pos1 ; y++) {
				
				if(!data[y * width + x]) continue;
				if(data[y * width + x]->get_map() != i) continue;

				flags = data[y * width + x]->scroll_flags;
				if(get_screen_map(x - 1, y) != i) flags |= SCREEN_LEFT;
				if(get_screen_map(x + 1, y) != i) flags |= SCREEN_RIGHT;
				if(get_screen_map(x, y - 1) != i) flags |= SCREEN_UP;
				if(get_screen_map(x, y + 1) != i) flags |= SCREEN_DOWN;
				door_flags = get_door_flags(x, y);

				//calc_map[z][(y - zones[z].y_pos0) * zones[z].width + (x - zones[z].x_pos0)] =
				//	map_offsets[flags * 16 + door_flags];
				
				screen_hdr[i][s].x_pos = flip_short((x - x_pos0));
				screen_hdr[i][s].y_pos = flip_short((y - y_pos0));
				screen_hdr[i][s].map_room = flip_short(data[y * width + x]->map_room);
				screen_hdr[i][s].scroll_flags = flags;
				screen_hdr[i][s].map_tile = map_offsets[flags * 16 + door_flags];

				s++;

				for(x1 = 0 ; x1 < SCREEN_WIDTH ; x1++) {
					for(y1 = 0 ; y1 < SCREEN_HIEGHT ; y1++) {
						tile = data[y * width + x]->get_tile(x1, y1, TILE_GFX);
						prop = data[y * width + x]->get_tile(x1, y1, TILE_PROPS) - 1;
						//if(prop >= prop_bg_offset) prop = ((prop - prop_bg_offset) | 0x80);
						if(tile >= MAX_TILES || ((prop & 0x7f) >= 4 && (prop & 0x7f) <= 59))
							load_universal = 1;
						//data[y * width + x]->set_tile(x1, y1, prop + 1, TILE_PROPS);
						/*if((tile >= MAX_TILES && tile < MAX_TILES + MAX_ITEMS) ||
							(prop >= FIRST_HIDDEN_ITEM - 1 && prop <= LAST_HIDDEN_ITEM - 1)) {
								if(prop >= FIRST_HIDDEN_ITEM - 1 && prop <= LAST_HIDDEN_ITEM - 1) {
									tmp_screen.prop_data[y1 * SCREEN_WIDTH + x1] = 25; //BREAK_SHOOT
									item_data[current_item].item = prop;
									item_data[current_item].hidden_flag = 1;
								} else {
									tmp_screen.data[y1 * SCREEN_WIDTH + x1] = 0;
									item_data[current_item].item = tile;
									item_data[current_item].hidden_flag = 0;
									calc_map[z][(y - zones[z].y_pos0) * zones[z].width + (x - zones[z].x_pos0)] |= 0x80;
								}

								item_data[current_item].map = flip_short(i);
								item_data[current_item].x_pos =
									flip_short(x1 + (x - x_pos0) * SCREEN_WIDTH);
								item_data[current_item].y_pos =
									flip_short(y1 + (y - y_pos0) * SCREEN_HIEGHT);
								current_item++;
						}*/
					}
				}

				memcpy(tmp_screen.data,
					data[y * width + x]->get_data(), SCREEN_WIDTH * SCREEN_HIEGHT);
				memcpy(tmp_screen.prop_data,
					data[y * width + x]->get_prop_data(), SCREEN_WIDTH * SCREEN_HIEGHT);

				fwrite(&tmp_screen, sizeof(CALC_SCREEN_DATA), 1, temp);

			}
		}

		fclose(temp);

		tool_params[0] = ""; // nothing - will be skipped internally
		tool_params[1] = file_name;
		tool_params[2] = output_name;

		if (TTPack(3,tool_params)) {
			// add error handling here!
		}

		temp = fopen(output_name, "rb");
		fseek(temp,0,SEEK_END);
		pack_size[i] = ftell(temp);
		fclose(temp);

		enemy_data[i] = new ENEMY[header[i].enemies->number];
		for(e = 0 ; e < header[i].enemies->number ; e++){
			ENEMY *enemy = header[i].enemies->data + e;
			ENEMY_SIZE *size = enemy_size + enemy->type;
			short ex = (enemy->x + (x_offset - x_pos0 * SCREEN_WIDTH)) * 12;
			short ey = (enemy->y + (y_offset - y_pos0 * SCREEN_HIEGHT)) * 12;
		
			if(enemy->type == 18) {
					ey -= size->y_off; ex -= size->x_off;
			} else if(size->width != 0 && size->hieght != 0) {
				if(enemy->gravity == DOWN) {
					ey = ey - size->hieght - size->y_off; ex -= size->x_off;
				} else if(enemy->gravity == UP) {
					ey = ey + 12 - (size->frame_hieght - size->hieght - size->y_off); ex -= size->x_off;
				} else if(enemy->gravity == RIGHT) {
					ex = ex - size->hieght - size->y_off; ey -= size->x_off;
					//ex = ex - size->width - (size->frame_width - size->width - size->x_off); ey -= size->y_off;
				} else if(enemy->gravity == LEFT) {
					ex = ex + 12 - size->y_off; ey -= size->x_off;
					//ex = ex + 12 - (size->frame_hieght - size->hieght - size->y_off); ey -= size->y_off;
				}
			}

			enemy_data[i][e].type = header[i].enemies->data[e].type;
			enemy_data[i][e].subtype = header[i].enemies->data[e].subtype;
			enemy_data[i][e].x = flip_short(ex);
			enemy_data[i][e].y = flip_short(ey);

			if(header[i].enemies->data[e].index == 0) {
				enemy_data[i][e].index = flip_short(current_enemy);
				current_enemy++;
			} else
				enemy_data[i][e].index = flip_short(-1);

			switch(header[i].enemies->data[e].direction) {
			case LEFT:
				enemy_data[i][e].direction = CLEFT; break;
			case RIGHT:
				enemy_data[i][e].direction = CRIGHT; break;
			case UP:
				enemy_data[i][e].direction = CUP; break;
			case DOWN:
				enemy_data[i][e].direction = CDOWN; break;
			}
			
			switch(header[i].enemies->data[e].gravity) {
			case LEFT:
				enemy_data[i][e].gravity = CLEFT; break;
			case RIGHT:
				enemy_data[i][e].gravity = CRIGHT; break;
			case UP:
				enemy_data[i][e].gravity = CUP; break;
			case DOWN:
				enemy_data[i][e].gravity = CDOWN; break;
			}

		}

		pipe_data[i] = new PIPE[header[i].pipes->number];
		for(e = 0 ; e < header[i].pipes->number ; e++) {
			pipe_data[i][e].x =
				flip_short(header[i].pipes->data[e].x + x_offset - x_pos0 * SCREEN_WIDTH);
			pipe_data[i][e].y =
				flip_short(header[i].pipes->data[e].y + y_offset - y_pos0 * SCREEN_HIEGHT);
			pipe_data[i][e].activation = header[i].pipes->data[e].activation;
			pipe_data[i][e].direction = header[i].pipes->data[e].direction;
			pipe_data[i][e].init_state = header[i].pipes->data[e].init_state;
			pipe_data[i][e].length = header[i].pipes->data[e].length;
			pipe_data[i][e].switch_pos = header[i].pipes->data[e].switch_pos;
			pipe_data[i][e].auto_delay = flip_short(header[i].pipes->data[e].auto_delay);
			pipe_data[i][e].speed = header[i].pipes->data[e].speed;
			pipe_data[i][e].prox = header[i].pipes->data[e].prox;
			pipe_data[i][e].toggle = header[i].pipes->data[e].toggle;
		}

		item_data[i] = new ITEM[header[i].items->number];
		for(e = 0 ; e < header[i].items->number ; e++) {
			x = header[i].items->data[e].x + x_offset;
			y = header[i].items->data[e].y + y_offset;
			sx = x / SCREEN_WIDTH;
			sy = y / SCREEN_HIEGHT;
			
			item_data[i][e].x = flip_short((x - x_pos0 * SCREEN_WIDTH));
			item_data[i][e].y = flip_short((y - y_pos0 * SCREEN_HIEGHT));

			item_data[i][e].type = header[i].items->data[e].type - 1;
			item_data[i][e].index = flip_short(current_item);
			current_item++;
			
			if(data[sy * width + sx]->get_tile(x % SCREEN_WIDTH, y % SCREEN_HIEGHT, TILE_GRAB) == 0)
				set_map_item(sx - x_pos0, sy - y_pos0, screen_hdr[i], screen_num[i]);
				//calc_map[z][(sy - zones[z].y_pos0) * zones[z].width + (sx - zones[z].x_pos0)] |= 0x80;
		}

		door_data[i] = new CALC_DOOR[door_number[i]];
		
		//clear(screen);
		//textprintf(screen, font, 0, 0, COLOR_WHITE, "before door loop");
		//pause();

		for(e = 0 ; e < door_number[i] ; e++) {
			short index = map_doors[i][e];
			
			//clear(screen);
			//textprintf(screen, font, 0, 0, COLOR_WHITE, "door:%d index:%d", e, index);
			//pause();
			
			door_data[i][e].x =
				flip_short(doors->data[index].x + x_offset - x_pos0 * SCREEN_WIDTH);
			
			door_data[i][e].y =
				flip_short(doors->data[index].y + y_offset - y_pos0 * SCREEN_HIEGHT);
			
			door_data[i][e].target_map =
				flip_short(doors->data[doors->data[index].target_door].map);
			
			door_data[i][e].target_door =
				flip_short(door_map_index[doors->data[index].target_door]);

			door_data[i][e].type = doors->data[index].type;

			door_data[i][e].level = doors->data[index].level;

			if(door_data[i][e].level >= 2 && door_data[i][e].level <= 4) {
				door_data[i][e].index = flip_short(current_door);
				current_door++;
			} else
				door_data[i][e].index = -1;
		}
		
		/*if(offset % 2) offset++;
		calc_header[i].screen_offset = flip_short(offset);
		offset += pack_size[i] + sizeof(CALC_SCREEN_HEADER) * screen_num[i];

		if(offset % 2) offset++;
		calc_header[i].enemy_offset = flip_short(offset);
		offset += sizeof(ENEMY) * header[i].enemies->number;

		if(offset % 2) offset++;
		calc_header[i].pipe_offset = flip_short(offset);
		offset += sizeof(PIPE) * header[i].pipes->number;

		if(offset % 2) offset++;
		calc_header[i].item_offset = flip_short(offset);
		offset += sizeof(ITEM) * header[i].items->number;

		if(offset % 2) offset++;
		calc_header[i].door_offset = flip_short(offset);
		offset += sizeof(CALC_DOOR) * door_number[i];*/

		old_offset = offset;

		if(offset % 2) offset++;
		screen_offset = offset;
		offset += pack_size[i] + sizeof(CALC_SCREEN_HEADER) * screen_num[i];

		if(offset % 2) offset++;
		enemy_offset = offset;
		offset += sizeof(ENEMY) * header[i].enemies->number;

		if(offset % 2) offset++;
		pipe_offset = offset;
		offset += sizeof(PIPE) * header[i].pipes->number;

		if(offset % 2) offset++;
		item_offset = offset;
		offset += sizeof(ITEM) * header[i].items->number;

		if(offset % 2) offset++;
		door_offset = offset;
		offset += sizeof(CALC_DOOR) * door_number[i];

		if(screen_offset > TT_MAX_OTHDATA || enemy_offset > TT_MAX_OTHDATA ||
		pipe_offset > TT_MAX_OTHDATA || item_offset > TT_MAX_OTHDATA ||
		door_offset > TT_MAX_OTHDATA || offset > TT_MAX_OTHDATA) {
			//clear(screen);
			//textprintf(screen, font, 0, 0, COLOR_WHITE, "current_file %d", current_file);
			//textprintf(screen, font, 0, 10, COLOR_WHITE, "map %d", i);
			//textprintf(screen, font, 0, 40, COLOR_WHITE, "offset %d", offset);
			//pause();
			
			offset -= screen_offset;
			enemy_offset -= screen_offset;
			pipe_offset -= screen_offset;
			item_offset -= screen_offset;
			door_offset -= screen_offset;
			screen_offset -= screen_offset;
			file_size[current_file] = old_offset;
			current_file++;
		}
		
		calc_header[i].file = current_file;
		calc_header[i].screen_offset = flip_short(screen_offset);
		calc_header[i].enemy_offset = flip_short(enemy_offset);
		calc_header[i].pipe_offset = flip_short(pipe_offset);
		calc_header[i].item_offset = flip_short(item_offset);
		calc_header[i].door_offset = flip_short(door_offset);

		//if(i == 0) {
		//	clear(screen);
		//	textprintf(screen, font, 0, 0, COLOR_WHITE, "1:%lu", screen_offset);
		//	pause();
		//}

		calc_header[i].width = flip_short((x_pos1 - x_pos0 + 1));
		calc_header[i].hieght = flip_short((y_pos1 - y_pos0 + 1));
		calc_header[i].x_pos = flip_short(x_pos0 - zones[z].x_pos0);
		calc_header[i].y_pos = flip_short(y_pos0 - zones[z].y_pos0);
		calc_header[i].zone = header[i].zone;
		calc_header[i].tileset = header[i].tileset;
		calc_header[i].background = header[i].background;
		calc_header[i].bg_wave = header[i].bg_wave;
		calc_header[i].load_universal = load_universal;
		calc_header[i].environment = header[i].environment;
		calc_header[i].water_start =
			flip_short((y_pos1 - y_pos0 + 1) * 12 * SCREEN_HIEGHT - header[i].water_start);
		calc_header[i].water_end =
			flip_short((y_pos1 - y_pos0 + 1) * 12 * SCREEN_HIEGHT - header[i].water_end);
		calc_header[i].water_speed = flip_short(header[i].water_speed);
		calc_header[i].water_type = header[i].water_type;
		calc_header[i].special_x = flip_short(header[i].special_x);
		calc_header[i].special_y = flip_short(header[i].special_y);
		calc_header[i].map_room = flip_short(header[i].map_room);
		calc_header[i].special_type = header[i].special_type;
		calc_header[i].screen_number = screen_num[i];
		calc_header[i].enemy_number = header[i].enemies->number;
		calc_header[i].pipe_number = header[i].pipes->number;
		calc_header[i].item_number = header[i].items->number;
		calc_header[i].door_number = door_number[i];
		
		if(header[i].special_type != SPECIAL_NONE) {
			sx = header[i].special_x / SCREEN_WIDTH + x_pos0;
			sy = header[i].special_y / SCREEN_HIEGHT + y_pos0;
			set_map_tile(sx - x_pos0, sy - y_pos0, 81 + header[i].special_type,
				screen_hdr[i], screen_num[i]);
			//calc_map[z][(sy - zones[z].y_pos0) * zones[z].width + (sx - zones[z].x_pos0)] =
			//	81 + header[i].special_type;
		}
		
		/*clear(screen);
		textprintf(screen, font, 0, 0, COLOR_WHITE, "xpos0: %d, ypos0: %d, xpos1: %d, ypos1: %d", x_pos0, y_pos0, x_pos1, y_pos1);
		textprintf(screen, font, 0, 10, COLOR_WHITE, "pack_size[i]: %d", pack_size[i]);
		textprintf(screen, font, 0, 20, COLOR_WHITE, "door_number[i]: %d", door_number[i]);
		textprintf(screen, font, 0, 30, COLOR_WHITE, "enemy_number[i]: %d", header[i].enemies->number);
		textprintf(screen, font, 0, 40, COLOR_WHITE, "screen_num: %d", screen_num);

		while(!key[KEY_SPACE]);
		while(key[KEY_SPACE]);*/

	}

	file_size[current_file] = offset;

	buffer = new unsigned char *[current_file + 1];

	for(i = 0 ; i < current_file + 1 ; i++) {
		buffer[i] = new unsigned char[file_size[i]];
		//clear(screen);
		//textprintf(screen, font, 0, 40, COLOR_WHITE, "%d", file_size[i]);
		//pause();
	}
	
	//buffer = new unsigned char[offset];

	offset = sizeof(short) * SHORT_NUMBER;
	
	*(short *)(buffer[0] + 0) = flip_short((short)(current_file + 1));
	*(short *)(buffer[0] + 2) = flip_short(zone_number);
	*(short *)(buffer[0] + 4) = flip_short(map_number);
	*(short *)(buffer[0] + 6) = flip_short(screen_number);
	*(short *)(buffer[0] + 8) = flip_short(item_number);
	*(short *)(buffer[0] + 10) = flip_short(current_enemy);
	*(short *)(buffer[0] + 12) = flip_short(current_door);
	*(short *)(buffer[0] + 14) = flip_short(offset); offset += sizeof(CALC_ZONE) * zone_number;
	*(short *)(buffer[0] + 16) = flip_short(offset); //offset += sizeof(ITEM_LOCATION) * item_number;
	//*(short *)(buffer + 10) = flip_short(offset);

	offset = sizeof(short) * SHORT_NUMBER;

	memcpy(buffer[0] + offset, calc_zones, sizeof(CALC_ZONE) * zone_number);
	offset += sizeof(CALC_ZONE) * zone_number;

	//for(i = 0 ; i < item_number ; i++)
	//	memcpy((ITEM_LOCATION *)(buffer + offset) + i, item_data + i, sizeof(ITEM_LOCATION));
	//offset += item_number * sizeof(ITEM_LOCATION);
	
	for(i = 0 ; i < map_number ; i++)
		memcpy((CALC_HEADER *)(buffer[0] + offset) + i, calc_header + i, sizeof(CALC_HEADER));
	offset += sizeof(CALC_HEADER) * map_number;

	//for(i = 0 ; i < zone_number ; i++) {
	//	if(calc_map[i] == 0) continue;
	//	memcpy(buffer + offset, calc_map[i], zones[i].width * zones[i].hieght);
	//	offset += zones[i].width * zones[i].hieght;
	//}

	for(i = 0 ; i < map_number ; i++) {
		offset = flip_short(calc_header[i].screen_offset);
		offset &= 0x0000ffff;

		memcpy((CALC_SCREEN_HEADER *)(buffer[calc_header[i].file] + offset), screen_hdr[i],
			screen_num[i] * sizeof(CALC_SCREEN_HEADER));
		offset += screen_num[i] * sizeof(CALC_SCREEN_HEADER);

		sprintf(output_name, "temp%d.pck", i);
		temp = fopen(output_name, "rb");
		fread(buffer[calc_header[i].file] + offset, pack_size[i], 1, temp);
		fclose(temp);
		offset += pack_size[i];

		if(offset % 2) offset++;
		memcpy((ENEMY *)(buffer[calc_header[i].file] + offset), enemy_data[i],
			header[i].enemies->number * sizeof(ENEMY));
		offset += header[i].enemies->number * sizeof(ENEMY);

		if(offset % 2) offset++;
		memcpy((PIPE *)(buffer[calc_header[i].file] + offset), pipe_data[i],
			header[i].pipes->number * sizeof(PIPE));
		offset += header[i].pipes->number * sizeof(PIPE);

		if(offset % 2) offset++;
		memcpy((ITEM *)(buffer[calc_header[i].file] + offset), item_data[i],
			header[i].items->number * sizeof(ITEM));
		offset += header[i].items->number * sizeof(ITEM);
		
		if(offset % 2) offset++;
		memcpy((CALC_DOOR *)(buffer[calc_header[i].file] + offset), door_data[i],
			door_number[i] * sizeof(CALC_DOOR));
		//offset += door_number[i] * sizeof(CALC_DOOR);

		/*if(offset % 2) offset++;
		memcpy((CALC_SCREEN_HEADER *)(buffer + offset), screen_hdr[i],
			screen_num[i] * sizeof(CALC_SCREEN_HEADER));
		offset += screen_num[i] * sizeof(CALC_SCREEN_HEADER);

		sprintf(output_name, "temp%d.pck", i);
		temp = fopen(output_name, "rb");
		fread(buffer + offset, pack_size[i], 1, temp);
		fclose(temp);
		offset += pack_size[i];
		
		if(offset % 2) offset++;
		memcpy((ENEMY *)(buffer + offset), enemy_data[i], header[i].enemies->number * sizeof(ENEMY));
		offset += header[i].enemies->number * sizeof(ENEMY);

		if(offset % 2) offset++;
		memcpy((PIPE *)(buffer + offset), pipe_data[i], header[i].pipes->number * sizeof(PIPE));
		offset += header[i].pipes->number * sizeof(PIPE);

		if(offset % 2) offset++;
		memcpy((ITEM *)(buffer + offset), item_data[i], header[i].items->number * sizeof(ITEM));
		offset += header[i].items->number * sizeof(ITEM);
		
		if(offset % 2) offset++;
		memcpy((CALC_DOOR *)(buffer + offset), door_data[i], door_number[i] * sizeof(CALC_DOOR));
		offset += door_number[i] * sizeof(CALC_DOOR);*/
	}

#ifdef MAPCONV_NATIVE
	// Native port: write each per-file blob raw (this is exactly the var-data
	// the game's file_pointer() hands to map_setup). outfile_name is a dir.
	for(i = 0 ; i < current_file + 1 ; i++) {
		if(i == 0) sprintf(current_file_name, "%s/mtlevel.raw", outfile_name);
		else       sprintf(current_file_name, "%s/mtlevel%d.raw", outfile_name, i);
		temp = fopen(current_file_name, "wb");
		fwrite(buffer[i], file_size[i], 1, temp);
		fclose(temp);
		printf("mapconv: wrote %s (%u bytes)\n", current_file_name, file_size[i]);
	}
#else
	strcpy(current_file_name, outfile_name);
	strcpy(oncalc_name, "mtlevel");

	//"timdemo2.89y";

	for(i = 0 ; i < current_file + 1 ; i++) {
		oth_buffer =
			DataBuffer2OTHBuffer(CALC_TI89, 0, oncalc_name, "MTD", file_size[i], buffer[i], &oth_length);
		temp = fopen(current_file_name, "wb");
		fwrite(oth_buffer, oth_length, 1, temp);
		fclose(temp);
		delete oth_buffer;

		if(i == 0) {
			current_file_name[strlen(outfile_name) - 4] = '0';
			current_file_name[strlen(outfile_name) - 3] = '.';
			current_file_name[strlen(outfile_name) - 2] = '8';
			current_file_name[strlen(outfile_name) - 1] = '9';
			current_file_name[strlen(outfile_name) + 0] = 'y';
			current_file_name[strlen(outfile_name) + 1] = 0;
			oncalc_name[7] = '0';
			oncalc_name[8] = 0;
		}
		oncalc_name[7]++;
		current_file_name[strlen(outfile_name) - 4]++;
	}
#endif

	for(i = 0 ; i < map_number ; i++){
		//clear(screen);
		//textprintf(screen, font, 0, 0, COLOR_WHITE, "%d", i);
		//pause();
		if(screen_hdr && screen_hdr[i]) delete screen_hdr[i];
		if(enemy_data && enemy_data[i]) delete enemy_data[i];
		if(item_data && item_data[i]) delete item_data[i];
		if(door_data && door_data[i]) delete door_data[i];
		if(map_doors && map_doors[i]) delete map_doors[i];
		if(pipe_data && pipe_data[i]) delete pipe_data[i];
	}
	
	//if(calc_map) {
	//	for(i = 0 ; i < zone_number ; i++) {
	//		if(calc_map[i]) delete calc_map[i];
	//	}
	//	delete calc_map;
	//}
	
	if(screen_hdr) delete screen_hdr;
	if(door_data) delete door_data;
	if(zones) delete zones;
	if(calc_zones) delete calc_zones;
	if(map_doors) delete map_doors;
	if(door_number) delete door_number;
	if(door_map_index) delete door_map_index;
	if(calc_header) delete calc_header;
	if(enemy_data) delete enemy_data;
	if(pipe_data) delete pipe_data;
	if(item_data) delete item_data;
	if(pack_size) delete pack_size;
	if(buffer) {
		for(i = 0 ; i < current_file + 1 ; i++) delete buffer[i];
		delete buffer;
	}
	
	/*clear(screen);
	textprintf(screen, font, 0, 0, COLOR_WHITE, "width:%d, hieght:%d", width, hieght);
	textprintf(screen, font, 0, 10, COLOR_WHITE, "screen_number:%d", screen_number);
	textprintf(screen, font, 0, 20, COLOR_WHITE, "camera_x:%d, camera_y:%d", camera_x, camera_y);
	textprintf(screen, font, 0, 30, COLOR_WHITE, "x_offset:%d, y_offset:%d", x_offset, y_offset);
	textprintf(screen, font, 0, 40, COLOR_WHITE, "map_number:%d, current_map:%d", map_number, current_map);
	data[0]->draw(screen, &tiles[0], 0, 50, 50);
	while(!key[KEY_SPACE]);
	while(key[KEY_SPACE]);*/
	
}



