// C Source File
// Created 9/9/2002; 7:04:09 PM

#include <tigcclib.h>         // Include All Header Files
#include "utility.h"
#include "particle.h"

short *table = NULL;

void particle_setup()
{
	SYM_ENTRY *sym_ptr;
	char *data;
	short *file;
	float a;
	short i;
	
	table = (short *)malloc(sizeof(short) * 256);
	
	file = file_pointer(SYMSTR("trig"));
	
	if(file == NULL) {
		for(i = 0 ; i < 256 ; i++) {
			a = (float)i * ((float)360 / (float)256);
			table[i] = (short)(sin(a * PI/(float)180) * (float)1024);
		}
	
		sym_ptr = DerefSym(SymAdd(SYMSTR("trig")));
		sym_ptr->handle = HeapAlloc(256 * sizeof(short) + 2 + 7);
		data = HeapDeref(sym_ptr->handle);
		*(short *)data = 256 * sizeof(short) + 7;
		memcpy(data + 2, table, 256 * sizeof(short));
		*(unsigned char *)(data + 256 * sizeof(short) + 2) = 0;
		*(unsigned char *)(data + 256 * sizeof(short) + 3) = 'T';
		*(unsigned char *)(data + 256 * sizeof(short) + 4) = 'R';
		*(unsigned char *)(data + 256 * sizeof(short) + 5) = 'I';
		*(unsigned char *)(data + 256 * sizeof(short) + 6) = 'G';
		*(unsigned char *)(data + 256 * sizeof(short) + 7) = 0;
		*(unsigned char *)(data + 256 * sizeof(short) + 8) = OTH_TAG;
	} else
		memcpy(table, file, 256 * sizeof(short));
}

void particle_cleanup()
{
	if(table != NULL) {
		free(table);
		table = NULL;
	}
}

#define table_sin(i) (table[(i) & 255])
#define table_cos(i) (table[((i) + 64) & 255])
//#define absolute(i) ((i) > 0 ? (i) : -(i))

void particle_init(PARTICLE *p, short x, short y, short angle, unsigned short speed)
{
	p->x = x;
	p->y = y;
	p->angle = angle;
	p->speed = speed;
	p->x_counter = 0;
	p->y_counter = 0;
	p->speed_counter = 0;
}


void particle_step(PARTICLE *p)
{
	short x_change;
	short y_change;
	
	char x_dir;
	char y_dir;
	
	x_change = table_cos(p->angle);
	y_change = -table_sin(p->angle);
	x_dir = ((x_change>=0) ? 1 : (-1));
	y_dir = ((y_change>=0) ? 1 : (-1));
	
	p->speed_counter += p->speed;
	
	while(p->speed_counter >= 100) {
		
		if(absolute(x_change) >= absolute(y_change)) {
			p->x += x_dir;
			p->y_counter += absolute(y_change);
			if(p->y_counter > p->x_counter) {
				p->y += y_dir;
				p->x_counter += absolute(x_change);
			}
		} else {
			p->y += y_dir;
			p->x_counter += absolute(x_change);
			if(p->x_counter > p->y_counter){
				p->x += x_dir;
				p->y_counter += absolute(y_change);
			}
		}
		
		p->speed_counter -= 100;
	}
	
	while(p->x_counter > 16384 || p->y_counter > 16384) {
		p->x_counter = p->x_counter << 1;
		p->y_counter = p->y_counter << 1;
	}
}


