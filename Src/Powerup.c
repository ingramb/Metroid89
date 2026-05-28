// C Source File
// Created 11/3/2002; 1:19:54 PM

#include <tigcclib.h>         // Include All Header Files
#include "utility.h"
//#include "extgraph.h"
#include "entity.h"
#include "player.h"
#include "map.h"
#include "powerup.h"
#include "bar.h"
#include "clipsprites.h"
#include "miscgfx.h"
#include "globals.h"

const POWERUP_DATA powerup_data[] = {
	{8, 8}, {12, 12}, {5, 11}, {5, 11}, {7, 7}
};

POWERUP *powerups;

short first_powerup;
short first_empty_powerup;

char powerup_setup()
{
	powerups = malloc(sizeof(POWERUP) * POWERUP_MAX);
	if(powerups == NULL) return FALSE;

	powerup_reset();
	return TRUE;
}

void powerup_reset()
{
	short i;

	memset(powerups, 0, sizeof(POWERUP) * POWERUP_MAX);

	for(i = 0 ; i < POWERUP_MAX - 1 ; i++) powerups[i].next = i + 1;
	powerups[POWERUP_MAX - 1].next = -1;

	first_powerup = -1;
	first_empty_powerup = 0;
}

void powerup_cleanup()
{
	if(powerups != NULL) {
		free(powerups);
		powerups = NULL;
	}
}

short powerup_create(short x, short y, short type)
{
	short i = first_empty_powerup;

	if(i < 0) return NONE;

	first_empty_powerup  = powerups[i].next;

	powerups[i].next = first_powerup;
	first_powerup = i;

	powerups[i].x = x - (powerup_data[type].width >> 1);
	powerups[i].y = y - (powerup_data[type].hieght >> 1);
	powerups[i].type = type;
	powerups[i].frame = random(4);
	powerups[i].frame_counter = random(POWERUP_ANIM_SPEED) + 1;
	powerups[i].duration = POWERUP_DURATION;
	powerups[i].signal = -1;
	
	return i;
}

void powerup_delete(short a)
{
	short i;
	
	//let pipe spawner know that all traces of the spawned enemy are gone
	if(powerups[a].signal >= 0) enemies[powerups[a].signal].var[0]--;

	if(a == first_powerup) first_powerup = powerups[a].next;
	else {
		for(i = first_powerup ; i >= 0 ; i = powerups[i].next) {
			if(powerups[i].next == a) {
				powerups[i].next = powerups[a].next;
				break;
			}
		}
	}

	powerups[a].next = first_empty_powerup;
	first_empty_powerup = a;
}

void powerup_set_signal(short a, short signal) { powerups[a].signal = signal; }

void powerup_get(short a)
{
	short type = powerups[a].type;
	short i;

	if(type == 0) glbs->player.hp += 10;
	else if(type == 1) glbs->player.hp += 25;
	else if(type == 2) glbs->player.ammunition[0] += 2;
	else if(type == 3) glbs->player.ammunition[1] += 2;
	else if(type == 4) glbs->player.ammunition[2] += 2;

	for(i = 0 ; i < 3 ; i++)
		if(glbs->player.ammunition[i] > glbs->player.ammunition_max[i]) glbs->player.ammunition[i] = glbs->player.ammunition_max[i];
	if(glbs->player.hp > glbs->player.hp_max) glbs->player.hp = glbs->player.hp_max;

	powerup_delete(a);
	bar_update();
}

void powerup_process()
{
	short i;
	short next;
	char type;
	short px = glbs->player.e.x + glbs->player.e.size->x_off;
	short py = glbs->player.e.y + glbs->player.e.size->y_off;
	short pw = glbs->player.e.size->width;
	short ph = glbs->player.e.size->hieght;

	for(i = first_powerup ; i >= 0 ; i = next) {
		next = powerups[i].next;

		powerups[i].duration--;
		if(powerups[i].duration == 0) {
			powerup_delete(i);
			continue;
		}
		type = powerups[i].type;
		powerups[i].frame_counter--;
		if(powerups[i].frame_counter == 0) {
			powerups[i].frame++;
			if(powerups[i].frame == 4) powerups[i].frame = 0;
			if(type <= 1 && powerups[i].frame == 3) powerups[i].frame_counter = POWERUP_ANIM_SPEED * 2;
			else powerups[i].frame_counter = POWERUP_ANIM_SPEED;
		}

		if(rect_overlap(powerups[i].x, powerups[i].y, powerup_data[type].width, powerup_data[type].hieght,
			px, py, pw, ph)) powerup_get(i);
	}
}

void powerup_draw()
{
	SPRITE_HEADER *header;
	short i;

	for(i = first_powerup ; i >= 0 ; i = powerups[i].next) {
		header = glbs->powerup_header + (powerups[i].type << 2) + powerups[i].frame;
		GraySpriteClipX8_MASK(
			powerups[i].x + header->x_offset - glbs->camera.x,
			powerups[i].y + header->y_offset - glbs->camera.y,
			header->hieght, glbs->powerup_gfx + header->offset, header->width, FALSE);
	}
}



