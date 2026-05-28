#include "tiles.h"

#define ENEMY_TYPE_MAX 19

typedef struct {
	char x_off;
	char y_off;
	short width;
	short hieght;
	short frame_width;
	short frame_hieght;
	char gravity_rotate;
} ENEMY_SIZE;

typedef struct {
	unsigned char type;
	unsigned char subtype;
	char gravity;
	char direction;
	short x;
	short y;
} OLD_ENEMY;

typedef struct {
	unsigned char type;
	unsigned char subtype;
	char gravity;
	char direction;
	short index;
	short x;
	short y;
} ENEMY;

class enemy {
public:
	ENEMY *data;
	short number;
	enemy();
	~enemy();
	void new_enemy(short x, short y, short type);
	void modify_enemy(short x, short y);
	void delete_enemy(short index);
	void delete_ship();
	void draw(BITMAP *buffer, short camera_x, short camera_y, tileset *pics);
	void save_data(FILE *outfile);
	void load_data(FILE *infile);
};

extern ENEMY_SIZE enemy_size[];
