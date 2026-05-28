// C Source File
// Created 3/12/2003; 7:02:11 PM

#include "utility.h"
#include "enemy.h"
#include "powerup.h"
#include "shots.h"
#include "animation.h"

//===============================================================================
//ENEMY_PIRATE
//===============================================================================
const ENTITY_ANIMATION pirate_walk = {8, 0, {0, 8, 1, 8, 2, 8, 3, 8, 4, 8, 3, 8, 2, 8, 1, 8}};
const ENTITY_ANIMATION pirate_look = {5, 0, {5, 30, 6, 10, 7, 30, 6, 10, 5, 30}};
const ENTITY_ANIMATION pirate_shoot = {7, 0, {6, 8, 8, 8, 9, 8, 10, 40, 9, 8, 8, 8, 6, 32}};
const ENTITY_ANIMATION pirate_hit = {1, 0, {11, 1}};
//===============================================================================
//ENEMY_CLIMBER
//===============================================================================
const ENTITY_ANIMATION climber_climb = {4, 0, {5, 12, 4, 12, 5, 12, 6, 12}};
const ENTITY_ANIMATION climber_shoot = {2, 0, {0, 12, 1, 60}};
const ENTITY_ANIMATION climber_shooting_done = {2, 0, {0, 12, 5, 24}};
const ENTITY_ANIMATION climber_jump = {2, 0, {2, 12, 3, 0}};
const ENTITY_ANIMATION climber_land = {1, 1, {2, 8, 5, 0}};
//===============================================================================
//ENEMY_CRAWLER
//===============================================================================
const ENTITY_ANIMATION crawler_walk = {5, 0, {0, 5, 1, 5, 2, 5, 3, 5, 4, 5}};
//===============================================================================
//ENEMY_SKREE
//===============================================================================
const ENTITY_ANIMATION skree_hang = {4, 0, {0, 9, 1, 9, 2, 9, 3, 9}};
const ENTITY_ANIMATION skree_dive_start = {1, 0, {4, 1}};
//===============================================================================
//ENEMY_HOPPER
//===============================================================================
const ENTITY_ANIMATION hopper_stand = {1, 0, {0, 1}};
const ENTITY_ANIMATION hopper_hop = {2, 1, {1, 4, 2, 1}};
const ENTITY_ANIMATION hopper_land = {3, 0, {1, 4, 0, 4, 1, 4}};
//===============================================================================
//ENEMY_CHOZO
//===============================================================================
const ENTITY_ANIMATION chozo_sit = {1, 0, {1, 1}};
const ENTITY_ANIMATION chozo_standup = {5, 0, {2, 8, 3, 8, 4, 8, 5, 8, 9, 8}};
const ENTITY_ANIMATION chozo_step_forward = {1, 0, {9, 8}};
const ENTITY_ANIMATION chozo_step_right = {4, 0, {1, 8, 2, 8, 3, 8, 4, 8}};
const ENTITY_ANIMATION chozo_wait_right = {1, 0, {5, 1}};
const ENTITY_ANIMATION chozo_step_left = {4, 0, {6, 8, 7, 8, 8, 8, 9, 8}};
const ENTITY_ANIMATION chozo_wait_left = {1, 0, {0, 1}};
const ENTITY_ANIMATION chozo_reverse = {1, 0, {0, 1}};
const ENTITY_ANIMATION chozo_jump = {4, 3, {10, 8, 11, 8, 12, 25, 13, 1}};
const ENTITY_ANIMATION chozo_swipe = {7, 0, {20, 5, 21, 5, 22, 5, 23, 5, 22, 5, 21, 5, 20, 5}};
const ENTITY_ANIMATION chozo_breath = {11, 0, {14, 5, 15, 5, 16, 5, 17, 5, 18, 5, 19, 50, 18, 5, 17, 5, 16, 5, 15, 5, 14, 15}};
const ENTITY_ANIMATION chozo_shoot = {7, 0, {20, 10, 21, 5, 22, 5, 23, 5, 22, 5, 21, 5, 20, 10}};
//===============================================================================
//ENEMY_SPIKEY
//===============================================================================
const ENTITY_ANIMATION spikey_walk = {4, 0, {0, 8, 1, 8, 2, 8, 3, 8}};
const ENTITY_ANIMATION spikey_rotate = {1, 0, {4, 1}};
//===============================================================================
//ENEMY_SPIKEY
//===============================================================================
const ENTITY_ANIMATION ripper_fly = {1, 0, {0, 1}};
const ENTITY_ANIMATION ripper_turn = {1, 0, {1 ,1}};
//===============================================================================
//ENEMY_ACID
//===============================================================================
const ENTITY_ANIMATION acid_stand = {1, 0, {0, 1}};
const ENTITY_ANIMATION acid_hop_start = {3, 0, {1, 6, 2, 6, 1, 6}};
const ENTITY_ANIMATION acid_hop = {2, 1, {0, 6, 3, 1}};
const ENTITY_ANIMATION acid_land = {4, 0, {0, 6, 1, 6, 2, 6, 1, 6}};
const ENTITY_ANIMATION acid_shoot = {6, 0, {0, 15, 4, 6, 5, 6, 6, 6, 7, 15, 0, 30}};
const ENTITY_ANIMATION acid_fly = {1, 0, {8, 1}};
const ENTITY_ANIMATION acid_swoop = {4, 3, {9, 4, 10, 4, 11, 4, 12, 1}};
//===============================================================================
//ENEMY_PLANT
//===============================================================================
const ENTITY_ANIMATION plant_walk = {8, 0, {0, 6, 1, 6, 2, 6, 3, 6, 4, 6, 5, 6, 6, 6, 7, 6}};
const ENTITY_ANIMATION plant_shoot = {4, 0, {8, 8, 9, 36, 8, 4, 0, 12}};
//===============================================================================
//ENEMY_ROBOT
//===============================================================================
const ENTITY_ANIMATION robot_walk = {12, 0,
	{0, 8, 1, 8, 2, 8, 3, 8, 4, 8, 5, 8, 6, 8, 7, 8, 8, 8, 9, 8, 10, 8, 11, 8}};
const ENTITY_ANIMATION robot_walk_fast = {12, 0,
	{0, 4, 1, 4, 2, 4, 3, 4, 4, 4, 5, 4, 6, 4, 7, 4, 8, 4, 9, 4, 10, 4, 11, 4}};
//===============================================================================
//ENEMY_RIDLEY
//===============================================================================
const ENTITY_ANIMATION ridley_fade_in = {4, 0, {0, 128, 1, 16, 2, 16, 3, 16}};
const ENTITY_ANIMATION ridley_take_off = {4, 0, {6, 16, 4, 4, 5, 16, 4, 4}};
const ENTITY_ANIMATION ridley_fly = {1, 0, {6, 1}};
const ENTITY_ANIMATION ridley_breath = {4, 0, {6, 4, 7, 4, 8, 32, 7, 4}};
const ENTITY_ANIMATION ridley_reverse = {1, 0, {0, 1}};
//===============================================================================
//ENEMY_FLOATER
//===============================================================================
const ENTITY_ANIMATION floater_wait = {1, 0, {0, 1}};
const ENTITY_ANIMATION floater_jump = {2, 1, {1, 8, 2, 1}};
const ENTITY_ANIMATION floater_fall = {2, 1, {1, 8, 3, 1}};
//===============================================================================
//ENEMY_KRAID
//===============================================================================
//	BODY (just use particle)
//	HEAD
const ENTITY_ANIMATION kraid_head_roar = {5, 0, {1, 6, 2, 6, 3, 64, 2, 6, 1, 6}};
const ENTITY_ANIMATION kraid_head_burp = {1, 0, {1, 1}};
//	ARM
const ENTITY_ANIMATION kraid_arm_idle = {18, 0,
	{0, 25, 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 5, 7, 5, 8, 5, 9, 10,
	8, 5, 7, 5, 6, 5, 5, 5, 4, 5, 3, 5, 2, 5, 1, 5}};
//	FOOT
const ENTITY_ANIMATION kraid_foot_idle = {1, 0, {3, 0}};
const ENTITY_ANIMATION kraid_foot_step_back = {5, 0, {2, 4, 1, 4, 0, 24, 1, 8, 2, 8}};
const ENTITY_ANIMATION kraid_foot_step_forward = {5, 0, {2, 8, 1, 8, 0, 24, 1, 4, 2, 4}};
const ENTITY_ANIMATION kraid_claw = {8, 0, {0, 3, 1, 3, 2, 3, 3, 3, 4, 3, 5, 3, 6, 3, 7, 3}};
//===============================================================================
//ENEMY_SWOOPER
//===============================================================================
const ENTITY_ANIMATION swooper_down = {3, 2, {1, 10, 2, 10, 3, 1}};
const ENTITY_ANIMATION swooper_up = {1, 0, {3, 1}};
const ENTITY_ANIMATION swooper_pause = {2, 0, {2, 10, 0, 10}}; 
//===============================================================================
//ENEMY_SUCKER
//===============================================================================
const ENTITY_ANIMATION sucker_walk = {4, 0, {0, 8, 1, 8, 2, 8, 1, 8}};
const ENTITY_ANIMATION sucker_jump_up = {2, 1, {3, 8, 4, 1}};
const ENTITY_ANIMATION sucker_jump_down = {2, 1, {3, 8, 0, 1}};
const ENTITY_ANIMATION sucker_suck = {4, 0, {5, 4, 6, 4, 7, 4, 6, 4}};
//===============================================================================
//ENEMY_LAVAHORSE
//===============================================================================
const ENTITY_ANIMATION lavahorse_wait = {1, 0, {1, 1}};
//const ENTITY_ANIMATION lavahorse_jump - use particle
const ENTITY_ANIMATION lavahorse_walk = {4, 0, {1, 8, 2, 8, 3, 8, 4, 8}};
const ENTITY_ANIMATION lavahorse_shoot = {5, 0, {8, 6, 5, 6, 6, 6, 7, 6, 8, 12}};
//===============================================================================
//ENEMY_PIPEGUY
//===============================================================================
const ENTITY_ANIMATION pipeguy0 = {2, 0, {0, 5, 1, 5}};
const ENTITY_ANIMATION pipeguy1 = {2, 0, {2, 5, 3, 5}};
const ENTITY_ANIMATION pipeguy2 = {8, 0, {0, 3, 1, 3, 2, 3, 1, 3, 3, 3, 4, 3, 5, 3, 4, 3}};
//===============================================================================
//ENEMY_PARTICLE
//===============================================================================
const ENTITY_ANIMATION particle = {1, 0, {0, 1}};

const _ENEMY_ACTION enemy_action[ACT_END_OF_ENEMY] = {
	{&pirate_walk, -1, 30, TRUE, ACT_PIRATE_LOOK},
	{&pirate_look, 110, 0, TRUE, ACT_PIRATE_WALK},
	{&pirate_shoot, 112, 0, TRUE, ACT_PIRATE_WALK},
	{&pirate_hit, 20, 0, TRUE, ACT_PIRATE_SHOOT},

	{&climber_climb, -1, 30, TRUE, ACT_CLIMBER_CLIMB},
	{&climber_shoot, 72, 0, TRUE, ACT_CLIMBER_SHOOTING_DONE},
	{&climber_shooting_done, 36, 0, TRUE, ACT_CLIMBER_CLIMB},
	{&climber_jump, -1, 0, TRUE, ACT_CLIMBER_LAND},
	{&climber_land, 32, 0, TRUE, ACT_CLIMBER_CLIMB},

	{&crawler_walk, -1, 40, FALSE, ACT_CRAWLER_WALK},
	{&crawler_walk, -1, 0, FALSE, ACT_CRAWLER_WALK},

	{&skree_hang, -1, 0, FALSE, ACT_SKREE_DIVE_START},
	{&skree_dive_start, 10, 0, FALSE, ACT_SKREE_DIVE},
	{&skree_hang, -1, 0, FALSE, NONE},

	{&hopper_stand, 10, 0, FALSE, ACT_HOPPER_HOP},
	{&hopper_hop, -1, 0, FALSE, ACT_HOPPER_LAND},
	{&hopper_land, 12, 0, FALSE, ACT_HOPPER_STAND},

	{&chozo_step_right, 32, 80, TRUE, ACT_CHOZO_WAIT_RIGHT},
	{&chozo_wait_right, 7, 0, TRUE, ACT_CHOZO_STEP_LEFT},
	{&chozo_step_left, 32, 80, TRUE, ACT_CHOZO_WAIT_LEFT},
	{&chozo_wait_left, 7, 0, TRUE, ACT_CHOZO_STEP_RIGHT},
	{&chozo_reverse, 7, 0, FALSE, ACT_CHOZO_STEP_RIGHT},
	{&chozo_jump, -1, 0, TRUE, ACT_CHOZO_BREATH},
	{&chozo_swipe, 35, 0, TRUE, ACT_CHOZO_WAIT_RIGHT},
	{&chozo_breath, 110, 0, TRUE, ACT_CHOZO_WAIT_RIGHT},
	{&chozo_sit, -1, 0, FALSE, ACT_CHOZO_STANDUP},
	{&chozo_standup, 40, 0, FALSE, ACT_CHOZO_STEP_FORWARD},
	{&chozo_step_forward, 8, 80, FALSE, ACT_CHOZO_WAIT_RIGHT},
	{&chozo_shoot, 45, 0, TRUE, ACT_CHOZO_WAIT_RIGHT},

	{&spikey_walk, -1, 30, FALSE, ACT_SPIKEY_WALK},
	{&spikey_walk, -1, 0, FALSE, ACT_SPIKEY_WALK},
	{&spikey_rotate, 50, 0, FALSE, ACT_SPIKEY_WALK},

	{&ripper_fly, -1, 60, TRUE, ACT_RIPPER_TURN},
	{&ripper_turn, 3, 0, TRUE, ACT_RIPPER_FLY},

	{&acid_stand, 30, 0, TRUE, ACT_ACID_HOP_START},
	{&acid_hop_start, 18, 0, TRUE, ACT_ACID_HOP},
	{&acid_hop, -1, 0, TRUE, ACT_ACID_LAND},
	{&acid_land, 24, 0, TRUE, ACT_ACID_STAND},
	{&acid_shoot, 78, 0, TRUE, ACT_ACID_STAND},
	{&acid_fly, -1, 60, TRUE, ACT_ACID_FLY},
	{&acid_swoop, -1, 0, TRUE, ACT_ACID_FLY},

	{&plant_walk, -1, 15, FALSE, ACT_PLANT_WALK},
	{&plant_shoot, 60, 0, FALSE, ACT_PLANT_WALK},

	{&robot_walk, -1, 15, TRUE, ACT_ROBOT_WALK},
	{&particle, 100, 0, TRUE, ACT_ROBOT_WALK},
	{&robot_walk_fast, 100, 0, TRUE, ACT_ROBOT_STAND},

	{&ridley_fade_in, 174, 0, TRUE, ACT_RIDLEY_BREATH_INTRO},
	{&ridley_breath, 44, 0, TRUE, ACT_RIDLEY_TAKE_OFF},
	{&ridley_take_off, 40, 0, TRUE, ACT_RIDLEY_FLY},
	{&ridley_fly, -1, 0, TRUE, NONE},
	{&ridley_breath, 44, 0, TRUE, ACT_RIDLEY_FLY},
	{&ridley_reverse, 12, 0, FALSE, ACT_RIDLEY_FLY},

	{&floater_wait, -1, 0, FALSE, ACT_FLOATER_JUMP},
	{&floater_jump, -1, 0, FALSE, ACT_FLOATER_FALL},
	{&floater_fall, -1, 0, FALSE, ACT_FLOATER_WAIT},
	
	{&kraid_head_burp, 12, 0, FALSE, ACT_PARTICLE},
	{&kraid_head_roar, 88, 0, FALSE, ACT_PARTICLE},
	{&kraid_arm_idle, -1, 0, FALSE, NONE},
	{&kraid_foot_idle, -1, 0, FALSE, NONE},
	{&kraid_foot_idle, 48, 0, FALSE, ACT_KRAID_FOOT_IDLE},
	{&kraid_foot_step_back, 48, 0, FALSE, ACT_KRAID_FOOT_PAUSE},
	{&kraid_foot_step_forward, 48, 0, FALSE, ACT_KRAID_FOOT_PAUSE},
	{&kraid_claw, -1, 0, FALSE, NONE},
	
	{&swooper_down, -1, 0, FALSE, ACT_SWOOPER_UP},
	{&swooper_up, -1, 0, FALSE, ACT_SWOOPER_PAUSE},
	{&swooper_pause, 20, 0, FALSE, ACT_SWOOPER_DOWN},
	
	{&sucker_walk, -1, 30, TRUE, ACT_SUCKER_JUMP_UP},
	{&sucker_jump_up, -1, 0, TRUE, ACT_SUCKER_JUMP_DOWN},
	{&sucker_jump_down, -1, 0, TRUE, ACT_SUCKER_WALK},
	{&sucker_suck, -1, 0, TRUE, NONE},
	
	{&lavahorse_wait, -1, 0, TRUE, ACT_LAVAHORSE_JUMP},
	{&particle, -1, 0, TRUE, ACT_LAVAHORSE_WALK},
	{&lavahorse_walk, -1, 40, TRUE, ACT_LAVAHORSE_SHOOT},
	{&lavahorse_shoot, 36, 0, TRUE, ACT_LAVAHORSE_WALK},
	
	{&pipeguy0, -1, 0, TRUE, ACT_PIPEGUY0_ATTACK},
	{&pipeguy0, -1, 0, TRUE, ACT_PIPEGUY1_ATTACK},
	{&pipeguy2, -1, 0, TRUE, ACT_PIPEGUY2_ATTACK},
	{&pipeguy0, -1, 0, TRUE, ACT_PIPEGUY0_READY},
	{&pipeguy1, -1, 0, TRUE, ACT_PIPEGUY1_READY},
	{&pipeguy2, -1, 0, TRUE, ACT_PIPEGUY2_READY},
	
	{&particle, -1, 0, FALSE, NONE},
};

/*
typedef struct {
	ENTITY_SIZE size;
	char powerup_drop_rate;
	char powerup_priority[4];
	char (*ai)(ENEMY *);
	void (*hit_ai)(ENEMY *);
	void (*killed_ai)(ENEMY *);
	char extra_frame_skip;
	char wmask;
	char simple_movement;
	char disable;
	char explosive;
	char reverse_offset;
	char gravity_right_offset;
	char gravity_left_offset;
	unsigned short immune;
	unsigned short hp;
	short damage;
	char death_anim;
	char init_action;
	char extra_enemies;
	char extra_enemy_type;
} ENEMY_DATA;
*/

#define HEAVY_ARMOR DAMAGE_NORMAL | DAMAGE_CHARGE | DAMAGE_MISSILE | DAMAGE_BOMB | DAMAGE_CHARGE_SCREW | DAMAGE_PLASMA
#define RIDLEY_ARMOR DAMAGE_NORMAL | DAMAGE_BOMB | DAMAGE_SCREW | DAMAGE_CHARGE_SCREW | DAMAGE_SPEED | DAMAGE_PLASMA
#define SUCKER_ARMOR DAMAGE_NORMAL | DAMAGE_CHARGE | DAMAGE_PLASMA | DAMAGE_CHARGE_SCREW | DAMAGE_MISSILE
#define IMMUNE 0xffff

const ENEMY_DATA enemy_data[ENEMY_END_OF_LIST] = {
	{{-2, 0, 140, 61, 140, 61, FALSE},
	0, {NONE, NONE, NONE, NONE}, /*ship*/
		0, NO_EXTRA_FRAMES, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, NONE, 0, NONE, ACT_SHIP, 0, NONE},

	{{12, 18, 16, 36, 40, 56, TRUE}, /*pirate walker*/
	90, {POWERUP_MISSILE, POWERUP_HEALTH, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		1, 0, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0, 4, 15, MULTI_EXPLODE, ACT_PIRATE_WALK, 0, NONE},

	{{0, 12, 24, 40, 32, 56, FALSE}, /*pirate_climber*/
	90, {POWERUP_MISSILE, POWERUP_HEALTH, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		2, 0, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 7, 0, 0, 4, 15, MULTI_EXPLODE, ACT_CLIMBER_CLIMB, 0, NONE},

	{{3, 5, 12, 12, 18, 18, TRUE}, /*crawler*/
	70, {POWERUP_HEALTH, POWERUP_MISSILE, NONE, NONE},
		3, NO_EXTRA_FRAMES, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 10, 5, 0, 1, 10, LARGE_EXPLODE, ACT_CRAWLER_WALK, 0, NONE},

	{{4, 4, 16, 20, 24, 24, FALSE}, /*skree*/
	70, {POWERUP_HEALTH, POWERUP_MISSILE, NONE, NONE},
		4, NO_EXTRA_FRAMES, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, 1, 15, LARGE_EXPLODE, ACT_SKREE_HANG, 4, ENEMY_SKREEBLOB},

	{{4, 11, 22, 19, 30, 30, FALSE}, /*hopper*/
	70, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		5, NO_EXTRA_FRAMES, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, 3, 20, HUGE_EXPLODE, ACT_HOPPER_STAND, 0, NONE},

	{{12, 25, 50, 68, 72, 96, TRUE}, /*chozo statue*/
	0, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		6, 6, TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, DAMAGE_NORMAL | DAMAGE_SCREW | DAMAGE_SPEED, 150, 20, MULTI_EXPLODE, ACT_CHOZO_SIT, 5, ENEMY_NUKEBALL},

	{{5, 4, 11, 11, 24, 24, TRUE}, /*spikey*/
	70, {POWERUP_HEALTH, POWERUP_MISSILE, NONE, NONE},
		7, 4, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 5, 10, DAMAGE_NORMAL, 1, 25, HUGE_EXPLODE, ACT_SPIKEY_WALK, 0, NONE},

	{{0, 1, 23, 12, 24, 24, TRUE}, /*ripper*/
	90, {POWERUP_SUPERMISSILE, POWERUP_POWERBOMB, POWERUP_HEALTH, NONE},
		8, 0, FALSE, FALSE, TRUE, FALSE, TRUE, FALSE, REVERSE_NORMAL, 0, 0, HEAVY_ARMOR, 1, 15, LARGE_EXPLODE, ACT_RIPPER_FLY, 0, NONE},

	{{26, 20, 14, 26, 66, 46, TRUE}, /*acid spitter*/
	90, {POWERUP_MISSILE, POWERUP_HEALTH, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		9, 0, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0, 10, 25, MULTI_EXPLODE, ACT_ACID_FLY, 0, NONE},

	{{7, 16, 12, 16, 24, 36, FALSE}, /*spike plant*/
	90, {POWERUP_SUPERMISSILE, POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_POWERBOMB},
		10, NO_EXTRA_FRAMES, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, 8, 20, HUGE_EXPLODE, ACT_PLANT_WALK, 0, NONE},

	{{9, 0, 18, 48, 36, 48, TRUE}, /*robot*/
	90, {POWERUP_MISSILE, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB, POWERUP_HEALTH},
		11, 0, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, DAMAGE_NORMAL, 15, 25, MULTI_EXPLODE, ACT_ROBOT_WALK, 0, NONE},

	{{22, 19, 28, 65, 72, 84, TRUE}, /*ridley*/
	0, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		12, 1, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, RIDLEY_ARMOR, 800, 100, MULTI_EXPLODE, ACT_RIDLEY_FADE_IN, 0, NONE},

	{{2, 4, 20, 7, 24, 12, TRUE}, /*floater*/
	0, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		13, NO_EXTRA_FRAMES, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, 8, 25, LARGE_EXPLODE, ACT_FLOATER_WAIT, 0, NONE},

	{{0, 0, 78, 66, 78, 66, TRUE}, /*kraid_head*/
	0, {NONE, NONE, NONE, NONE},
		14, NO_EXTRA_FRAMES, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, REVERSE_NORMAL, 0, 0, 0xffff, 300, 60, MULTI_EXPLODE, ACT_PARTICLE, 10, ENEMY_KRAID_ARM},

	{{1, 7, 21, 18, 23, 26, FALSE}, /*swooper*/
	70, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		15, NO_EXTRA_FRAMES, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, 3, 20, HUGE_EXPLODE, ACT_SWOOPER_PAUSE, 0, NONE},

	{{0, 0, 12, 12, 12, 12, FALSE}, /*pipeguy_spawn*/
	0, {0, 0, 0, 0},
		0xff, NO_EXTRA_FRAMES, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, NONE, 0, NONE, 0, 1, ENEMY_PIPEGUY0},
	
	{{10, 2, 12, 12, 32, 26, FALSE}, /*sucker*/
	90, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		16, 0, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, SUCKER_ARMOR, 3, 0, LARGE_EXPLODE, ACT_SUCKER_WALK, 0, NONE},

	{{8, 8, 14, 32, 30, 40, FALSE}, /*lavahorse_walker*/
	90, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		17, 0, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0, 12, 40, MULTI_EXPLODE, ACT_LAVAHORSE_WAIT, 0, NONE},
		
	
	{{0, 0, 12, 10, 12, 10, FALSE}, /*pipeguy0*/
	95, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		18, 0, FALSE, TRUE, DISABLE_KILL, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0, 1, 15, LARGE_EXPLODE, ACT_PIPEGUY0_READY, 0, ENEMY_PIPEGUY1},
	
	{{0, 0, 12, 12, 12, 12, FALSE}, /*pipeguy1*/
	95, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		19, 0, FALSE, TRUE, DISABLE_KILL, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0, 3, 30, LARGE_EXPLODE, ACT_PIPEGUY1_READY, 0, ENEMY_PIPEGUY2},

	{{0, 0, 12, 12, 12, 12, FALSE}, /*pipeguy2*/
	95, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		20, 0, FALSE, TRUE, DISABLE_KILL, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0, 8, 60, LARGE_EXPLODE, ACT_PIPEGUY2_READY, 0, NONE},	
		
	{{0, 0, 48, 54, 48, 54, TRUE}, /*kraid_arm*/
	0, {NONE, NONE, NONE, NONE},
		21, NO_EXTRA_FRAMES, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0xffff, 5000, 40, MULTI_EXPLODE, ACT_KRAID_ARM_IDLE, 0, ENEMY_KRAID_FOOT},

	{{0, 0, 108, 54, 108, 54, TRUE}, /*kraid_foot*/
	0, {NONE, NONE, NONE, NONE},
		22, NO_EXTRA_FRAMES, TRUE, TRUE, FALSE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0xffff, 5000, 40, MULTI_EXPLODE, ACT_KRAID_FOOT_IDLE, 0, ENEMY_KRAID_BODY},

	{{0, 0, 112, 162, 112, 162, TRUE}, /*kraid_body*/
	0, {POWERUP_HEALTH, POWERUP_MISSILE, POWERUP_SUPERMISSILE, NONE},
		23, NO_EXTRA_FRAMES, TRUE, TRUE, FALSE, FALSE, FALSE, TRUE, REVERSE_NORMAL, 0, 0, 0xffff, 5000, 60, MULTI_EXPLODE, ACT_PARTICLE, 0, ENEMY_KRAID_SPIKE},

	{{0, 0, 36, 18, 36, 18, TRUE}, /*kraid_spike*/
	0, {NONE, NONE, NONE, NONE},
		24, NO_EXTRA_FRAMES, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, REVERSE_NORMAL, 0, 0, 0xffff, 5000, 50, LARGE_EXPLODE, ACT_PARTICLE, 0, ENEMY_KRAID_CLAW},
	
	{{0, 0, 12, 12, 12, 12, TRUE}, /*kraid_claw*/
	100, {POWERUP_MISSILE, POWERUP_HEALTH, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		25, NO_EXTRA_FRAMES, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, 1, 40, SMALL_EXPLODE, ACT_KRAID_CLAW, 0, NONE},
	
	
	{{0, 0, 10, 10, 12, 12, FALSE}, /*nukeball*/
	100, {POWERUP_MISSILE, POWERUP_HEALTH, POWERUP_SUPERMISSILE, POWERUP_POWERBOMB},
		26, NO_EXTRA_FRAMES, FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, 1, 15, SMALL_EXPLODE, ACT_NUKEBALL, 0, NONE},

	{{0, 0, 6, 6, 6, 6, FALSE}, /*skree blob*/
	0, {NONE, NONE, NONE, NONE},
		27, NO_EXTRA_FRAMES, FALSE, TRUE, DISABLE_KILL, FALSE, FALSE, FALSE, NO_REVERSE, 0, 0, 0, NONE, 15, NONE, ACT_SKREE_BLOB, 0, NONE},
};