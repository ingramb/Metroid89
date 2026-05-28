// C Header File
// Created 9/9/2002; 7:55:49 PM

#ifndef __SOLDIER__
#define __SOLDIER__

#include "particle.h"

typedef struct {
	PARTICLE p;
	short hp;
} SOLDIER;

void soldier_init(SOLDIER *s, short x, short y, short angle, short hp);
void soldier_process(SOLDIER *s);
void soldier_draw(SOLDIER *s);

#endif
