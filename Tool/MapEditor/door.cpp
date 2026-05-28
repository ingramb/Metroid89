#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include "tiles.h"
#include "screen.h"
#include "enemy.h"
#include "doors.h"
#include "map.h"

#define COLOR_BLACK makecol(0, 0, 0)
#define COLOR_LINK makecol(0, 128, 0)
#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_RED makecol(255, 0, 0)

door::door()
{
	BITMAP *doors_bmp;
	RGB pal[256];
	
	data = 0;
	link = (-1);
	door_number = 0;

	gfx = 0; elevator_gfx = 0; icon_gfx = 0;

	doors_bmp = load_bitmap("c:/calculator/metroid gfx/doors_small.bmp", pal);
	gfx = new tileset();
	gfx->load_tiles(doors_bmp, 48, 48);
	destroy_bitmap(doors_bmp);

	doors_bmp = load_bitmap("c:/calculator/metroid gfx/elevator.bmp", pal);
	elevator_gfx = new tileset();
	elevator_gfx->load_tiles(doors_bmp, 24, 12);
	destroy_bitmap(doors_bmp);

	doors_bmp = load_bitmap("c:/calculator/metroid gfx/door_icons.bmp", pal);
	icon_gfx = new tileset();
	icon_gfx->load_tiles(doors_bmp, 12, 12);
	destroy_bitmap(doors_bmp);
}

door::~door()
{
	if(data) delete data;
	
	if(gfx) delete gfx;
	if(elevator_gfx) delete elevator_gfx;
	if(icon_gfx) delete icon_gfx;
}

void door::new_door(short x, short y, short map, char type)
{
	DOOR *new_door;
	short i;

	new_door = new DOOR[door_number + 1];
	
	if(data) {
		for(i = 0 ; i < door_number ; i++)
			memcpy(new_door + i, data + i, sizeof(DOOR));
	
		delete data;
	}

	data = new_door;

	door_number++;

	data[door_number - 1].x = x;
	data[door_number - 1].y = y;
	data[door_number - 1].map = map;
	data[door_number - 1].level = 0;

	if(link < 0) {
		link = door_number - 1;
		data[door_number - 1].type = type;
		data[door_number - 1].target_door = -1;
	} else {
		data[door_number - 1].target_door = link;

		data[link].target_door = door_number - 1;

		if(data[link].type == DOOR_LEFT) data[door_number - 1].type = DOOR_RIGHT;
		else if(data[link].type == DOOR_RIGHT) data[door_number - 1].type = DOOR_LEFT;
		else if(data[link].type == DOOR_UP) data[door_number - 1].type = DOOR_DOWN;
		else if(data[link].type == DOOR_DOWN) data[door_number - 1].type = DOOR_UP;
		else if(data[link].type == ELEVATOR_UP) data[door_number - 1].type = ELEVATOR_DOWN;
		else if(data[link].type == ELEVATOR_DOWN) data[door_number - 1].type = ELEVATOR_UP;

		link = (-1);
	}
}

void door::door_properties(short x, short y)
{
	short index = -1;
	short i;

	DIALOG d[] = {
		{d_box_proc, 0, 0, 240, 120, COLOR_BLACK, COLOR_WHITE, 0, 0,	0, 0, NULL, NULL, NULL},
		{d_text_proc, 5, 5, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Type:", NULL, NULL},
		{d_radio_proc, 5, 15, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Normal", NULL, NULL},
		{d_radio_proc, 5, 25, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Invisible", NULL, NULL},
		{d_radio_proc, 5, 35, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Missile", NULL, NULL},
		{d_radio_proc, 5, 45, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Super Missile", NULL, NULL},
		{d_radio_proc, 5, 55, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Enemy remember", NULL, NULL},
		{d_radio_proc, 5, 65, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Enemy reset", NULL, NULL},
		{d_radio_proc, 5, 75, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Bomb switch", NULL, NULL},
		{d_radio_proc, 5, 85, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Sealed", NULL, NULL},

		{d_button_proc,	5,	100,	55, 15,	COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0,	0, "OK", NULL, NULL},
		{d_button_proc,	65,	100,	55,	15,	COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0,	0, "NO", NULL, NULL},
		{d_button_proc,	125, 100, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "DELETE", NULL, NULL},
		{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
	};
	
	for(i = 0 ; i < door_number ; i++)
		if(x == data[i].x && y == data[i].y) {
			index = i;
			break;
		}

	if(index < 0) return;

	if(data[index].type >= 4) {
		delete_door(index);
		return;
	}

	d[2 + data[index].level].flags |= D_SELECTED;

	centre_dialog(d);

	do_dialog(d, 2);

	if(d[11].flags & D_GOTFOCUS) return;
	if(d[12].flags & D_GOTFOCUS) {
		delete_door(index);
		return;
	}

	for(i = 0 ; i < 8 ; i++) {
		if(d[i + 2].flags & D_SELECTED) data[index].level = i;
	}
}

void door::delete_door(short index)
{
	short next_index;
	short loop;
	short i;
	DOOR *new_data;

	if(index == link) link = -1;

	next_index = data[index].target_door;
	if(next_index > index) next_index--;

	for(loop = 0 ; loop < 2 ; loop++){

		for(i = 0 ; i < door_number ; i++)
			if(data[i].target_door > index) data[i].target_door--;
		if(link > index) link--;

		new_data = new DOOR[door_number - 1];

		for(i = 0 ; i < index ; i++)
			memcpy(new_data + i, data + i, sizeof(DOOR));
		for(i = index + 1 ; i < door_number ; i++)
			memcpy(new_data + i - 1, data + i, sizeof(DOOR));

		door_number--;

		delete data;
		data = new_data;

		if(next_index < 0) break;
		else index = next_index;

	}
}

void door::draw_doors_select(BITMAP *bmp, short x, short y, short selected_door)
{
	int i;

	for(i = 0 ; i < 4 ; i++) {
		draw_rle_sprite(bmp, gfx->get_tile(i), x + 48 * i, y);
		if(i == selected_door) rect(bmp, x + 48 * i, y, x + 48 * i + 47, y + 47, COLOR_RED);
	}
	for(i = 4 ; i < 6 ; i++) {
		draw_rle_sprite(bmp, elevator_gfx->get_tile(i - 4), x + 48 * i, y);
		if(i == selected_door) rect(bmp, x + 48 * i, y, x + 48 * i + 47, y + 47, COLOR_RED);
	}
	
	
	if(selected_door != 6)
		rect(bmp, x + 48 * 6, y, x + 48 * 6 + 47, y + 47, COLOR_WHITE);
	else
		rect(bmp, x + 48 * 6, y, x + 48 * 6 + 47, y + 47, COLOR_RED);
}

void door::draw_doors(BITMAP *bmp, short camera_x, short camera_y)
{
	int i;
	char *links_done = new char[door_number];
	char level;
	
	memset(links_done, 0, door_number * sizeof(char));

	drawing_mode(DRAW_MODE_TRANS, 0, 0, 0);
	
	for(i = 0 ; i < door_number ; i++) {
		if(data[i].type < 4) {
			level = data[i].level;
			if(level > 4) level = 4;
		
			draw_rle_sprite(bmp, gfx->get_tile(level * 4 + data[i].type),
				(data[i].x - camera_x) * 12, (data[i].y - camera_y) * 12);
			
			draw_rle_sprite(bmp, icon_gfx->get_tile(data[i].level),
						(data[i].x - camera_x) * 12, (data[i].y - camera_y) * 12);
		} else
			draw_rle_sprite(bmp, elevator_gfx->get_tile(data[i].type - 4),
				(data[i].x - camera_x) * 12, (data[i].y - camera_y) * 12);
			
		
		//textprintf(bmp, font, (data[i].x - camera_x) * 12 + 13, (data[i].y - camera_y) * 12,
		//	COLOR_WHITE, "%d", data[i].map);
	
		if(data[i].target_door >=0 && !links_done[i]) {
			links_done[data[i].target_door] = 1;
			line(bmp,
				(data[i].x - camera_x) * 12,
				(data[i].y - camera_y) * 12,
				(data[data[i].target_door].x - camera_x) * 12,
				(data[data[i].target_door].y - camera_y) * 12,
				COLOR_LINK);
		}
		
	}

	drawing_mode(DRAW_MODE_SOLID, 0, 0, 0);
}

void door::save_data(FILE *outfile)
{
	fwrite(&door_number, sizeof(short), 1, outfile);
	fwrite(&link, sizeof(short), 1, outfile);
	if(door_number) fwrite(data, sizeof(DOOR), door_number, outfile);
}

void door::load_data(FILE *infile)
{
	//short i;
	//OLD_DOOR *old_data;
	
	if(data) delete data;
	
	fread(&door_number, sizeof(short), 1, infile);
	fread(&link, sizeof(short), 1, infile);
	
	//old_data = new OLD_DOOR[door_number];
	//fread(old_data, sizeof(OLD_DOOR), door_number, infile);
	//data = new DOOR[door_number];
	//for(i = 0 ; i < door_number ; i++) {
	//	data[i].map = old_data[i].map;
	//	data[i].target_door = old_data[i].target_door;
	//	data[i].type = old_data[i].type;
	//	data[i].x = old_data[i].x;
	//	data[i].y = old_data[i].y;
	//	data[i].level = 0;
	//}
	
	if(door_number) {
		data = new DOOR[door_number];
		fread(data, sizeof(DOOR), door_number, infile);
		//for(short i = 0 ; i < door_number ; i++) {
		//	if(data[i].level >= 1) data[i].level++;
		//}
	} else
		data = 0;
}