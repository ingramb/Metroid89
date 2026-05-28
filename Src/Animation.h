// C Header File
// Created 1/21/02; 1:10:15 PM

#ifndef __ANIMATION__
#define __ANIMATION__

#include "entity.h"
#include "map.h"
#include "enemy.h"
#include "dllexports.h"

#define SMALL_EXPLODE 0
#define MULTI_EXPLODE 1
#define LARGE_EXPLODE 10
#define BOMB_EXPLODE 11
#define TINY_EXPLODE 12
#define DUST_CLOUD 13
#define HUGE_EXPLODE 14
#define BLOCK_EXPLODE 15
#define SCREW_EXPLODE 16
#define NUKE_EXPLODE 21
#define SPLASH 22
#define ICE_DUST 23
#define SPARKLE 24
#define SHOCKWAVE_SMALL 25
#define BLOCK_REFORM 26
#define WINGS_LEFT 27
#define WINGS_RIGHT 28
#define SPURT_LEFT 29
#define SPURT_RIGHT 30

#define LINK_PLAYER -2

#define ANIM_MAX 15

typedef struct {
	short type;
	short link;
	char frame;
	char frame_counter;
	char direction;
	short x;
	short y;
	short x_speed_counter;
	short y_speed_counter;

	short next;
} ANIMATION;

typedef struct {
	char frame_start;
	char frame_number;
	char speed;
	char width;
	char hieght;
	char x_speed;
	char y_speed;
	unsigned loop : 1;
	unsigned reverse : 1;
	char x_off;
	char y_off;
	char spawn_frame;
} ANIMATION_DATA;

#ifndef __IN_DLL__
#define animation_setup _DLL_call(char, (), ANIMATION_SETUP)
#define animation_reset _DLL_call(void, (), ANIMATION_RESET)
#define animation_cleanup _DLL_call(char, (), ANIMATION_CLEANUP)
#define animation_create _DLL_call(short, (short, short, short, short), ANIMATION_CREATE)
#define animation_delete _DLL_call(void, (short), ANIMATION_DELETE)
#define animation_process _DLL_call(void, (), ANIMATION_PROCESS)
#define animation_draw _DLL_call(void, (SPRITE_HEADER *, unsigned char *, ENEMY *), ANIMATION_DRAW)
#endif

#endif

