// C Header File
// Created 6/13/02; 3:41:57 PM

#ifndef __BOMB__
#define __BOMB__

#define NORMAL_BOMB 0
#define POWER_BOMB 1
#define POWER_BOMB_EXPLODE 2
#define BOMB_MAX 3

typedef struct {
	short x;
	short y;
	char type;
	char frame;
	char frame_counter;
	char duration;
	unsigned long enemy_hit;
} BOMB;

extern char bombs_placed;

void bomb_reset();
void bomb_create(short x, short y, char type);
void bomb_process();
void bomb_draw();

#endif

