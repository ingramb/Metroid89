// C Header File
// Created 9/9/2002; 7:15:21 PM

#ifndef __PARTICLE__
#define __PARTICLE__

typedef struct {
	short angle;
	unsigned short speed;
	short x;
	short y;
	unsigned short x_counter;
	unsigned short y_counter;
	unsigned short speed_counter;
} PARTICLE;

void particle_setup();
void particle_init(PARTICLE *p, short x, short y, short angle, unsigned short speed);
void particle_step(PARTICLE *p);
void particle_cleanup();

#endif

