#include <allegro.h>
#include <string.h>
#include <stdio.h>
#include "tiles.h"
#include "enemy.h"
#include "doors.h"
#include "screen.h"
#include "map.h"
#include "background.h"

background::background()
{
	background_number = 0;
	data = 0;
}

background::~background()
{
	short i;

	for(i = 0 ; i < background_number ; i++)
		if(data[i]) delete data[i];
	delete data;
}

void background::background_create()
{
	BACKGROUND **new_data;
	short i;

	new_data = new BACKGROUND *[background_number + 1];
	
	if(data) {
		for(i = 0 ; i < background_number ; i++) new_data[i] = data[i];
		delete data;
	}
	data = new_data;
	background_number++;
	data[background_number - 1] = new BACKGROUND;
	memset(data[background_number - 1]->data, 0, SCREEN_WIDTH * SCREEN_HIEGHT);
}

void background::set_tile(short i, short x, short y, unsigned char a)
{
	while(i >= background_number) background_create();
	data[i]->data[y * SCREEN_WIDTH + x] = a;
}

unsigned char background::get_tile(short i, short x, short y)
{
	if(i >= background_number) return 0;
	return data[i]->data[y * SCREEN_WIDTH + x];
}

void background::draw(BITMAP *buffer, short i, short x, short y, tileset *tiles, short tile_mode)
{
	short x1, y1;
	unsigned char tile;
	
	if(i >= background_number) return;
	
	for(x1 = 0 ; x1 < SCREEN_WIDTH ; x1++) {
		for(y1 = 0 ; y1 < SCREEN_WIDTH ; y1++) {
			tile = data[i]->data[y1 * SCREEN_WIDTH + x1];
			if(tile >= 0) {
				
					draw_lit_rle_sprite(buffer, tiles->get_tile(tile),
						x1 * 12 + x, y1 * 12 + y, 200);
			}
		}
	}
}

unsigned char *background::get_data(short i)
{
	return data[i]->data;
}

void background::write_data(FILE *file)
{
	short i;
	
	fwrite(&background_number, sizeof(short), 1, file);
	for(i = 0 ; i < background_number ; i++)
		fwrite(data[i]->data, SCREEN_WIDTH * SCREEN_HIEGHT, 1, file);
}

void background::read_data(FILE *file)
{
	short i;

	fread(&background_number, sizeof(short), 1, file);
	data = new BACKGROUND *[background_number];
	for(i = 0 ; i < background_number ; i++) {
		data[i] = new BACKGROUND;
		fread(data[i]->data, SCREEN_WIDTH * SCREEN_HIEGHT, 1, file);
	}
}


