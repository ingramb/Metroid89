#define DOOR_LEFT 0
#define DOOR_RIGHT 1
#define DOOR_UP 2
#define DOOR_DOWN 3
#define ELEVATOR_UP 4
#define ELEVATOR_DOWN 5

#include "tiles.h"

typedef struct {
	short x;
	short y;
	short map;
	char type;
	char level;
	short target_door;
} DOOR;

typedef struct {
	short x;
	short y;
	short map;
	char type;
	short target_door;
} OLD_DOOR;

typedef struct {
	short x;
	short y;
	char type;
	char level;
	short index;
	short target_map;
	short target_door;
} CALC_DOOR;

class door {
	short link;
	tileset *gfx;
	tileset *elevator_gfx;
	tileset *icon_gfx;
public:
	DOOR *data;	
	short door_number;
	
	door();
	~door();
	void new_door(short x, short y, short map, char type);
	void draw_doors(BITMAP *bmp, short camera_x, short camera_y);
	void draw_doors_select(BITMAP *bmp, short x, short y, short selected_door);
	void door_properties(short x, short y);
	void delete_door(short index);
	void save_data(FILE *outfile);
	void load_data(FILE *infile);
};