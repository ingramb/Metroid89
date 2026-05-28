// C Header File
// Created 1/21/02; 11:52:01 PM

#ifndef __ENEMY__
#define __ENEMY__

#include "alloc.h"
#include "entity.h"

typedef struct {
	unsigned char type;
	unsigned char subtype;
	char gravity;
	char direction;
	short index;
	short x;
	short y;
} ENEMY_HEADER;

typedef struct {
	short vert_speed_min;
	short vert_speed_max;
	short vert_accel;
	char horiz_bounce;
	char vert_bounce;
} PROJECTILE_PATH;

enum RIDLEY_TAILS {
	RTAIL_IDLE,
	RTAIL_SWIPE,
	RTAIL_WHIRLWIND,
	RTAIL_BOUNCE,
};

typedef struct {
	unsigned char xpos[7];
	unsigned char ypos[7];
	char spike_frame;
} TAIL_FRAME;

typedef struct {
	TAIL_FRAME *frames;
	short frame_number;
	unsigned char speed;
	unsigned char next;
} TAIL_DATA;

typedef struct {
	ENTITY e;
	PROJECTILE_PATH *path;
	short path_horiz_speed;
	short path_vert_speed;
	char path_horiz_direction;
	char path_vert_direction;
	char direction;
	char hit_horiz_wall;
	char hit_vert_wall;
	char powerup;
	short hp;
	unsigned short immune;
	char flash;
	short frozen_counter;
	short frozen_block_handle;
	unsigned short immune_combo;
	HANDLE frozen_handle;
	unsigned char type;
	unsigned char subtype;
	short action;
	short action_duration;
	short frame;
	short frame_counter;
	ENTITY_ANIMATION *extra_anim;
	short extra_frame;
	short extra_frame_counter;
	short var[8];
	short index;
	short killed_index;

	char tail_type;
	unsigned char tail_frame;
	unsigned char tail_counter;

	short next;
} ENEMY;

typedef struct {
	const ENTITY_ANIMATION *animation;
	short duration;
	short move_speed;
	char reverse_flip;

	short next_action;
} _ENEMY_ACTION;

typedef struct {
	unsigned short animation;
	short duration;
	short move_speed;
	char reverse_flip;

	short next_action;
} ENEMY_ACTION;

typedef struct {
	ENTITY_SIZE size;
	char powerup_drop_rate;
	char powerup_priority[4];
	unsigned char gfx;
	char extra_frame_skip;
	char wmask;
	char simple_movement;
	char disable;
	char explosive;
	char disable_slope;
	char solid;
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

typedef struct {
	char (*ai)(ENEMY *);
	void (*init_ai)(ENEMY *e);
	void (*hit_ai)(ENEMY *, short damage);
	void (*killed_ai)(ENEMY *);
} ENEMY_AI;

typedef struct {
	HANDLE buffer;
	SPRITE_HEADER *header;
	unsigned char *gfx;
	unsigned short frame_number;
	char used;
} ENEMY_GFX;

enum enemies {
	ENEMY_SHIP,
	ENEMY_PIRATE_WALKER,
	ENEMY_PIRATE_CLIMBER,
	ENEMY_CRAWLER,
	ENEMY_SKREE,
	ENEMY_HOPPER,
	ENEMY_CHOZO,
	ENEMY_SPIKEY,
	ENEMY_RIPPER,
	ENEMY_ACID,
	ENEMY_PLANT,
	ENEMY_ROBOT,
	ENEMY_RIDLEY,
	ENEMY_FLOATER,
	ENEMY_KRAID_HEAD,
	ENEMY_SWOOPER,
	ENEMY_PIPEGUY_SPAWN,
	ENEMY_SUCKER,
	ENEMY_LAVAHORSE_WALKER,
	
	//these enemies are unplacable in map editor
	ENEMY_PIPEGUY0,
	ENEMY_PIPEGUY1,
	ENEMY_PIPEGUY2,
	ENEMY_KRAID_ARM,
	ENEMY_KRAID_FOOT,
	ENEMY_KRAID_BODY,
	ENEMY_KRAID_SPIKE,
	ENEMY_KRAID_CLAW,
	ENEMY_NUKEBALL,
	ENEMY_SKREEBLOB,
	ENEMY_END_OF_LIST,
};

enum enemy_actions {
	ACT_PIRATE_WALK,
	ACT_PIRATE_LOOK,
	ACT_PIRATE_SHOOT,
	ACT_PIRATE_HIT,

	ACT_CLIMBER_CLIMB,
	ACT_CLIMBER_SHOOT,
	ACT_CLIMBER_SHOOTING_DONE,
	ACT_CLIMBER_JUMP,
	ACT_CLIMBER_LAND,

	ACT_CRAWLER_WALK,
	ACT_CRAWLER_FALL,

	ACT_SKREE_HANG,
	ACT_SKREE_DIVE_START,
	ACT_SKREE_DIVE,

	ACT_HOPPER_STAND,
	ACT_HOPPER_HOP,
	ACT_HOPPER_LAND,

	ACT_CHOZO_STEP_RIGHT,
	ACT_CHOZO_WAIT_RIGHT,
	ACT_CHOZO_STEP_LEFT,
	ACT_CHOZO_WAIT_LEFT,
	ACT_CHOZO_REVERSE,
	ACT_CHOZO_JUMP,
	ACT_CHOZO_SWIPE,
	ACT_CHOZO_BREATH,
	ACT_CHOZO_SIT,
	ACT_CHOZO_STANDUP,
	ACT_CHOZO_STEP_FORWARD,
	ACT_CHOZO_SHOOT,

	ACT_SPIKEY_WALK,
	ACT_SPIKEY_FALL,
	ACT_SPIKEY_ROTATE,

	ACT_RIPPER_FLY,
	ACT_RIPPER_TURN,

	ACT_ACID_STAND,
	ACT_ACID_HOP_START,
	ACT_ACID_HOP,
	ACT_ACID_LAND,
	ACT_ACID_SHOOT,
	ACT_ACID_FLY,
	ACT_ACID_SWOOP,

	ACT_PLANT_WALK,
	ACT_PLANT_SHOOT,

	ACT_ROBOT_WALK,
	ACT_ROBOT_STAND,
	ACT_ROBOT_BACKUP,

	ACT_RIDLEY_FADE_IN,
	ACT_RIDLEY_BREATH_INTRO,
	ACT_RIDLEY_TAKE_OFF,
	ACT_RIDLEY_FLY,
	ACT_RIDLEY_BREATH,
	ACT_RIDLEY_REVERSE,
	/*ACT_RIDLEY_BOUNCE_DOWN,
	ACT_RIDLEY_BOUNCE_UP,
	ACT_RIDLEY_ALIGN,
	ACT_RIDLEY_CHARGE,
	ACT_RIDLEY_REVERSE,*/

	ACT_FLOATER_WAIT,
	ACT_FLOATER_JUMP,
	ACT_FLOATER_FALL,
	
	ACT_KRAID_HEAD_BURP,
	ACT_KRAID_HEAD_ROAR,
	ACT_KRAID_ARM_IDLE,
	ACT_KRAID_FOOT_IDLE,
	ACT_KRAID_FOOT_PAUSE,
	ACT_KRAID_FOOT_STEP_BACK,
	ACT_KRAID_FOOT_STEP_FORWARD,
	ACT_KRAID_CLAW,
	
	ACT_SWOOPER_DOWN,
	ACT_SWOOPER_UP,
	ACT_SWOOPER_PAUSE,
	
	ACT_SUCKER_WALK,
	ACT_SUCKER_JUMP_UP,
	ACT_SUCKER_JUMP_DOWN,
	ACT_SUCKER_SUCK,
	
	ACT_LAVAHORSE_WAIT,
	ACT_LAVAHORSE_JUMP,
	ACT_LAVAHORSE_WALK,
	ACT_LAVAHORSE_SHOOT,
	
	ACT_PIPEGUY0_READY,
	ACT_PIPEGUY1_READY,
	ACT_PIPEGUY2_READY,
	ACT_PIPEGUY0_ATTACK,
	ACT_PIPEGUY1_ATTACK,
	ACT_PIPEGUY2_ATTACK,

	ACT_PARTICLE,

	ACT_END_OF_ENEMY,
};

#define ACT_NUKEBALL ACT_PARTICLE
#define ACT_SKREE_BLOB ACT_PARTICLE
#define ACT_SHIP ACT_PARTICLE

//#define PARTICLE_START 18
//#define ENEMY_TYPE_NUMBER (PARTICLE_START + 2)

#define KILLED_ENEMIES_SIZE 10

#define REVERSE_NORMAL -1
#define NO_REVERSE 0
#define NO_EXTRA_FRAMES -1
#define DISABLE_KILL 2
extern char killed_enemies[];

extern ENEMY *enemies;
extern short enemy_number;
//extern ENEMY_DATA *enemy_data;
extern ENEMY_GFX enemy_gfx[];
extern short active_enemies[];
extern short active_enemy_number;
extern short enemy_kills;

extern TAIL_DATA ridley_tail_data[];

ENEMY *get_enemy(short i);
char enemy_setup();
void enemy_reset(ENEMY_HEADER *header, short number);
void enemy_cleanup();
void enemy_init(ENEMY_HEADER *header, short number);
void enemy_process();
void enemy_draw();
char enemy_hit(short i, short damage, char freeze, short damage_type, unsigned short combo_id);
void enemy_freeze(ENEMY *e, unsigned short combo_id);
void enemy_unfreeze(ENEMY *e);

#endif


