// C Header File
// Created 11/3/2002; 1:25:30 PM

#ifndef __POWERUP__
#define __POWERUP__

typedef struct {
	short x;
	short y;
	short type;
	short duration;
	short signal;
	char frame;
	char frame_counter;

	short next;
} POWERUP;

typedef struct {
	short width;
	short hieght;
} POWERUP_DATA;

enum {
	POWERUP_HEALTH,
	POWERUP_MISSILE,
	POWERUP_SUPERMISSILE,
	POWERUP_POWERBOMB,
};

#define POWERUP_MAX 10
#define POWERUP_DURATION 800
#define POWERUP_ANIM_SPEED 3

char powerup_setup();
void powerup_reset();
void powerup_cleanup();
short powerup_create(short x, short y, short type);
void powerup_delete(short a);
void powerup_set_signal(short a, short signal);
void powerup_process();
void powerup_draw();

#endif

