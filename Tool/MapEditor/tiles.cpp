#include <allegro.h>
#include "tiles.h"

#define COLOR_SELECTED makecol(255, 0, 0)

bool tile_is_empty(BITMAP *bmp)
{
	int mask = makecol(255, 0, 255);
	for(short x = 0 ; x < bmp->w ; x++) {
		for(short y = 0 ; y < bmp->h ; y++) {
			if(getpixel(bmp, x, y) != mask) return false;
		}
	}

	return true;
}

tileset::tileset()
{
	tiles = 0;
	tile_number = 0;
}

tileset::~tileset()
{
	short i;

	for(i = 0 ; i < tile_number ; i++)
		destroy_rle_sprite(tiles[i]);

	delete tiles;
}

void tileset::load_tiles(BITMAP *bmp, short width, short hieght)
{
	load_tiles(bmp, width, hieght, false);
}

void tileset::load_tiles(BITMAP *bmp, short width, short hieght, bool skip_empty)
{
	BITMAP *temp;
	short x_pos = 0;
	short y_pos = 0;
	short i = 0, j = 0;
	short last_not_empty = 0;
	short max_number = (bmp->w * bmp->h) / (width * hieght);
	bool empty;

	temp = create_bitmap(width, hieght);
	tiles = new RLE_SPRITE *[max_number];
	twidth = width;
	thieght = hieght;

	for(i = 0 ; i < max_number ; i++){
		//if(i == 0){
		//	clear(temp);
		//	tiles[i] = get_compiled_sprite(temp, false);
		//	continue;
		//}
		
		blit(bmp, temp, x_pos, y_pos, 0, 0, width, hieght);
		empty = tile_is_empty(temp);
		if(!empty) last_not_empty = i;
		if(!empty || !skip_empty) {
			tiles[j] = get_rle_sprite(temp);
			j++;
		}
		x_pos += width;
		if(x_pos + width > bmp->w){
			x_pos = 0;
			y_pos += hieght;
		}
	}

	destroy_bitmap(temp);
	if(!skip_empty) {
		for(i = last_not_empty + 1 ; i < max_number ; i++) destroy_rle_sprite(tiles[i]);
		tile_number = last_not_empty + 1;
	} else tile_number = j;
}

bool tileset::add_tileset(tileset *add, short offset)
{
	RLE_SPRITE **new_tiles;
	BITMAP *temp;
	short i;
	
	if(twidth != add->twidth && thieght != add->thieght) return false;

	new_tiles = new RLE_SPRITE *[tile_number + add->tile_number - offset];
	temp = create_bitmap(twidth, thieght);

	for(i = 0 ; i < tile_number ; i++) new_tiles[i] = tiles[i];
	for(i = 0 ; i < add->tile_number - offset ; i++) {
		clear_to_color(temp, makecol(255, 0, 255));
		draw_rle_sprite(temp, add->tiles[i + offset], 0, 0);
		new_tiles[i + tile_number] = get_rle_sprite(temp);
	}
	destroy_bitmap(temp);
	delete tiles;
	tiles = new_tiles;
	tile_number = tile_number + add->tile_number - offset;

	return true;
}

RLE_SPRITE *tileset::get_tile(short i)
{
	if(i >= 0 && i < tile_number) return tiles[i];
	else return tiles[0];
}

void tileset::draw_small(BITMAP *bmp, short width, short selected, short xpos, short ypos)
{
	BITMAP *temp;
	short x = xpos;
	short y = ypos;
	short j = 0;
	short i;

	for(i = 0 ; i < tile_number ; i++) {
		draw_rle_sprite(bmp, get_tile(i), x, y);
		
		if(i == selected) rect(bmp, x, y, x + twidth - 1, y + thieght - 1, COLOR_SELECTED);
		x += twidth;
		j++;
		if(j == width){
			x = xpos;
			j = 0;
			y += thieght;
		}
	}
}

void tileset::draw(BITMAP *bmp, short width, short selected, short xpos, short ypos)
{
	BITMAP *temp;
	short x = xpos;
	short y = ypos;
	short j = 0;
	short i;

	temp = create_bitmap(twidth, thieght);

	for(i = 0 ; i < tile_number ; i++){
		clear(temp);
		draw_rle_sprite(temp, get_tile(i), 0, 0);
		stretch_sprite(bmp, temp, x, y, twidth * 2, thieght * 2);
		
		if(i == selected) rect(bmp, x, y, x + twidth * 2 - 1, y + thieght * 2 - 1, COLOR_SELECTED);
		x += twidth * 2;
		j++;
		if(j == width){
			x = xpos;
			j = 0;
			y += thieght * 2;
		}
	}

	destroy_bitmap(temp);
}
