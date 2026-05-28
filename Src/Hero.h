// C Header File
// Created 7/17/02; 9:53:47 PM

#ifndef __HERO
#define __HERO

#include "character.h"
#include "defines.h"

typedef struct {
	CHARACTER				stats;
	unsigned short	bonus_stats[STAT_NUMBER];
	unsigned short	stat_exp[STAT_NUMBER];
} HERO;




#endif
	