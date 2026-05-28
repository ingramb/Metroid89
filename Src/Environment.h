// C Header File
// Created 8/28/02; 5:00:15 PM

#ifndef __ENVIRONMENT__
#define __ENVIRONMENT__

#include "player.h"

#define ENVIRONMENT_NONE 0
#define ENVIRONMENT_DARK 1
#define ENVIRONMENT_SNOW 2

#define SNOW_Y_SPEED 1

enum {
	WATER_NONE,
	WATER_NORMAL,
	WATER_LAVA,
	WATER_ACID,
};

typedef struct {
	char frame;
	char anim_counter;
	char type;
	short counter;
	short speed;
	short speed_max;
	short top;
	short target;
	short start;
	short end;
	short cnt;
} WATER;

#ifndef __IN_DLL__

void environment_setup(char, char, short, short, short);
void environment_process(void);
void environment_draw(unsigned char *);
#endif

#endif