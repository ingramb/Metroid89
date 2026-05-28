// C Header File
// Created 7/15/02; 6:59:21 PM

#ifndef __INVENTORY
#define __INVENTORY

#include "defines"

enum inventory_types {
	TYPE_EMPTY,
	TYPE_ABILITY,
	TYPE_EQUIPABLE,
	TYPE_MODULE
};

typedef struct {
	unsigned short	index;
	unsigned char		uses;
	char						type;
} INVENTORY_SLOT;

enum targets {
	TARG_SINGLE_ENEMY,
	TARG_SINGLE_ALLY,
	TARG_ENEMY_GROUP,
	TARG_ALLY_GROUP,
	TARG_ALL_ENEMY,
	TARG_ALL_ALLY
};

typedef struct {
	unsigned char	uses;
	unsigned char	stat;
	unsigned char	element;
	unsigned char	target;
	unsigned char	hit;
	unsigned char	damage;
} ATTACK;

typedef struct {
	char						name[NAME_LENGTH];
	unsigned short	attack;
} ABILITY;

enum equipable_types {
	EQUIP_USABLE,
	EQUIP_BODY,
	EQUIP_HAT,
	EQUIP_GLOVE
};

typedef struct {
	char						name[NAME_LENGTH];
	unsigned char		icon;
	unsigned short	cost;
	char						type;
	char						repair;
	unsigned short	attack;
	unsigned char		stat_bonus[STAT_NUMBER];
	unsigned char		robot_stat_bonus[STAT_NUMBER];
	unsigned char		resists[ELEM_NUMBER];
} EQUIBABLE;

const char *slot_get_name(INVENTORY_SLOT *slot);
	
#endif