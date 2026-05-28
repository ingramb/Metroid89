// C Source File
// Created 4/5/02; 10:44:07 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
//#include "extgraph.h"
#include "utility.h"
#include "tiles.h"
#include "entity.h"
#include "miscgfx.h"
#include "enemy.h"
#include "door.h"
#include "map.h"
#include "shots.h"
#include "player.h"
#include "bar.h"
#include "clipsprites.h"
#include "globals.h"

#define DOOR_LEFT 0
#define DOOR_RIGHT 1
#define DOOR_UP 2
#define DOOR_DOWN 3
#define ELEVATOR_UP 4
#define ELEVATOR_DOWN 5

extern void screen_draw();

DOOR *doors = NULL;
short door_number;

char opened_doors[OPENED_DOORS_SIZE];

void fade_1(long *dst_light, long *dst_dark, long *light, long *dark);
void fade_2(long *dst_light, long *dst_dark, long *light, long *dark);
void fade_3(long *dst_light, long *dst_dark, long *light, long *dark);
void fade_4(long *dst_light, long *dst_dark, long *light, long *dark);
void fade_5(long *dst_light, long *dst_dark, long *light, long *dark);
void fade_6(long *dst_light, long *dst_dark, long *light, long *dark);

void (*fade_funcs[])(long *dst_light, long *dst_dark, long *light, long *dark) =
	{fade_1, fade_2, fade_3, fade_4, fade_5, fade_6};

char door_init(DOOR_HEADER *header, short number)
{
	short i, x, y, w, h;
	unsigned char val;
	short byte;
	char bit;

	door_number = number;

	if(number == 0) return TRUE;

	doors = malloc(number * sizeof(DOOR));
	if(!doors) return FALSE;
	memset(doors, 0, sizeof(DOOR) * number);

	for(i = 0 ; i < number ; i++) {
		doors[i].x = header[i].x * 12;
		doors[i].y = header[i].y * 12;
		doors[i].y_target = doors[i].y;
		doors[i].index = header[i].index;
		doors[i].type = header[i].type;
		doors[i].target_map = header[i].target_map;
		doors[i].target_door = header[i].target_door;
		doors[i].flash = FALSE;
		doors[i].status = DOOR_CLOSED;
		if(header[i].index >= 0) {
			//ClrScr();
			//printf_xy(0, 0, "%d", header[i].index);
			//pause();

			byte = header[i].index >> 3;
			bit = header[i].index & 7;
			if(opened_doors[byte] & (1 << bit)) {
				doors[i].level = DOOR_NORMAL;
			} else doors[i].level = header[i].level;
		} else doors[i].level = header[i].level;

		if(doors[i].level == DOOR_MISSILE)
			doors[i].hp = 3;
		else doors[i].hp = 1;

		if(header[i].type == DOOR_LEFT || header[i].type == DOOR_RIGHT) {
			w = 2; h = 4;
		} else if(header[i].type >= ELEVATOR_UP) {
			w = 2; h = 1;
			doors[i].status = DOOR_OPEN;
		} else {
			w = 4; h = 2;
		}

		doors[i].width = w * 12;
		doors[i].hieght = h * 12;

		if(doors[i].level == DOOR_BLANK) doors[i].status = DOOR_OPEN;
		else {
			for(x = 0 ; x < w ; x++)
				for(y = 0 ; y < h ; y++) {
					val = SOLID;
					if((header[i].type == DOOR_LEFT && x == 1) || (header[i].type == DOOR_RIGHT && x == 0) ||
						(header[i].type == DOOR_UP && y == 1) || (header[i].type == DOOR_DOWN && y == 0))
							val |= 0x80;

					glbs->current_map.prop_data[(y + header[i].y) * glbs->current_map.width + x + header[i].x] = val;
				}
		}

	}

	return TRUE;
}

void door_reset()
{
	door_cleanup();
}

void door_cleanup()
{
	if(doors != NULL) {
		free(doors);
		doors = NULL;
	}
}

void door_bomb_switch_check()
{
	short i;

	for(i = 0 ; i < door_number ; i++) {
		if(doors[i].level == DOOR_BOMB_SWITCH && doors[i].status == DOOR_CLOSED)
			door_open(doors + i, DAMAGE_BOMB_SWITCH);
	}
}

char door_open(DOOR *door, short damage_type)
{
	short x, y;
	short byte;
	char bit;

	if(door->level == DOOR_MISSILE) {
		if(damage_type == DAMAGE_MISSILE) {
			door->hp--;
			door->flash = TRUE;
			if(door->hp) return FALSE;
		} else if(damage_type == DAMAGE_SUPERMISSILE) door->hp = 0;
		else return TRUE;
	} else if(door->level == DOOR_SUPERMISSILE) {
		if(damage_type != DAMAGE_SUPERMISSILE) return TRUE;
	} else if(door->level >= DOOR_ENEMY_REMEMBER && door->level < DOOR_ENEMY_OPENED &&
		damage_type != DAMAGE_BOMB_SWITCH) return TRUE;

	if(door->index != -1) {
		byte = door->index >> 3;
		bit = door->index & 7;
		opened_doors[byte] |= (1 << bit);
	}

	door->status = DOOR_OPENING;
	door->frame = 1;
	door->frame_counter = 7;

	for(x = 0 ; x < door->width / 12 ; x++)
			for(y = 0 ; y < door->hieght / 12 ; y++)
				glbs->current_map.prop_data[(y + door->y / 12) * glbs->current_map.width + x + door->x / 12] = EMPTY;

	return TRUE;
}

void fade_1(long *dst_light, long *dst_dark, long *light, long *dark)
{
	*dst_light = (*light | ~(*dark));
	*dst_dark = *dark;
}

void fade_2(long *dst_light, long *dst_dark, long *light, long *dark)
{
	*dst_light = ~(*light ^ *dark);
	*dst_dark = (*light | *dark);
}

void fade_3(long *dst_light, long *dst_dark, long *light, long *dark)
{
	*dst_light = (~(*light) | *dark);
	*dst_dark = (*light | *dark);
}

void fade_4(long *dst_light, long *dst_dark, long *light, long *dark)
{
	*dst_light = (*light & *dark);
	*dst_dark = 0xffffffff;
}

void fade_5(long *dst_light, long *dst_dark, long *light, long *dark)
{
	*dst_light = (~(*light) | ~(*dark));
	*dst_dark = 0xffffffff;
}

void fade_6(long *dst_light, long *dst_dark, long *light, long *dark)
{
	*dst_light = 0xffffffff;
	*dst_dark = 0xffffffff;
}

void blacken_screen()
{
	memset(glbs->light_buffer, 0xff, LCD_SIZE);
	memset(glbs->dark_buffer, 0xff, LCD_SIZE);
}

void door_draw_stub(short x, short y, short type)
{
	SPRITE_HEADER *header = glbs->door_header + type * 17;

	if(type == DOOR_LEFT) x += 6;
	else if(type == DOOR_UP) y += 6;

	GraySpriteClipX8_MASK(x, y, header->hieght,
			glbs->door_gfx + header->offset, header->width, FALSE);
}

void fade(short b, long *light, long *dark, DOOR *door, char draw_player)
{
	register long *dst_light = glbs->light_buffer;
	register long *dst_dark = glbs->dark_buffer;
	register short i;

	glbs->timer = 25;

	if(glbs->display_width == 192) {
		for(i = 0 ; i < glbs->display_hieght ; i++) {
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);

			light = (void *)((char *)light + (6));
			dark = (void *)((char *)dark + (6));
			dst_light = (void *)((char *)dst_light + (6));
			dst_dark = (void *)((char *)dst_dark + (6));
		}

	} else {

		for(i = 0 ; i < glbs->display_hieght ; i++) {
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);
			(*fade_funcs[b])(dst_light++, dst_dark++, light++, dark++);

			light = (void *)((char *)light + (10));
			dark = (void *)((char *)dark + (10));
			dst_light = (void *)((char *)dst_light + (10));
			dst_dark = (void *)((char *)dst_dark + (10));
		}
	}

	if(door != NULL && door->level != DOOR_BLANK)
		door_draw_stub(door->x - glbs->camera.x, door->y - glbs->camera.y, door->type);
	if(draw_player) {
		glbs->player.flash = 0;
		player_draw();
	} else bar_draw();
	update_screen();
	while(glbs->timer);
}

void fade_out(long *light, long *dark, DOOR *door)
{
	short i;

	for(i = 0 ; i < 6 ; i++) fade(i, light, dark, door, FALSE);

	//glbs->timer = 25;
	//blacken_screen();
	//if(door != NULL && door->level != DOOR_BLANK)
	//	door_draw_stub(door->x - glbs->camera.x, door->y - glbs->camera.y, door->type);
	//bar_draw();
	//update_screen();
	//while(glbs->timer);
}

void fade_out_player(long *light, long *dark)
{
	short i;

	for(i = 0 ; i < 6 ; i++) fade(i, light, dark, NULL, TRUE);

	//glbs->timer = 25;
	//blacken_screen();
	//player_draw();
	//update_screen();
	//while(glbs->timer);
}

void fade_in(long *light, long *dark, DOOR *door)
{
	short i;

	for(i = 4 ; i >= 0 ; i--) fade(i, light, dark, door, FALSE);
}

void elevator_enter(short i)
{
	DOOR *d = &doors[i];

	if(d->type == ELEVATOR_UP) d->status = ELEVATOR_GOING_UP;
	else d->status = ELEVATOR_GOING_DOWN;
	player_set_action(ACT_FRONT_WAIT);
	player_set_position(d->x - 3, glbs->player.e.y);
	bar_update();
}

void door_enter(DOOR *door)
{
	DOOR old_door;
	short target_door = door->target_door;
	short player_x_off = glbs->player.e.x - door->x;
	short player_y_off = glbs->player.e.y - door->y;
	short old_door_x = door->x - glbs->camera.x;
	short old_door_y = door->y - glbs->camera.y;
	short x = 0, y = 0;
	short i;
	char loop;
	char light[LCD_SIZE];
	char dark[LCD_SIZE];

	memcpy(&old_door, door, sizeof(DOOR));

	memcpy(light, glbs->light_buffer, LCD_SIZE);
	memcpy(dark, glbs->dark_buffer, LCD_SIZE);

	fade_out((long *)light, (long *)dark, &old_door);

	set_map(old_door.target_map);

	if(doors[target_door].type == DOOR_LEFT) {
		x = doors[target_door].x - 11 - 2 - glbs->player.e.width;
		y = doors[target_door].y + player_y_off;
	} else if(doors[target_door].type == DOOR_RIGHT) {
		x = doors[target_door].x + 23 + 2 - glbs->player.e.x_off;
		y = doors[target_door].y + player_y_off;
	} else if(doors[target_door].type == DOOR_UP) {
		x = doors[target_door].x + player_x_off;
		y = doors[target_door].y - 11 - 2 - glbs->player.e.hieght;
	} else if(doors[target_door].type == DOOR_DOWN) {
		x = doors[target_door].x + player_x_off;
		y = doors[target_door].y + 23 + 2 - glbs->player.e.y_off;
	}

	player_set_position(x, y);
	focus_camera();
	bar_update();

	if(old_door.level != DOOR_BLANK && doors[target_door].level != DOOR_BLANK) {
		glbs->game_counter = 1; loop = TRUE;
		while(loop) {
			while(glbs->game_counter > 0) {
				if(old_door.type == DOOR_LEFT || old_door.type == DOOR_RIGHT) {
					if(old_door_y < doors[target_door].y - glbs->camera.y) old_door_y++;
					if(old_door_y > doors[target_door].y - glbs->camera.y) old_door_y--;
					if(old_door_y == doors[target_door].y - glbs->camera.y) loop = FALSE;
				} else {
					if(old_door_x < doors[target_door].x - glbs->camera.x) old_door_x++;
					if(old_door_x > doors[target_door].x - glbs->camera.x) old_door_x--;
					if(old_door_x == doors[target_door].x - glbs->camera.x) loop = FALSE;
				}
				bar_process();
				glbs->game_counter--;
			}

			blacken_screen();
			door_draw_stub(old_door_x, old_door_y, old_door.type);
			bar_draw();
			update_screen();
		}

		glbs->game_counter = 1; i = 0; loop = TRUE;
		while(loop) {
			while(glbs->game_counter > 0) {
				if(old_door.type == DOOR_LEFT)
					old_door_x -= 4;
				else if(old_door.type == DOOR_RIGHT)
					old_door_x += 4;
				else if(old_door.type == DOOR_UP)
					old_door_y -= 4;
				else
					old_door_y += 4;

				glbs->game_counter--;
				i++;
				if(i == glbs->display_width / 4 && (old_door.type == DOOR_LEFT || old_door.type == DOOR_RIGHT))
					loop = FALSE;
				else if(i == glbs->display_hieght / 4 && (old_door.type == DOOR_UP || old_door.type == DOOR_DOWN))
					loop = FALSE;
				bar_process();
			}

			blacken_screen();
			door_draw_stub(old_door_x, old_door_y, old_door.type);
			if(old_door.type == DOOR_LEFT) door_draw_stub(old_door_x + 24, old_door_y, DOOR_RIGHT);
			else if(old_door.type == DOOR_RIGHT) door_draw_stub(old_door_x - 24, old_door_y, DOOR_LEFT);
			else if(old_door.type == DOOR_UP) door_draw_stub(old_door_x, old_door_y + 24, DOOR_DOWN);
			else door_draw_stub(old_door_x, old_door_y - 24, DOOR_UP);
			bar_draw();
			update_screen();
		}
	}

	doors[target_door].status = DOOR_OPEN;

	screen_draw();

	memcpy(light, glbs->light_buffer, LCD_SIZE);
	memcpy(dark, glbs->dark_buffer, LCD_SIZE);

	fade_in((long *)light, (long *)dark, doors + target_door);

	if(doors[target_door].level != DOOR_BLANK) {
		doors[target_door].status = DOOR_CLOSING;
		doors[target_door].frame = 3;
		doors[target_door].frame_counter = 7;
	}

	game_write(&glbs->save_game);

	glbs->game_counter = 0;
}

void elevator_change_map(short i)
{
	DOOR *d = doors + i;
	short target_elevator = d->target_door;
	char direction = d->status;
	char light[LCD_SIZE];
	char dark[LCD_SIZE];
	short yc = glbs->player.e.y_off + glbs->player.e.hieght;

	memcpy(light, glbs->light_buffer, LCD_SIZE);
	memcpy(dark, glbs->dark_buffer, LCD_SIZE);
	fade_out((long *)light, (long *)dark, NULL);

	set_map(d->target_map);
	d = doors + target_elevator;
	d->status = direction + 2;
	if(direction == ELEVATOR_GOING_DOWN) d->y = -5;
	else d->y = glbs->current_map.hieght * 12;
	player_set_position(d->x - 3, d->y - yc);
	focus_camera();
	screen_draw();
	memcpy(light, glbs->light_buffer, LCD_SIZE);
	memcpy(dark, glbs->dark_buffer, LCD_SIZE);
	fade_in((long *)light, (long *)dark, NULL);

	glbs->game_counter = 0;
}

void door_process()
{
	short i;
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short pw = glbs->player.e.width;
	short ph = glbs->player.e.hieght;
	short dx, dy, dw, dh;
	char type;

	for(i = 0 ; i < door_number ; i++) {
		type = doors[i].type;

		if(type >= ELEVATOR_UP) {
			if(doors[i].status == DOOR_OPEN) continue;
			else if(doors[i].status == ELEVATOR_GOING_UP || doors[i].status == ELEVATOR_COMMING_UP) {
				doors[i].y--;
				glbs->player.e.y--;
				player_screen_up();
				camera_process_up(Y_THRESH_START, 2);
				if(glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght == 0) {
					elevator_change_map(i);
					return;
				}
				if(doors[i].y == doors[i].y_target) {
					doors[i].status = DOOR_OPEN;
					player_set_action(ACT_FRONT_READY);
					game_write(&glbs->save_game);
				}
			} else {
				doors[i].y++;
				glbs->player.e.y++;
				player_screen_down();
				camera_process_down(Y_THRESH_START, 2);
				if(glbs->player.e.y == glbs->current_map.hieght * 12) {
					elevator_change_map(i);
					return;
				}
				if(doors[i].y == doors[i].y_target) {
					doors[i].status = DOOR_OPEN;
					player_set_action(ACT_FRONT_READY);
					game_write(&glbs->save_game);
				}
			}
			continue;
		}

		if(doors[i].status == DOOR_OPEN) {
			dx = doors[i].x; dy = doors[i].y;
			if(type == DOOR_LEFT) dx += 18;
			else if(type == DOOR_UP) dy += 18;

			if(type == DOOR_LEFT || type == DOOR_RIGHT) {
				dw = 6; dh = 48;
			} else {
				dw = 48; dh = 6;
			}

			if(rect_overlap(px, py, pw, ph, dx, dy, dw, dh)) {
				door_enter(doors + i);
				return;
			}
		}

		if((doors[i].level == DOOR_ENEMY_REMEMBER || doors[i].level == DOOR_ENEMY_RESET) &&
			enemy_kills == enemy_number) doors[i].level = DOOR_ENEMY_OPENED;

		if(doors[i].status == DOOR_OPEN || doors[i].status == DOOR_CLOSED) continue;

		doors[i].frame_counter--;
		if(doors[i].frame_counter == 0) {
			if(doors[i].status == DOOR_OPENING) {
				doors[i].frame++;
				if(doors[i].frame == 4) doors[i].status = DOOR_OPEN;
			} else {
				doors[i].frame--;
				if(doors[i].frame == 0) doors[i].status = DOOR_CLOSED;
			}
			doors[i].frame_counter = 7;
		}
	}
}

void door_draw()
{
	SPRITE_HEADER *header = NULL;
	register short i;
	register short x_pos;
	register short y_pos;

	for(i = 0 ; i < door_number ; i++) {
		if(doors[i].level == DOOR_BLANK) continue;
		x_pos = doors[i].x - glbs->camera.x;
		y_pos = doors[i].y - glbs->camera.y;

		if(doors[i].type >= ELEVATOR_UP) {
			doors[i].frame = !doors[i].frame;
			GraySpriteClipX8_MASK(x_pos, y_pos, 5,
				glbs->elevator_gfx + doors[i].frame * 45, 3, FALSE);
			continue;
		}

		header = glbs->door_header + doors[i].type * 17;
		if(doors[i].type == DOOR_LEFT) x_pos += 6;
		else if(doors[i].type == DOOR_UP) y_pos += 6;

		GraySpriteClipX8_MASK(x_pos, y_pos, header->hieght,
			glbs->door_gfx + header->offset, header->width, FALSE);

		if(doors[i].status == DOOR_OPEN) continue;

		header += 1 + doors[i].frame;
		if(doors[i].level == DOOR_MISSILE) header += 4;
		else if(doors[i].level == DOOR_SUPERMISSILE) header += 8;
		else if(doors[i].level >= DOOR_ENEMY_REMEMBER) header += 12;

		if(doors[i].type == DOOR_LEFT) x_pos -= 6;
		else if(doors[i].type == DOOR_RIGHT) x_pos += 18;
		else if(doors[i].type == DOOR_UP) y_pos -= 6;
		else if(doors[i].type == DOOR_DOWN) y_pos += 18;

		GraySpriteClipX8_MASK(x_pos + header->x_offset, y_pos + header->y_offset, header->hieght,
			glbs->door_gfx + header->offset, header->width, doors[i].flash);

		if(doors[i].level == DOOR_ENEMY_OPENED) doors[i].flash = !doors[i].flash;
		else if(doors[i].flash) doors[i].flash = FALSE;
	}
}
