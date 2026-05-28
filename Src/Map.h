// C Header File
// Created 1/17/02; 11:00:04 PM

#ifndef __MAP__
#define __MAP__

#include "tiles.h"
#include "door.h"
#include "enemy.h"
#include "dllexports.h"

#define SCREEN_WIDTH 16
#define SCREEN_HIEGHT 16

#define SCREEN_LEFT 1
#define SCREEN_RIGHT 2
#define SCREEN_UP 4
#define SCREEN_DOWN 8

#define MAX_TILES 180
#define MAX_ITEMS 20

#define FOUND_ITEM_SIZE 15

#define BG_NUMBER 5

#define TILE_LIST_CLIPPED_SIZE 32
#define TILE_LIST_SIZE 180

enum {
	SPECIAL_NONE,
	SPECIAL_SAVE,
	SPECIAL_RECHARGE,
	SPECIAL_MAP,
};

typedef struct {
	char name[20];
	short width;
	short hieght;
	//unsigned short offset;
} ZONE_HEADER;

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
} MAP_HEADER;

typedef struct {
	short	x_pos;
	short	y_pos;
	unsigned short	map_room;
	unsigned char	scroll_flags;
	unsigned char	map_tile;
} SCREEN_HEADER;

typedef struct {
	unsigned char		data[SCREEN_WIDTH * SCREEN_HIEGHT];
	char						prop_data[SCREEN_WIDTH * SCREEN_HIEGHT];
} SCREEN_DATA;

typedef struct {
	unsigned char scroll_flags;
} SCREEN_TILE;

typedef struct {
	short x;
	short y;
	unsigned short index;
	unsigned char type;
	unsigned char unused;
} ITEM;

typedef struct {
	short							index;
	unsigned char			zone;
	short 						width;
	short 						hieght;
	unsigned short		x_pos;
	unsigned short		y_pos;
	unsigned char			background;
	char 							bg_wave;
	short							special_x;
	short							special_y;
	short							map_room;
	char							special_type;
	short							screen_width;
	short							screen_hieght;
	short							screen_number;
	SCREEN_HEADER *		screen_header;
	SCREEN_TILE *		 	screen_data;
	unsigned char *		data;
	char *						prop_data;
} CURRENT_MAP;

typedef struct {
	short x;
	short y;
	char x_off;
	char y_off;
	short tile_x;
	short tile_y;

	short bg_tile_x;
	short bg_tile_y;
	short bg_x_off;
	short bg_y_off;
	short bg_x_counter;
	short bg_y_counter;
	char bg_wave;
	char bg_wave_counter;
	char bg_ripple;
	char bg_ripple_counter;
	char bg_x_moved;
	short old_bg_x;
} CAMERA;

typedef struct {
	short counter;
	char active;
	char saving;
	short frame;
	short frame_counter;
} SAVE;

typedef struct {
	short frame;
	short frame_counter;
	short counter;
} MAP_RECHARGE;

typedef union {
	SAVE save;
	MAP_RECHARGE other;
} MAP_SPECIAL;

typedef struct {
	unsigned short *tile;
	unsigned short cnt;
	short offset;
	char h;
	char blit;
} TILE_NODE_CLIPPED;

typedef struct {
	unsigned short tile;
	unsigned short cnt;
	short offset;
} TILE_NODE;

typedef struct {
	short width;
	short hieght;
	short footer_hieght;
	short scroll_x;
	short scroll_y;
	short auto_x;
	short auto_y;
	const unsigned char *data;
	const unsigned char *footer;
} _BACKGROUND_HEADER;

typedef struct {
	short width;
	short hieght;
	short footer_hieght;
	short scroll_x;
	short scroll_y;
	short auto_x;
	short auto_y;
	unsigned short data;
	unsigned short footer;
} BACKGROUND_HEADER;

typedef struct {
	short map_index;
	short player_x;
	short player_y;
	char player_action;
	char player_direction;
	char weapon_selected;
	char BLANK;
	short player_hp;
	short player_hp_max;
	unsigned char ammo[3];
	unsigned char ammo_max[3];
	unsigned long items_found;
	unsigned long items_equiped;
	unsigned long game_time;
	char map_items_found[FOUND_ITEM_SIZE];
	char opened_doors[OPENED_DOORS_SIZE];
	char killed_enemies[KILLED_ENEMIES_SIZE];
} SAVE_GAME;

#ifndef __IN_DLL__
extern char game_name[9];

//void TEST_bg_setup();
//void TEST_bg_draw();
//void TEST_bg_cleanup();

extern ZONE_HEADER *zone_header;
extern short zone_number;
extern char *zone_name;
//extern unsigned char *global_map;
extern short global_map_width;
extern short global_map_hieght;

extern short map_number;
extern short item_number;
extern char item_flash;

extern short current_item_number;
extern unsigned char items_found[FOUND_ITEM_SIZE];
extern const char* const item_names[];
#endif

char map_setup(char *map_file);
void map_cleanup();
void map_unload();
void map_room_active(short z, unsigned short room);
void set_map(short i);
void check_items();
void draw_items();
void draw_special(char p);
void check_special();
char camera_process(char speed);
void camera_process_left(short bound, char speed);
void camera_process_right(short bound, char speed);
void camera_process_up(short bound, char speed);
void camera_process_down(short bound, char speed);
void focus_camera();
void camera_left();
void camera_right();
void camera_up();
void camera_down();
void map_process();
void respawn_player();
void game_write();
char game_save();
char game_load();

#ifndef __IN_DLL__
char bg_setup(void);
void bg_reset(void);
void bg_cleanup(void);
void bg_draw(void);
void draw_map0(void);
void draw_map1(void *, void *, TILE_DATA *, TILE_NODE *, TILE_NODE_CLIPPED *);
unsigned char get_data(short, short);
unsigned char get_prop_data(short, short);
void set_data(short, short, unsigned char);
void set_prop_data(short, short, unsigned char);
//#define tile_list _DLL_glbvar(TILE_NODE *, DLL_TILE_LIST)
//#define tile_list_clipped _DLL_glbvar(TILE_NODE_CLIPPED *, DLL_TILE_LIST_CLIPPED)
#endif

#endif

