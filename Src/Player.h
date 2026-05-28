// C Header File
// Created 1/8/02; 10:56:50 PM

#ifndef __PLAYER__
#define __PLAYER__

#include "entity.h"

typedef struct {
	ENTITY e;
	char direction;
	short screen_x;
	short screen_y;
	short global_screen_x;
	short global_screen_y;
	short hp;
	short hp_max;
	unsigned char ammunition_max[3];
	unsigned char ammunition[3];
	unsigned long items_found;
	unsigned long items_equiped;
	char gun_position;
	char weapon_selected;
	char action;
	unsigned char action_type;
	char input_disabled;
	unsigned char frame;
	short frame_counter;
	short action_duration;
	char invinc_duration;
	char flash;

	char overide_action;
	unsigned char overide_frame;
	short overide_frame_counter;

	char wall_kick_direction;
	char bounce_direction;

	char sprint_state;
	char sprint_counter;
	char sprint_direction;
	char blur_counter;
	unsigned char turbo_counter;
	short walk_speed;

	short gun_x;
	short gun_y;
	unsigned char charge_counter;
	char charge_level;
	char charge_frame;
	char charge_frame_counter;

	char leach_type;
	short leach_target;
	short leach_counter;

	short jump_hieght;
	short jump_speed;
	char flip_start;
	short fall_distance;
	short rubber_charge;
	char shot_delay;
	char bounced;
	char jumped;
	char in_water;
	char hit_water;
	char hp_drain_counter;
	char hp_drain;
	char y_thresh;
	char y_thresh_base;
	short y_scroll_counter;
	char look_counter;
	short gravity_counter;
	char gravity_cooldown;

	char down_pressed;
	char up_pressed;
} PLAYER;

typedef struct {
	const ENTITY_ANIMATION *animation[6];
	char duration;
	unsigned frame_reset : 1;
	unsigned overide_action : 1;
	unsigned change_direction : 1;
	unsigned input_disabled : 1;
	unsigned char type;
	char size;
	char next_action;
} _PLAYER_ACTION;

typedef struct {
	unsigned short animation[6];
	char duration;
	unsigned frame_reset : 1;
	unsigned overide_action : 1;
	unsigned change_direction : 1;
	unsigned input_disabled : 1;
	char UNUSED;
	unsigned char type;
	char size;
	char next_action;
} PLAYER_ACTION;

#define JUMP_HIEGHT 84
#define SPACE_JUMP_HIEGHT 48
#define HIGHJUMP_HIEGHT 108
#define WALL_KICK_HIEGHT 48
#define FALL_SPEED_START 98
#define FALL_SPEED_MAX 200
#define FALL_SPEED_INCREASE 3

#define WALK_SPEED_START 40
#define WALK_SPEED_MAX 100
#define WALK_SPEED_INCREASE 4
#define WALK_SPEED_DECREASE 4
#define SPRINT_SPEED_MAX 150
#define TURBO_SPEED_MAX 225
#define TURBO_SPEED_INCREASE 1
#define SPRINT_DELAY 10
#define BLUR_DELAY 7
#define Y_THRESH_START 25
#define CHARGE_DELAY 20
#define CHARGE_TIME 40
#define CHARGE_ANIM_SPEED 4
#define HP_DRAIN_DELAY 10

#define FOUND_ITEMS_SIZE 15

enum LEACH_MODE {
	LEACH_NONE,
	LEACH_SHOT,
	LEACH_ENEMY,
};

enum keys {
	LEFT_PRESSED =								1 << 0,
	RIGHT_PRESSED =								1 << 1,
	UP_PRESSED =									1 << 2,
	DOWN_PRESSED =								1 << 3,
	JUMP_PRESSED =								1 << 4,
	SHOOT_PRESSED =								1 << 5,
	DIAG_UP_PRESSED =							1 << 6,
	DIAG_DOWN_PRESSED =						1 << 7,
	BEAM_SELECT_PRESSED =					1 << 8,
	MISSILE_SELECT_PRESSED =			1 << 9,
	SUPERMISSILE_SELECT_PRESSED =	1 << 10,
	POWERBOMB_SELECT_PRESSED =		1 << 11,
	REVERSE_G_SELECT_PRESSED = 		1 << 12,
	MENU_PRESSED = 								1 << 13,
	PLUS_PRESSED = 								1 << 14,
	MINUS_PRESSED = 							1 << 15,
};

enum gun_positions {
	GUN_NONE,
	GUN_FOWARD,
	GUN_UP,
	GUN_DOWN,
	GUN_DIAG_UP,
	GUN_DIAG_DOWN,
};

enum actions {
	ACT_FRONT_WAIT,
	ACT_FRONT_READY,
	ACT_SIDE_WAIT,
	ACT_TURN_LEFT,
	ACT_TURN_RIGHT,
	ACT_DUCK_TURN_LEFT,
	ACT_DUCK_TURN_RIGHT,
	ACT_AIR_TURN_LEFT,
	ACT_AIR_TURN_RIGHT,
	ACT_STAND,
	ACT_HURT,
	ACT_RUN,
	ACT_DUCK_DOWN,
	ACT_DUCK,
	ACT_STAND_UP,
	ACT_BALL_DOWN,
	ACT_BALL,
	ACT_BALL_UP,
	ACT_LAND,
	ACT_FLIP_LAND,
	ACT_FALL,
	ACT_JUMP_UP,
	ACT_JUMP_DOWN,
	ACT_TURBO_JUMP_START,
	ACT_TURBO_JUMP,
	ACT_TURBO_JUMP_END,
	ACT_TURBO_CHARGE,
	ACT_TURBO_CHARGE_END,
	ACT_BOUNCE,
	ACT_BALL_JUMP_UP,
	ACT_BALL_JUMP_DOWN,
	ACT_BALL_BOUNCE,
	ACT_RUBBER_BALL_CHARGE,
	ACT_TURBO_BALL_BOUNCE,
	ACT_FLIP_START,
	ACT_FLIP_JUMP_UP,
	ACT_FLIP_JUMP_DOWN,
	ACT_SPACE_JUMP_START,
	ACT_SPACE_JUMP_UP,
	ACT_SPACE_JUMP_DOWN,
	ACT_WALL_KICK_READY,
	ACT_WALL_KICK,
	ACT_WALL_KICK_SPACE,
	ACT_END_OF_LIST,
};

enum action_types {
	TYPE_STAND = 1,
	TYPE_BALL = 2,
	TYPE_JUMP_UP = 4,
	TYPE_JUMP_DOWN = 8,
	TYPE_FLIP = 16,
	TYPE_SPACE = 32,
	TYPE_BOUNCE = 64,
	TYPE_TURBO = 128
};

enum sizes {
	SIZE_STAND,
	SIZE_DUCK,
	SIZE_BALL
};

enum weapons {
	BEAM = -1,
	MISSILE,
	SUPERMISSILE,
	POWERBOMB,
	HOMING_MISSILE,
	ENERGY_LEACH,
};

enum {
	CHARGE_BEAM,
	ICE_BEAM,
	WAVE_BEAM,
	SPAZER_BEAM,
	PLASMA_BEAM,
	VARIA_SUIT,
	GRAVITY_SUIT,
	MORPHING_BALL,
	BOMBS,
	RUBBER_BALL,
	SPRING_BALL,
	HIJUMP_BOOTS,
	SPEED_BOOTS,
	SPACE_JUMP,
	SCREW_ATTACK,
	HEAT_SEEKERS,
	ENERGY_LEECH,
	FORCE_SHIELD,
	REVERSE_GRAVITY,
};

#ifndef __IN_DLL__
extern unsigned char **explored_map;

extern unsigned char *player_gfx;

extern const unsigned char player_size_mask[];

char player_setup();
void player_cleanup();
void player_init();
void player_set_position(short x, short y);
char player_hurt(short damage, short dx, short dy, short blocker);
void player_death();
void player_step(char direction);
char player_move(char direction, short speed);
void player_screen_up();
void player_screen_down();
void player_jump();
void player_bounce(short hieght, char direction);
void player_shoot();
void player_set_action(short action);
void player_process();
void player_animate();
void player_draw();
//SPRITE_HEADER *player_get_hit_detection();
void player_draw_gravity_bar();

short player_get_input_89();
short player_get_input_92();

void item_menu();
#else

#define player_move (*(char (*)(short, short))(glbs->dll_imports[PLAYER_MOVE]))

#endif

#endif


