#ifndef __BACKGROUND__
#define __BACKGROUND__

#include "screen.h"

typedef struct {
	unsigned char data[SCREEN_WIDTH * SCREEN_HIEGHT];
} BACKGROUND;


class background {
	BACKGROUND **data;
	void background_create();
public:
	short background_number;
	background();
	~background();
	void set_tile(short i, short x, short y, unsigned char a);
	unsigned char get_tile(short i, short x, short y);
	void draw(BITMAP *buffer, short i, short x, short y, tileset *tiles, short tile_mode);
	unsigned char *get_data(short i);
	void write_data(FILE *file);
	void read_data(FILE *file);
};

#endif
