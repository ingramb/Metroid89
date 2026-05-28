// C Header File
// Created 7/28/2003; 7:50:11 PM

#ifndef __PIPE__
#define __PIPE__

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
	PIPE_OPENED,
	PIPE_OPENING,
	PIPE_CLOSING,
};

enum {
	PIPE_OFF = -1,
	PIPE_ONCE = 0,
	PIPE_TOGGLE = 1,
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
} PIPE_HEADER;

typedef struct {
	short x;
	short y;
	short speed;
	short pipe_delay;
	short delay_counter;
	short speed_counter;
	short switch_x;
	short switch_y;
	short tile_x;
	short tile_y;
	char x_off;
	char y_off;
	char current_pos;
	char switch_pos;
	char activation;
	char direction;
	char length;
	char state;
	char init_state;
	char prox;
	char toggle;
	char reversed;
	short block_handle;
} PIPE;

#ifndef __IN_DLL__
char pipe_init(PIPE_HEADER*, short);
void pipe_reset(void);
void pipe_cleanup(void);
void pipe_open(PIPE*, short);
void pipe_process(void);
void pipe_draw(void);
#endif

#endif