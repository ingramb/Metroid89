// C Header File
// Created 1/20/02; 5:03:14 PM

#ifndef __SHOTS__
#define __SHOTS__

#include "entity.h"

typedef struct {
	SPRITE_HEADER *header;
	char gfx;
	char frame;
	char frame_counter;
	char frame_direction;
	short type;
	short combo;
	unsigned short combo_id;
	short x;
	short y;
	short tile_x;
	short tile_y;
	char x_off;
	char y_off;
	short width;
	short hieght;
	short angle;
	short delta_x;
	short delta_y;
	char x_direction;
	char y_direction;
	char wave_range;
	char wave_type;
	short decay_counter;
	unsigned short speed;
	short x_speed_counter;
	short y_speed_counter;
	short target;
	short target_counter;
	char player;
	char remove;
	char animation_counter;
	unsigned long enemy_hit;

	short next;
} SHOT;

typedef struct {
	const char frames[3];
	char pattern;
	unsigned short damage_type;
	char width;
	char hieght;
	char diagonal;
	char diag_width;
	unsigned short initial_speed;
	unsigned short top_speed;
	char spawn_animation;
	char anim_spawn_delay;
	char death_animation;
	char special;
	unsigned freeze : 1;
	unsigned penetrate : 1;
} SHOT_DATA;

typedef struct {
	short shot_type;
	char pattern_type;
	short damage;
	char wave;
} SHOT_COMBO;

typedef struct {
	char player;
	char remove;
	short x;
	short y;
	char gfx;
	short angle;
	short width;
	short hieght;
	short type;
	short combo;
	unsigned short id;
} SHOT_ARGS;


enum {
	COMBO_NORMAL,
	COMBO_WAVE,
	COMBO_SPREAD,
	COMBO_SPLIT,
	COMBO_TRISPLIT,
};

enum {
	WAVE_ONCE,
	WAVE_CONT,
	WAVE_DECAY,
};

enum {
	DAMAGE_NORMAL = 				1 << 0,
	DAMAGE_MISSILE = 				1 << 1,
	DAMAGE_SUPERMISSILE = 	1 << 2,
	DAMAGE_POWERBOMB = 			1 << 3,
	DAMAGE_CHARGE = 				1 << 4,
	DAMAGE_PLASMA = 				1 << 5,
	DAMAGE_CHARGE_PLASMA = 	1 << 6,
	DAMAGE_BOMB = 					1 << 7,
	DAMAGE_SPEED = 					1 << 8,
	DAMAGE_STAND = 					1 << 9,
	DAMAGE_CHARGE_SCREW = 	1 << 10,
	DAMAGE_SCREW = 					1 << 11,
	DAMAGE_ENEMY_KILL = 		1 << 12,
	DAMAGE_BOMB_SWITCH = 		1 << 13,
	DAMAGE_ALL = 						1 << 14,
};

enum {
	STYPE_NORMAL,
	STYPE_WAVE,
	STYPE_ICE,
	STYPE_SPAZER,
	STYPE_SPAZER_ICE,
	STYPE_PLASMA,
	STYPE_PLASMA_ICE,
	STYPE_CHARGE,
	STYPE_CHARGE_ICE,
	STYPE_CHARGE_SPAZER,
	STYPE_CHARGE_SPAZER_ICE,
	STYPE_CHARGE_PLASMA,
	STYPE_CHARGE_PLASMA_ICE,
	STYPE_MISSILE,
	STYPE_SUPERMISSILE,
	STYPE_HOMING_MISSILE,
	STYPE_ENERGY_LEACH,
	STYPE_ENEMY_PLASMA,
	STYPE_ENEMY_ACID,
	STYPE_ENEMY_SHOCKWAVE,
	STYPE_ENEMY_SPIKE,
	STYPE_ENEMY_WAVE,
	STYPE_ENEMY_FIREBALL,
	STYPE_ENEMY_FIREBLIP,

	STYPE_END_OF_LIST,
};

enum {
	SHOT_NORMAL,
	SHOT_NORMAL_WAVE,
	SHOT_NORMAL_ICE,
	SHOT_NORMAL_ICE_WAVE,

	SHOT_SPAZER,
	SHOT_SPAZER_WAVE,
	SHOT_SPAZER_ICE,
	SHOT_SPAZER_ICE_WAVE,

	SHOT_PLASMA,
	SHOT_PLASMA_WAVE,
	SHOT_PLASMA_ICE,
	SHOT_PLASMA_ICE_WAVE,

	SHOT_NORMAL_CHARGE,
	SHOT_NORMAL_CHARGE_WAVE,
	SHOT_NORMAL_CHARGE_ICE,
	SHOT_NORMAL_CHARGE_ICE_WAVE,

	SHOT_SPAZER_CHARGE,
	SHOT_SPAZER_CHARGE_WAVE,
	SHOT_SPAZER_CHARGE_ICE,
	SHOT_SPAZER_CHARGE_ICE_WAVE,

	SHOT_PLASMA_CHARGE,
	SHOT_PLASMA_CHARGE_WAVE,
	SHOT_PLASMA_CHARGE_ICE,
	SHOT_PLASMA_CHARGE_ICE_WAVE,

	SHOT_MISSILE,
	SHOT_SUPERMISSLE,
	SHOT_HOMING_MISSILE,
	SHOT_ENERGY_LEACH,

	SHOT_ENEMY_PLASMA,
	SHOT_ENEMY_ACID,
	SHOT_ENEMY_SHOCKWAVE,
	SHOT_ENEMY_SPIKE,
	SHOT_ENEMY_WAVE,
	SHOT_ENEMY_FIREBALL,
	SHOT_ENEMY_FIREBLIP,

	SHOT_END_OF_LIST,
};

enum SHOT_SPECIAL {
	ANGLE_DECAY,
	HEAT_SEAKING,
	LEACH,
};

#define TARGET_NEEDED -3
#define TARGET_PLAYER -2
#define SHOT_MAX 30

#ifndef __IN_DLL__
//extern SHOT *shots;
//extern SHOT_DATA *shot_data;

char shot_setup();
void shot_reset();
void shot_cleanup();
void shot_create(char player, char remove, short x, short y, char direction, short combo);
void shot_create_angle(short x, short y, short angle, short combo);
void shot_create_directed(short x, short y, short x_target, short y_target, short combo);
short get_angle(short x, short y, short x_target, short y_target);
void shot_untarget(short e);
void shots_process();
void shots_draw();
unsigned short get_combo_id();
#endif

#endif
