#ifndef __TILESET__
#define __TILESET__

class tileset {
	RLE_SPRITE **tiles;
	short twidth, thieght;
public:
	short tile_number;
	tileset();
	~tileset();
	void load_tiles(BITMAP *bmp, short width, short hieght);
	void load_tiles(BITMAP *bmp, short width, short hieght, bool skip_empty);
	bool add_tileset(tileset *add, short offset);
	RLE_SPRITE *get_tile(short i);
	void draw_small(BITMAP *bmp, short width, short selected, short xpos, short ypos);
	void draw(BITMAP *bmp, short width, short selected, short xpos, short ypos);
};

#endif