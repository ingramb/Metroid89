// C Source File
// Created 1/21/02; 1:11:37 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "extgraph.h"
#include "dllutility.h"
#include "player.h"
#include "dllanimation.h"
#include "enemy.h"
#include "dllclipsprites.h"
#include "globals.h"

//ANIMATION *animations = NULL;

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

char animation_setup()
{
	//animations = malloc(sizeof(ANIMATION) * ANIM_MAX);
	//if(animations == NULL) return FALSE;

	animation_reset();
	return TRUE;
}

void animation_reset()
{
	short i;

	memset(glbs->animations, 0, sizeof(ANIMATION) * ANIM_MAX);

	for(i = 0 ; i < ANIM_MAX - 1 ; i++) glbs->animations[i].next = i + 1;
	glbs->animations[ANIM_MAX - 1].next = -1;

	glbs->first_animation = -1;
	glbs->first_empty_animation = 0;
}

void animation_cleanup()
{
	//if(animations != NULL) {
	//	free(animations);
	//	animations = NULL;
	//}
}

short animation_create(short x, short y, short type, short link)
{
	short i = glbs->first_empty_animation;

	if(i < 0 || type == NONE) return NONE;

	glbs->first_empty_animation  = glbs->animations[i].next;

	glbs->animations[i].next = glbs->first_animation;
	glbs->first_animation = i;

	glbs->animations[i].type = type;
	glbs->animations[i].x = x + anim_data[type].x_off - anim_data[type].width / 2;
	glbs->animations[i].y = y + anim_data[type].y_off - anim_data[type].hieght / 2;
	if(anim_data[type].reverse) {
		glbs->animations[i].frame = anim_data[type].frame_number - 1;
		glbs->animations[i].direction = -1;
	} else {
		glbs->animations[i].frame = 0;
		glbs->animations[i].direction = 1;
	}
	glbs->animations[i].frame_counter = 0;
	glbs->animations[i].x_speed_counter = 0;
	glbs->animations[i].y_speed_counter = 0;
	glbs->animations[i].link = link;

	if(anim_data[type].spawn_frame == 0)
		animation_create(
			glbs->animations[i].x + anim_data[type].width / 2,
			glbs->animations[i].y + anim_data[type].width / 2, type + 1, NONE);

	return i;
}

void animation_delete(short a)
{
	register short i;

	if(a == glbs->first_animation)
		glbs->first_animation = glbs->animations[a].next;
	else
		for(i = glbs->first_animation ; i >= 0 ; i = glbs->animations[i].next)
			if(glbs->animations[i].next == a) {
				glbs->animations[i].next = glbs->animations[a].next;
				break;
			}

	glbs->animations[a].next = glbs->first_empty_animation;
	glbs->first_empty_animation = a;
}

void animation_process()
{
	short i;
	short next;
	short type;

	for(i = glbs->first_animation ; i >= 0 ; i = next) {
		next = glbs->animations[i].next;
		type = glbs->animations[i].type;

		glbs->animations[i].x_speed_counter += anim_data[type].x_speed;
		glbs->animations[i].y_speed_counter += anim_data[type].y_speed;

		if(glbs->animations[i].x_speed_counter < 0) {
			while(glbs->animations[i].x_speed_counter <= -100) {
				glbs->animations[i].x_speed_counter += 100;
				glbs->animations[i].x--;
			}
		} else if(glbs->animations[i].x_speed_counter > 0) {
			while(glbs->animations[i].x_speed_counter >= 100) {
				glbs->animations[i].x_speed_counter -= 100;
				glbs->animations[i].x++;
			}
		}

		if(glbs->animations[i].y_speed_counter < 0) {
			while(glbs->animations[i].y_speed_counter <= -100) {
				glbs->animations[i].y_speed_counter += 100;
				glbs->animations[i].y--;
			}
		} else if(glbs->animations[i].y_speed_counter > 0) {
			while(glbs->animations[i].y_speed_counter >= 100) {
				glbs->animations[i].y_speed_counter -= 100;
				glbs->animations[i].y++;
			}
		}

		glbs->animations[i].frame_counter++;
		if(glbs->animations[i].frame_counter == anim_data[type].speed) {
			glbs->animations[i].frame_counter = 0;
			glbs->animations[i].frame += glbs->animations[i].direction;

			if(glbs->animations[i].frame == anim_data[type].frame_number || glbs->animations[i].frame < 0) {
				if(anim_data[type].loop) {
					glbs->animations[i].direction = -glbs->animations[i].direction;
					glbs->animations[i].frame += glbs->animations[i].direction;
				} else
					animation_delete(i);
					continue;
			}

			if(glbs->animations[i].frame == anim_data[type].spawn_frame)
					animation_create(
						glbs->animations[i].x + anim_data[type].width / 2,
						glbs->animations[i].y + anim_data[type].width / 2, type + 1, NONE);
		}
	}
}

void animation_draw(SPRITE_HEADER *anim_header, unsigned char *anim_gfx, ENEMY *e)
{
	SPRITE_HEADER *header;
	register short i;
	short link;
	short type;
	short x, y;

	for(i = glbs->first_animation ; i >= 0 ; i = glbs->animations[i].next) {
		type = glbs->animations[i].type;
		header = &anim_header[glbs->animations[i].frame + anim_data[type].frame_start];

		if(glbs->animations[i].link == LINK_PLAYER) {
			x = glbs->player.e.x;
			y = glbs->player.e.y;
		} else if(glbs->animations[i].link != NONE) {
			link = glbs->animations[i].link;
			x = e[link].e.x;
			y = e[link].e.y;
		} else {
			x = glbs->animations[i].x;
			y = glbs->animations[i].y;
		}
		if(glbs->animations[i].link != NONE) {
			x += anim_data[type].x_off;
			y += anim_data[type].y_off;
		}

		GraySpriteClipX8_MASK(
			x + header->x_offset - glbs->camera.x,
			y + header->y_offset - glbs->camera.y,
			header->hieght, (anim_gfx + header->offset), header->width, FALSE);
	}
}
