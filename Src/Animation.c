// C Source File
// Created 1/21/02; 1:11:37 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "extgraph.h"
#include "utility.h"
#include "player.h"
#include "animation.h"
#include "enemy.h"
#include "map.h"
#include "miscgfx.h"
#include "clipsprites.h"

#define ANIM_MAX 15

ANIMATION *animations = NULL;

/*
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
*/

ANIMATION_DATA anim_data[] = {
	{0, 6, 4, 12, 12, 0, 0, FALSE, FALSE,		0, 0, NONE},
	
	{6, 6, 3, 24, 24, 0, 0, FALSE, FALSE,		0, 0, 4},
	
	{6, 6, 3, 24, 24, 0, 0, FALSE, FALSE,		-5, -5, 2},
	{0, 6, 4, 12, 12, 0, 0, FALSE, FALSE,		5, 0, 2},
	
	{6, 6, 3, 24, 24, 0, 0, FALSE, FALSE,		5, 10, 2},
	{0, 6, 4, 12, 12, 0, 0, FALSE, FALSE,		-5, 0, 2},
	
	{6, 6, 3, 24, 24, 0, 0, FALSE, FALSE,		5, -10, 2},
	{0, 6, 4, 12, 12, 0, 0, FALSE, FALSE,		-10, 5, 2},
	
	{6, 6, 3, 24, 24, 0, 0, FALSE, FALSE,		0, 5, 2},
	{0, 6, 4, 12, 12, 0, 0, FALSE, FALSE,		10, 5, NONE},
	
	{6, 6, 3, 24, 24, 0, 0, FALSE, FALSE,		0, 0, NONE},
	{12, 5, 3, 24, 24, 0, 0, FALSE, FALSE,	0, 0, NONE},
	{17, 3, 6, 8, 8, 0, 0, FALSE, FALSE,		0, 0, NONE},
	{20, 4, 3, 6, 6, 0, 0, FALSE, FALSE,		0, 0, NONE},
	{24, 6, 3, 36, 36, 0, 0, FALSE, FALSE,	0, 0, NONE},
	{30, 3, 6, 12, 12, 0, 0, FALSE, FALSE,	0, 0, NONE},
	
	{6, 6, 4, 24, 24, 0, 0, FALSE, FALSE,					0, 0, 0},
	{0, 6, 4, 12, 12, -50, -50, FALSE, FALSE,		-8, -8, 0},
	{0, 6, 4, 12, 12, 50, -50, FALSE, FALSE,		16, 0, 0},
	{0, 6, 4, 12, 12, 50, 50, FALSE, FALSE,			0, 16, 0},
	{0, 6, 4, 12, 12, -50, 50, FALSE, FALSE,		-16, 0, NONE},
	
	{33, 6, 5, 11, 23, 0, 0, FALSE, FALSE,			0, 0, NONE},
	
	{39, 7, 5, 15, 31, 0, 0, FALSE, FALSE,			0, 0, NONE},
	
	{46, 4, 5, 8, 8, 0, 50, FALSE, FALSE,			0, 0, NONE},
	
	{46, 4, 5, 8, 8, 0, 0, FALSE, FALSE,			0, 0, NONE},
	
	{50, 5, 3, 8, 8, 0, 0, FALSE, FALSE,				0, 0, NONE},
	
	{30, 3, 6, 12, 12, 0, 0, FALSE, TRUE,	0, 0, NONE},
	
	{55, 3, 2, 18, 21, 0, 0, TRUE, FALSE,	24, -2, NONE},
	{58, 3, 2, 18, 21, 0, 0, TRUE, FALSE,	-6, -2, NONE},
	
	{61, 5, 4, 17, 9, 0, 0, FALSE, FALSE, -8, 7, NONE},
	{66, 5, 4, 17, 9, 0, 0, FALSE, FALSE, 25, 7, NONE},
};

short first_animation;
short first_empty_animation;

char animation_setup()
{
	animations = malloc(sizeof(ANIMATION) * ANIM_MAX);
	if(animations == NULL) return FALSE;

	animation_reset();
	return TRUE;
}

void animation_reset()
{
	short i;
	
	memset(animations, 0, sizeof(ANIMATION) * ANIM_MAX);
	
	for(i = 0 ; i < ANIM_MAX - 1 ; i++) animations[i].next = i + 1;
	animations[ANIM_MAX - 1].next = -1;
	
	first_animation = -1;
	first_empty_animation = 0;
}

void animation_cleanup()
{
	if(animations != NULL) {
		free(animations);
		animations = NULL;
	}
}

short animation_create(short x, short y, short type, short link)
{
	short i = first_empty_animation;
	
	if(i < 0 || type == NONE) return NONE;
	
	first_empty_animation  = animations[i].next;
	
	animations[i].next = first_animation;
	first_animation = i;
	
	animations[i].type = type;
	animations[i].x = x + anim_data[type].x_off - anim_data[type].width / 2;
	animations[i].y = y + anim_data[type].y_off - anim_data[type].hieght / 2;
	if(anim_data[type].reverse) {
		animations[i].frame = anim_data[type].frame_number - 1;
		animations[i].direction = -1;
	} else {
		animations[i].frame = 0;
		animations[i].direction = 1;
	}
	animations[i].frame_counter = 0;
	animations[i].x_speed_counter = 0;
	animations[i].y_speed_counter = 0;
	animations[i].link = link;
	
	if(anim_data[type].spawn_frame == 0)
		animation_create(
			animations[i].x + anim_data[type].width / 2,
			animations[i].y + anim_data[type].width / 2, type + 1, NONE);
			
	return i;
}

void animation_delete(short a)
{
	register short i;
	
	if(a == first_animation)
		first_animation = animations[a].next;
	else
		for(i = first_animation ; i >= 0 ; i = animations[i].next)
			if(animations[i].next == a) {
				animations[i].next = animations[a].next;
				break;
			}
	
	animations[a].next = first_empty_animation;
	first_empty_animation = a;
}

void animation_process()
{
	short i;
	short next;
	short type;
	
	for(i = first_animation ; i >= 0 ; i = next) {
		next = animations[i].next;
		type = animations[i].type;
		
		animations[i].x_speed_counter += anim_data[type].x_speed;
		animations[i].y_speed_counter += anim_data[type].y_speed;
		
		if(animations[i].x_speed_counter < 0) {
			while(animations[i].x_speed_counter <= -100) {
				animations[i].x_speed_counter += 100;
				animations[i].x--;
			}
		} else if(animations[i].x_speed_counter > 0) {
			while(animations[i].x_speed_counter >= 100) {
				animations[i].x_speed_counter -= 100;
				animations[i].x++;
			}
		}
		
		if(animations[i].y_speed_counter < 0) {
			while(animations[i].y_speed_counter <= -100) {
				animations[i].y_speed_counter += 100;
				animations[i].y--;
			}
		} else if(animations[i].y_speed_counter > 0) {
			while(animations[i].y_speed_counter >= 100) {
				animations[i].y_speed_counter -= 100;
				animations[i].y++;
			}
		}
		
		animations[i].frame_counter++;
		if(animations[i].frame_counter == anim_data[type].speed) {
			animations[i].frame_counter = 0;
			animations[i].frame += animations[i].direction;
			
			if(animations[i].frame == anim_data[type].frame_number || animations[i].frame < 0) {
				if(anim_data[type].loop) {
					animations[i].direction = -animations[i].direction;
					animations[i].frame += animations[i].direction;
				} else
					animation_delete(i);
					continue;
			}
			
			if(animations[i].frame == anim_data[type].spawn_frame)
					animation_create(
						animations[i].x + anim_data[type].width / 2,
						animations[i].y + anim_data[type].width / 2, type + 1, NONE);
		}
	}
}

void animation_draw(PLAYER *p, ENEMY *e)
{
	SPRITE_HEADER *header;
	register short i;
	short link;
	short type;
	short x, y;
	
	for(i = first_animation ; i >= 0 ; i = animations[i].next) { 
		type = animations[i].type;
		header = &anim_header[animations[i].frame + anim_data[type].frame_start];
		
		if(animations[i].link == LINK_PLAYER) {
			x = p->e.x;
			y = p->e.y;
		} else if(animations[i].link != NONE) {
			link = animations[i].link;
			x = e[link].e.x;
			y = e[link].e.y;
		} else {
			x = animations[i].x;
			y = animations[i].y;
		}
		if(animations[i].link != NONE) {
			x += anim_data[type].x_off;
			y += anim_data[type].y_off;
		}

		GraySpriteClipX8_MASK(
			x + header->x_offset - camera.x,
			y + header->y_offset - camera.y,
			header->hieght, (anim_gfx + header->offset), header->width, FALSE);
	}		 	
}
