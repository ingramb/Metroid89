// C Header File
// Created 7/15/02; 7:17:40 PM

#ifndef __DEFINES
#define __DEFINES

#define NAME_LENGTH 14
#define INVENTORY_SIZE 8

#define ELEM_NUMBER 8

enum elements {
	ELEM_DAMAGE,
	ELEM_MAGIC,
	ELEM_FIRE,
	ELEM_ICE,
	ELEM_THUNDER,
	ELEM_QUAKE,
	ELEM_DARK,
	ELEM_HOLY
};

enum resist {
	RESIST_NORMAL,
	RESIST_ABSORB,
	RESIST_IMMUNE,
	RESIST_STRONG,
	RESIST_WEAK
};

#define STAT_NUMBER 7

enum stats {
	STAT_HP,
	STAT_STR,
	STAT_AGL,
	STAT_DEF,
	STAT_MAG
};

#endif