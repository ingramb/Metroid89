#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include "enemy.h"

#define COLOR_BLACK makecol(0, 0, 0)
#define COLOR_WHITE makecol(255, 255, 255)
#define COLOR_RED makecol(255, 0, 0)

enum {
	LEFT,
	DOWN,
	RIGHT,
	UP
};

ENEMY_SIZE enemy_size[] = {
	{-2, 0, 140, 61, 140, 61, FALSE},
	{12, 18, 16, 36, 40, 56, TRUE},
	{0, 12, 24, 40, 32, 56, FALSE},
	{3, 5, 12, 12, 18, 18, TRUE},
	{4, 4, 16, 20, 24, 24, FALSE},
	{4, 11, 22, 19, 30, 30, FALSE},
	{12, 25, 50, 68, 72, 96, TRUE},
	{5, 4, 11, 11, 24, 24, TRUE},
	{0, 1, 23, 12, 24, 24, TRUE},
	{19, 20, 20, 26, 66, 46, TRUE},
	{7, 16, 12, 16, 24, 36, FALSE},
	{9, 0, 18, 48, 36, 48, FALSE},
	{22, 19, 28, 65, 72, 84, TRUE},
	{3, 4, 20, 9, 24, 12, TRUE},
	{12, 0, 88, 228, 112, 228, FALSE},
	{1, 13, 21, 12, 23, 26, FALSE},
	{0, 0, 0, 0, 0, 0, FALSE},
	{10, 2, 12, 12, 32, 26, FALSE},
	{8, 8, 14, 32, 30, 40, FALSE},
};

/*struct {
	short x_off;
	short y_off;
	short width;
	short hieght;
} enemy_data[] = {
	{10, 12, 20, 40},
	{0, 6, 24, 46},
	{1, 5, 16, 12}
};*/

enemy::enemy()
{
	number = 0;
	data = 0;
}

enemy::~enemy()
{
	delete data;
}

void enemy::new_enemy(short x, short y, short type)
{
	ENEMY *new_data;
	int i;
	
	for(i = 0 ; i < number ; i++)
		if(data[i].x == x && data[i].y == y && data[i].type == type) return;

	new_data = new ENEMY[number + 1];
	for(i = 0 ; i < number ; i++)
		memcpy(new_data + i, data + i, sizeof(ENEMY));
	
	number++;

	new_data[number - 1].x = x;
	new_data[number - 1].y = y;
	new_data[number - 1].gravity = DOWN;
	new_data[number - 1].direction = RIGHT;
	new_data[number - 1].type = type;
	new_data[number - 1].subtype = 0;
	new_data[number - 1].index = -1;

	delete data;
	data = new_data;
}

void enemy::modify_enemy(short x, short y)
{
	char type_char[5];
	char stype_char[5];

	DIALOG d[] = {
		{d_box_proc, 0, 0, 240, 150, COLOR_BLACK, COLOR_WHITE, 0, 0,	0, 0, NULL, NULL, NULL},
		
		{d_text_proc, 5, 5, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Type:", NULL, NULL},
		{d_edit_proc, 85, 5, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, type_char, NULL, NULL},

		{d_text_proc, 5, 15, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "SubType:", NULL, NULL},
		{d_edit_proc, 85, 15, 38, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 3, 0, stype_char, NULL, NULL},

		{d_text_proc, 5, 25, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Direction:", NULL, NULL},
		{d_radio_proc, 5, 35, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Left", NULL, NULL},
		{d_radio_proc, 5, 45, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Right", NULL, NULL},

		{d_text_proc, 85, 25, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Gravity:", NULL, NULL},
		{d_radio_proc, 85, 35, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Left", NULL, NULL},
		{d_radio_proc, 85, 45, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Down", NULL, NULL},
		{d_radio_proc, 85, 55, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Right", NULL, NULL},
		{d_radio_proc, 85, 65, 20, 8, COLOR_BLACK, COLOR_WHITE, 0, 0, 1, 0, "Up", NULL, NULL},
		{d_check_proc, 5, 85, 150, 10, COLOR_BLACK, COLOR_WHITE, 0, 0, 0, 0, "Stay dead", NULL, NULL},

		{d_button_proc,	5,	130,	55, 15,	COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0,	0, "OK", NULL, NULL},

		{d_button_proc,	65,	130,	55,	15,	COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0,	0, "NO", NULL, NULL},

		{d_button_proc,	125, 130, 55, 15, COLOR_BLACK, COLOR_WHITE, 0, D_EXIT, 0, 0, "DELETE", NULL, NULL},
		
		{NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
	};
	
	short i;
	short index = -1;

	for(i = 0 ; i < number ; i++)
		if(x == (data + i)->x && y == (data + i)->y) {
			index = i;
			break;
		}
	
	if(index < 0) return;

	memset(type_char, 0, 5);
	memset(stype_char, 0, 5);
	sprintf(type_char, "%d", (data + index)->type);
	sprintf(stype_char, "%d", (data + index)->subtype);

	if(data[index].direction == LEFT) d[6].flags |= D_SELECTED;
	else d[7].flags |= D_SELECTED;

	d[9 + data[index].gravity].flags |= D_SELECTED;

	if(data[index].index == 0) d[13].flags |= D_SELECTED;

	centre_dialog(d);

	do_dialog(d, 2);

	if(d[15].flags & D_GOTFOCUS) return;
	if(d[16].flags & D_GOTFOCUS) {
		delete_enemy(index);
		return;
	}

	i = atoi(type_char);
	if(i >=0 && i <= 10) data[index].type = i;
	i = atoi(stype_char);
	if(i >= 0) data[index].subtype = i;

	if(d[6].flags & D_SELECTED) data[index].direction = LEFT;
	else data[index].direction = RIGHT;

	if(d[13].flags & D_SELECTED) data[index].index = 0;
	else data[index].index = -1;
	
	for(i = 0 ; i < 4 ; i++)
		if(d[9 + i].flags & D_SELECTED) {
			data[index].gravity = i;
			break;
		}
}

void enemy::delete_ship()
{
	short i;

	for(i = 0 ; i < number ; i++)
		if(data[i].type == 0) {
			delete_enemy(i);
			return;
		}
}

void enemy::delete_enemy(short index)
{
	ENEMY *new_data;
	short i;

	new_data = new ENEMY[number - 1];

	for(i = 0 ; i < index ; i++)
		memcpy(new_data + i, data + i, sizeof(ENEMY));
	for(i = index + 1 ; i < number ; i++)
		memcpy(new_data + i - 1, data + i, sizeof(ENEMY));

	number--;

	delete data;
	data = new_data;
}

void enemy::draw(BITMAP *buffer, short camera_x, short camera_y, tileset *pics)
{
	int i;
	short x;
	short y;

	for(i = 0 ; i < number ; i++){
		x = (data[i].x - camera_x) * 12;
		y = (data[i].y - camera_y) * 12;

		rect(buffer, x, y, x + 11, y + 11, COLOR_WHITE);

		if(data[i].gravity == 0) { //LEFT
			y -= 2;
			x += 12;
		} else if(data[i].gravity == 1) { //DOWN
			x -= 2;
			y -= 16;
		} else if(data[i].gravity == 2) { //RIGHT
			y -= 2;
			x -= 16;
		} else { //UP
			x -= 2;
			y += 12;
		}

		draw_rle_sprite(buffer, pics->get_tile(data[i].type), x, y);
	}
}

void enemy::save_data(FILE *outfile)
{
	fwrite(&number, sizeof(short), 1, outfile);
	if(number) fwrite(data, sizeof(ENEMY), number, outfile);
}

void enemy::load_data(FILE *infile)
{
	//OLD_ENEMY *old_data;
	//short i;

	delete data;
	fread(&number, sizeof(short), 1, infile);
	if(number) {
		data = new ENEMY[number];
		fread(data, sizeof(ENEMY), number, infile);
		
		//for(short i = 0 ; i < number ; i++) {
		//	if(data[i].type == 10) data[i].type = 0;
		//	else data[i].type++;
		//}

	} else
		data = 0;

	/*old_data = new OLD_ENEMY[number];
	fread(old_data, sizeof(OLD_ENEMY), number, infile);
	data = new ENEMY[number];
	for(i = 0 ; i < number ; i++) {
		data[i].type = old_data[i].type;
		data[i].subtype = old_data[i].subtype;
		data[i].gravity = old_data[i].gravity;
		data[i].direction = old_data[i].direction;
		data[i].index = -1;
		data[i].x = old_data[i].x;
		data[i].y = old_data[i].y;
	}
	delete old_data;*/
}
