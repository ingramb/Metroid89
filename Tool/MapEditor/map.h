#include "item.h"
#include "tiles.h"
#include "pipe.h"

#define MAX_TILESET 10
#define MAGIC_A 'M'
#define MAGIC_B 'M'
#define MAGIC_C 'P'
#define MAGIC_D 1

#define TILE_GFX 0
#define TILE_PROPS 1
#define TILE_UNIVERSAL 2
#define TILE_ITEMS 3
#define TILE_GRAB 4

#define MAX_TILES 180

#define TILE_A 0
//#define TILE_A_BG 1
#define TILE_B 1
#define TILE_ITEM_BASE 2
#define TILE_BASE 3

enum {
	SPECIAL_NONE,
	SPECIAL_SAVE,
	SPECIAL_RECHARGE,
	SPECIAL_MAP,
};

typedef struct {
	unsigned short width;
	unsigned short hieght;
	unsigned short x_pos;
	unsigned short y_pos;
	unsigned char zone;
	unsigned char tileset;
	unsigned char background;
	char bg_wave;
	char load_universal;
	unsigned char environment;
	short water_start;
	short water_end;
	short water_speed;
	short special_x;
	short special_y;
	char water_type;
	unsigned char map_room;
	char special_type;
	unsigned char file;
	char UNUSED;
	unsigned char screen_number;
	unsigned char enemy_number;
	unsigned char pipe_number;
	unsigned char item_number;
	unsigned char door_number;
	unsigned short screen_offset;
	unsigned short enemy_offset;
	unsigned short pipe_offset;
	unsigned short item_offset;
	unsigned short door_offset;
} CALC_HEADER;

typedef struct {
	short x_pos0;
	short y_pos0;
	short x_pos1;
	short y_pos1;
	short width;
	short hieght;
} ZONE_HEADER;

typedef struct {
	char name[20];
	short width;
	short hieght;
	//unsigned short offset;
} CALC_ZONE;

typedef struct {
	unsigned short map;
	unsigned short x_pos;
	unsigned short y_pos;
	unsigned char item;
	unsigned char hidden_flag;
} ITEM_LOCATION;

typedef struct {
	unsigned char zone;
	char tileset;

	unsigned char background;
	char bg_wave;

	short special_x;
	short special_y;
	char special_type;
	unsigned short map_room;

	char environment;
	short water_start;
	short water_end;
	short water_speed;
	char water_type;
	enemy *enemies;
	pipe *pipes;
	item *items;
} MAP_HEADER;

class map {
	MAP_HEADER *header;
	map_screen **data;
	tileset tiles[MAX_TILESET + TILE_BASE];
	tileset bg_tiles;
	
	enemy *enemies;
	char item_display;
	short item_counter;
	short width;
	short hieght;
	int x_offset;
	int y_offset;
	char tile_mode;
	short screen_number;
	void resize_grid(int x_off, int y_off, int x_shrink, int y_shrink, int new_width, int new_hieght);
	void crop_grid();
	short get_screen_map(short x, short y);
	unsigned char get_door_flags(short x, short y);
	short find_map(short x, short y);
	char zone_names[50 * 20];
	short player_start_x;
	short player_start_y;
	short player_start_map;
	short prop_bg_offset;
public:
	door *doors;
	void destroy_data();
	short map_number;
	short current_map;
	short camera_x;
	short camera_y;
	short item_number;
	
	map();
	~map();
	void new_map();
	void next_map();
	void prev_map();
	void set_tile(char a, short x, short y);
	RLE_SPRITE *get_tile(short i);
	unsigned char read_tile(short x, short y);
	short get_tile_number();
	void change_tile_mode();
	MAP_HEADER get_header(short i);

	void center_view();
	void select_map(short tile_x, short tile_y);

	void new_pipe(short x, short y);
	
	void new_enemy(short x, short y, short type);
	void modify_enemy(short x, short y);

	void new_door(short x, short y, char type);
	void door_properties(short x, short y);

	void new_item(short x, short y, unsigned char type);
	void delete_item(short x, short y);

	void draw_maps(BITMAP *bmp, tileset *pics, tileset *pipe_pics);
	void draw_tiles(BITMAP *bmp, int width, int selected_tile);
	void draw_doors_select(BITMAP *bmp, short x, short y, short selected_door);
	void printdim(BITMAP *bmp, int x, int y, int color);
	void camera_up();
	void camera_down();
	void camera_left();
	void camera_right();
	void map_properties();
	void set_zone_names();
	void screen_properties(short tile_x, short tile_y);
	void auto_save();
	void save_data(FILE *outfile);
	void load_data(FILE *infile);
	void export_data(char *outfile_name);
};

extern short flip_short(short a);