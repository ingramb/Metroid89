#ifndef __SCREEN__
#define __SCREEN__

#define SCREEN_WIDTH 16
#define SCREEN_HIEGHT 16

#define SCREEN_LEFT 1
#define SCREEN_RIGHT 2
#define SCREEN_UP 4
#define SCREEN_DOWN 8

typedef struct {
	short	x_pos;
	short	y_pos;
	unsigned short	map_room;
	unsigned char	scroll_flags;
	unsigned char	map_tile;
} CALC_SCREEN_HEADER;

typedef struct {
	unsigned char	data[SCREEN_WIDTH * SCREEN_HIEGHT];
	char			prop_data[SCREEN_WIDTH * SCREEN_HIEGHT];
} CALC_SCREEN_DATA;

class map_screen {
	short map;
	unsigned char *data;
	char *prop_data;
public:
	unsigned char scroll_flags;
	short map_room;
	map_screen(short m);
	map_screen(map_screen *old);
	~map_screen();
	bool set_tile(int x, int y, char a, char mode);
	unsigned char get_tile(int x, int y, char mode);
	void properties(short map_number);
	int get_map();
	void draw(BITMAP *buffer, tileset *tiles, tileset *bg_tiles,
		char tileset, char background, short current_map, int x, int y);
	void save_data(FILE *outfile);
	void load_data(FILE *infile);
	unsigned char *get_data();
	char *get_prop_data();
};

#endif