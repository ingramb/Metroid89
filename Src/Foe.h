// C Header File
// Created 7/17/02; 10:00:27 PM

#ifndef __FOE
#define __FOE

typedef struct {
	unsigned short	enemies[3];
	unsigned short	number[3];
} ENCOUNTER;

typedef struct {
	CHARACTER				stats;
	unsigned short	exp;
	unsigned short	gold;
} ENEMY;

void enemy_draw(ENEMY *e, char x_pos);

#endif