// C Header File
// Created 7/15/02; 7:39:30 PM

#ifndef __CHARACTER
#define __CHARACTER

#include "defines.h"
#include "inventory.h"

typedef struct {
	unsigned short offset;
	char x_offset;
	char y_offset;
	char width;
	char hieght;
} SPRITE_HEADER;

typedef struct {
	char						name[NAME_LENGTH];
	unsigned char		gfx;
	unsigned short	stats[STAT_NUMBER];
	INVENTORY_SLOT	inventory[INVENTORY_SIZE];
} CHARACTER;






#endif