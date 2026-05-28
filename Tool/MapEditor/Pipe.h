#ifndef __PIPE__
#define __PIPE__

#include "tiles.h"

enum {
	PIPE_SHOOT,
	PIPE_MISSILE,
	PIPE_SUPERMISSILE,
	PIPE_BOMB,
	PIPE_PROXIMITY,
	PIPE_AUTO,
};

enum {
	PIPE_CLOSED,
	PIPE_OPEN,
};

typedef struct {
	short x;
	short y;
	short auto_delay;
	unsigned char speed;
	char switch_pos;
	char activation;
	char direction;
	char length;
	char init_state;
	char prox;
	char toggle;
} PIPE;

class pipe {
public:
	PIPE *data;
	short number;
	pipe();
	~pipe();
	void new_pipe(short x, short y);
	void modify_pipe(short index);
	void delete_pipe(short index);
	void draw(BITMAP *buffer, short camera_x, short camera_y, tileset *pics);
	void save_data(FILE *outfile);
	void load_data(FILE *infile);
};

#endif