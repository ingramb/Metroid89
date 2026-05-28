#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include "tiles.h"
#include "item.h"

//void pause()
//{
	//textout(screen, font, "LKJSDFLKASJDLKAj", 0, 0, COLOR_WHITE);
	//while(!key[KEY_SPACE]);
	//while(key[KEY_SPACE]);
	//clear(screen);
//}

item::item()
{
	number = 0;
	data = 0;
}

item::~item()
{
	delete data;
}

bool item::new_item(short x, short y, unsigned char type)
{
	ITEM *new_data;
	int i;
	
	for(i = 0 ; i < number ; i++)
		if(data[i].x == x && data[i].y == y) return false;

	new_data = new ITEM[number + 1];
	memcpy(new_data, data, sizeof(ITEM) * number);
	
	number++;
	
	memset(new_data + number - 1, 0, sizeof(ITEM));
	new_data[number - 1].x = x;
	new_data[number - 1].y = y;
	new_data[number - 1].type = type;
	new_data[number - 1].index = -1;
	
	delete data;
	data = new_data;

	return true;
}

bool item::delete_item(short x, short y)
{
	ITEM *new_data;
	short i;
	short index = -1;

	for(i = 0 ; i < number ; i++) {
		if(data[i].x == x && data[i].y == y) {
			index = i;
			break;
		}
	}
	if(index == -1) return false;

	new_data = new ITEM[number - 1];

	for(i = 0 ; i < index ; i++)
		memcpy(new_data + i, data + i, sizeof(ITEM));
	for(i = index + 1 ; i < number ; i++)
		memcpy(new_data + i - 1, data + i, sizeof(ITEM));

	number--;

	delete data;
	data = new_data;

	return true;
}

unsigned char item::get_item(short x, short y)
{
	short i, index = -1;
	
	for(i = 0 ; i < number ; i++) {
		if(data[i].x == x && data[i].y == y) {
			index = i;
			break;
		}
	}
	if(index == -1) return 0;
	else return data[index].type;
}

void item::draw(BITMAP *buffer, short camera_x, short camera_y, tileset *pics)
{
	short i;

	for(i = 0 ; i < number ; i++){
		draw_rle_sprite(buffer, pics->get_tile(data[i].type),
			(data[i].x - camera_x) * 12, (data[i].y - camera_y) * 12);
	}
}

void item::save_data(FILE *outfile)
{
	fwrite(&number, sizeof(short), 1, outfile);
	if(number) fwrite(data, sizeof(ITEM), number, outfile);
}

void item::load_data(FILE *infile)
{
	delete data;
	fread(&number, sizeof(short), 1, infile);
	if(number) {
		data = new ITEM[number];
		fread(data, sizeof(ITEM), number, infile);	
	} else
		data = 0;
}
