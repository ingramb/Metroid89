// C Source File
// Created 1/8/02; 10:57:04 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>
#include "tiles.h"
#include "entity.h"
#include "enemy.h"
#include "door.h"
#include "player.h"
#include "map.h"
#include "utility.h"
#include "shots.h"
#include "bomb.h"
#include "miscgfx.h"
#include "bar.h"
#include "system.h"
#include "blur.h"
#include "environment.h"
#include "animation.h"
#include "clipsprites.h"
#include "globals.h"

void screen_draw();

char beam_select_wait;
char missile_select_wait;
char supermissile_select_wait;
char powerbomb_select_wait;
char reverse_g_select_wait;

void player_continue_action();
void player_next_action();

PLAYER_ACTION *player_action;
unsigned char **explored_map = NULL;

char plus_down = 0;
char minus_down = 0;

//#define player_action _DLL_glbvar(const PLAYER_ACTION *, DLL_PLAYER_ACTION)
//extern const PLAYER_ACTION player_action[];


//#define player_action _DLL_reference(PLAYER_ACTION, DLL_PLAYER_ACTION)

const ENTITY_SIZE player_size[] = {
	{11, 13, 10, 34, 32, 48, TRUE},
	{11, 25, 10, 22, 32, 48, TRUE},
	{11, 37, 10, 10, 32, 48, TRUE}
};

//max hieght of 34, width of 10 = 2 bytes, 2 planes....34 * 2 * 2 bytes
const unsigned char player_size_mask[34 * 2 * 2] = {
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0, 0xff, 0xc0,
	0xff, 0xc0, 0xff, 0xc0,
};

const unsigned char leach_link[] = {
	0b00110000,
	0b01001000,
	0b10110100,
	0b10110100,
	0b01001000,
	0b00110000,

	0b00110000,
	0b01001000,
	0b10110100,
	0b10110100,
	0b01001000,
	0b00110000,

	0b00110000,
	0b01111000,
	0b11111100,
	0b11111100,
	0b01111000,
	0b00110000,
};

unsigned char *player_gfx;
SPRITE_HEADER *player_header;
short frame_number;

char player_setup()
{
	void *file;
	short i;

	file = file_pointer(SYMSTR("samus"));
	if(file == NULL) return FALSE;

	// samus gfx is big-endian (SpriteMaker output): swap the frame count and
	// each SPRITE_HEADER.offset to host order.
	frame_number = *((short *)file);
	frame_number = (short)(((unsigned short)frame_number >> 8) | ((unsigned short)frame_number << 8));
	player_header = (SPRITE_HEADER *)(file + 2);
	{
		short f;
		for(f = 0 ; f < frame_number ; f++) {
			unsigned short o = player_header[f].offset;
			player_header[f].offset = (unsigned short)((o >> 8) | (o << 8));
		}
	}
	player_gfx = (unsigned char *)(file + 2 + frame_number * sizeof(SPRITE_HEADER));

	beam_select_wait = 0;
	missile_select_wait = 0;
	supermissile_select_wait = 0;
	powerbomb_select_wait = 0;

	player_action = (PLAYER_ACTION *)(glbs->metpack_base + glbs->metpack_header->player_action_offset);

	explored_map = malloc(sizeof(unsigned char *) * zone_number);
	if(!explored_map) return FALSE;
	memset(explored_map, 0, sizeof(unsigned char *) * zone_number);
	for(i = 0 ; i < zone_number ; i++) {
		explored_map[i] = malloc(zone_header[i].width * zone_header[i].hieght);
		if(explored_map[i] == NULL) return FALSE;
		memset(explored_map[i], 81, zone_header[i].width * zone_header[i].hieght);
	}

	return TRUE;
}

void player_cleanup()
{
	short i;

	if(explored_map != NULL) {
		for(i = 0 ; i < zone_number ; i++)
			if(explored_map[i] != NULL) free(explored_map[i]);
		free(explored_map);
		explored_map = NULL;
	}
}

void player_init()
{
	memset(&glbs->player, 0, sizeof(PLAYER));
	entity_init(&(glbs->player.e), 0, 0, &player_size[SIZE_STAND], FALSE);
	glbs->player.direction = RIGHT;
	glbs->player.hp = glbs->player.hp_max = 99;
	glbs->player.overide_action = NONE;
	glbs->player.wall_kick_direction = NONE;
	glbs->player.weapon_selected = BEAM;
	//player_set_position(0, 0);
	player_set_action(ACT_FRONT_READY);
}

void player_set_position(short x, short y)
{
	short x_mid = x + glbs->player.e.x_off + glbs->player.e.width / 2;
	short y_mid = y + glbs->player.e.y_off + glbs->player.e.hieght / 2;

	entity_set_pos(&(glbs->player.e), x, y);
	glbs->player.y_thresh_base = glbs->player.y_thresh = Y_THRESH_START;
	glbs->player.y_scroll_counter = glbs->player.look_counter = 0;
	glbs->player.e.x_speed_counter = 0;
	glbs->player.e.y_speed_counter = 0;
	glbs->player.screen_x = x_mid / (SCREEN_WIDTH * 12);
	glbs->player.screen_y = y_mid / (SCREEN_HIEGHT * 12);
	glbs->player.global_screen_x = glbs->player.screen_x + glbs->current_map.x_pos;
	glbs->player.global_screen_y = glbs->player.screen_y + glbs->current_map.y_pos;
	map_update();
}

short player_get_input_89()
{
	short flags = 0;
	short k = _rowread(ARROWS_ROW);

	if(k & LEFT_KEY) flags |= LEFT_PRESSED;
	if(k & RIGHT_KEY) flags |= RIGHT_PRESSED;
	if(k & UP_KEY) flags |= UP_PRESSED;
	if(k & DOWN_KEY) flags |= DOWN_PRESSED;
	if(k & SEL_KEY) flags |= SHOOT_PRESSED;
	if(k & DMND_KEY) flags |= JUMP_PRESSED;

	if(_rowread(BSPACE_ROW) & BSPACE_KEY) flags |= DIAG_UP_PRESSED;
	if(_rowread(CLEAR_ROW) & CLEAR_KEY) flags |= DIAG_DOWN_PRESSED;
	if(_rowread(APPS_ROW) & APPS_KEY) flags |= MENU_PRESSED;

	if(_rowread(0b1111111111011111) & 128) flags |= BEAM_SELECT_PRESSED;
	if(_rowread(0b1111111111101111) & 128) flags |= MISSILE_SELECT_PRESSED;
	if(_rowread(0b1111111111110111) & 128) flags |= SUPERMISSILE_SELECT_PRESSED;
	if(_rowread(0b1111111111111011) & 128) flags |= POWERBOMB_SELECT_PRESSED;
	if(_rowread(0b1111111111111101) & 128) flags |= REVERSE_G_SELECT_PRESSED;

	if(_rowread(0b1111111111111101) & 2) flags |= PLUS_PRESSED;
	if(_rowread(0b1111111111111101) & 4) flags |= MINUS_PRESSED;

	if(k & SHIFT_KEY) {
		short i;
		glbs->player.hp = 2099;
		glbs->player.hp_max = 2099;
		glbs->player.items_found = 0xffffffff;
		glbs->player.items_equiped = 0xffffffff;
		glbs->player.ammunition[0] = glbs->player.ammunition_max[0] = 250;
		for(i = 1 ; i < 3 ; i++)
			glbs->player.ammunition[i] = glbs->player.ammunition_max[i] = 50;
		bar_update();
	}

	return flags;
}

short player_get_input_92()
{
	short flags = 0;
	short k = _rowread(ARROWS_ROW);

	if(k & 16) flags |= LEFT_PRESSED;
	if(k & 64) flags |= RIGHT_PRESSED;
	if(k & 32) flags |= UP_PRESSED;
	if(k & 128) flags |= DOWN_PRESSED;
	if(k & 1) flags |= SHOOT_PRESSED;
	if(k & 2) flags |= JUMP_PRESSED;

	if(_rowread(0b1111111101111111) & 64) flags |= DIAG_UP_PRESSED;
	if(_rowread(0b1111111110111111) & 64) flags |= DIAG_DOWN_PRESSED;

	if(_rowread(0b1111111110111111) & 16) flags |= BEAM_SELECT_PRESSED;
	if(_rowread(0b1111111111101111) & 16) flags |= MISSILE_SELECT_PRESSED;
	if(_rowread(0b1111111111111011) & 16) flags |= SUPERMISSILE_SELECT_PRESSED;
	if(_rowread(0b1111110111111111) & 16) flags |= POWERBOMB_SELECT_PRESSED;

	return flags;
}

void player_screen_up()
{
	short y0 = glbs->player.e.y + glbs->player.e.y_off;

	if(y0 < glbs->player.screen_y * SCREEN_HIEGHT * 12) {
		glbs->player.screen_y--;
		if(glbs->player.screen_y < 0) glbs->player.screen_y = 0;
		glbs->player.global_screen_y = glbs->player.screen_y + glbs->current_map.y_pos;
		map_update();
	}
}

void player_screen_down()
{
	short y1 = glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght;

	if(y1 > (glbs->player.screen_y + 1) * SCREEN_HIEGHT * 12) {
		glbs->player.screen_y++;
		if(glbs->player.screen_y == glbs->current_map.screen_hieght) glbs->player.screen_y = glbs->current_map.screen_hieght - 1;
		glbs->player.global_screen_y = glbs->player.screen_y + glbs->current_map.y_pos;
		map_update();
	}
}

unsigned long player_item_found(unsigned long i)
{
	/*unsigned long rv = glbs->player.items_found;

	asm("move.l #1, %%d0; lsr.l %1, %%d0; and.l %%d0, %0"
		: "=g" (rv)
		: "r" (i)
		: "d0"
	);

	return rv;*/

	return (glbs->player.items_found >> i) & 1L;
}

unsigned short player_item_equiped(unsigned long i)
{
	/*unsigned long rv = glbs->player.items_equiped;

	asm("move.l #1, %%d0; lsr.l %1, %%d0; and.l %%d0, %0"
		: "=g" (rv)
		: "r" (i)
		: "d0"
	);

	return rv;*/

	return (glbs->player.items_equiped >> i) & 1L;
}

char player_move(char direction, short speed)
{
	ENTITY *e = &(glbs->player.e);
	char i;
	char x_dir = 0;
	char y_dir = 0;
	char x_speed;
	char y_speed;
	char down_base, up_base;
	short x0;
	short x1;

	if(direction & LEFT) x_dir = -1;
	if(direction & RIGHT) x_dir = 1;
	if(direction & UP) y_dir = -1;
	if(direction & DOWN) y_dir = 1;

	if(e->gravity == UP) y_dir *= -1;

	i = entity_move_internal(e, x_dir, y_dir, speed);

	if(e->y_moved < 0) {
		glbs->player.jump_hieght += e->y_moved;
		glbs->player.fall_distance -= e->y_moved;
	} else {
		glbs->player.jump_hieght -= e->y_moved;
		glbs->player.fall_distance += e->y_moved;
	}

	glbs->player.y_scroll_counter += e->y_moved;

	down_base = 10 - glbs->player.y_thresh_base + Y_THRESH_START;
	if(down_base < 1) down_base = 1;
	up_base = -10 - glbs->player.y_thresh_base + Y_THRESH_START;
	if(up_base > -1) up_base = -1;

	if(Y_THRESH_START - glbs->player.y_thresh_base < 14) {
		while(glbs->player.y_scroll_counter >= down_base) {
				glbs->player.y_thresh_base--;
				glbs->player.y_scroll_counter -= down_base;
		}
	} else if(glbs->player.y_scroll_counter > 0) glbs->player.y_scroll_counter = 0;

	if(glbs->player.y_thresh_base - Y_THRESH_START < 14) {
		while(glbs->player.y_scroll_counter <= up_base) {
				glbs->player.y_thresh_base++;
				glbs->player.y_scroll_counter -= up_base;
		}
	} else if(glbs->player.y_scroll_counter < 0) glbs->player.y_scroll_counter = 0;

	if(!i) {
		if(glbs->player.sprint_state == 3 && x_dir && y_dir == 0) glbs->player.sprint_state = 4;//sprinted into wall
		return FALSE;
	}

	x0 = e->x + e->x_off;
	x1 = e->x + e->x_off + e->width;

	if(e->x_moved > 0)
		if(x1 > (glbs->player.screen_x + 1) * SCREEN_WIDTH * 12) {
			glbs->player.screen_x++;
			if(glbs->player.screen_x == glbs->current_map.screen_width) glbs->player.screen_x = glbs->current_map.screen_width - 1;
			glbs->player.global_screen_x = glbs->player.screen_x + glbs->current_map.x_pos;
			map_update();
		}
	if(e->x_moved < 0)
		if(x0 < glbs->player.screen_x * SCREEN_WIDTH * 12) {
			glbs->player.screen_x--;
			if(glbs->player.screen_x < 0) glbs->player.screen_x = 0;
			glbs->player.global_screen_x = glbs->player.screen_x + glbs->current_map.x_pos;
			map_update();
		}
	if(e->y_moved > 0)
		player_screen_down();
	if(e->y_moved < 0)
		player_screen_up();


	x_speed = e->x_moved << 1;
	y_speed = e->y_moved << 1;
	if(x_speed < 0) x_speed *= -1;
	if(y_speed < 0) y_speed *= -1;

	if(x_speed > y_speed) i = camera_process(x_speed);
	else i = camera_process(y_speed);

	if(e->x_moved < 0 && !(i & LEFT)) {
		camera_process_left(88, x_speed);
	} else if(glbs->player.e.x_moved > 0 && !(i & RIGHT)) {
		camera_process_right(40, x_speed);
	}

	if(e->y_moved < 0 && !(i & UP)) {
		camera_process_up(glbs->player.y_thresh, y_speed);
	} else if(e->y_moved > 0 && !(i & DOWN)) {
		camera_process_down(glbs->player.y_thresh, y_speed);
	}

	return TRUE;
}

char player_check_elevator(char direction)
{
	short i;

	for(i = 0 ; i < door_number ; i++) {
		if(doors[i].type < ELEVATOR_UP) continue;
		if(doors[i].x < glbs->player.e.x + glbs->player.e.x_off &&
			doors[i].x + 24 > glbs->player.e.x + glbs->player.e.x_off + glbs->player.e.width &&
			doors[i].y == glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght &&
			((direction == DOWN && doors[i].type == ELEVATOR_DOWN) ||
			(direction == UP && doors[i].type == ELEVATOR_UP))) {
				elevator_enter(i);
				return TRUE;
		}
	}

	return FALSE;
}

void player_step(char direction)
{

	if(glbs->player.overide_action != NONE || glbs->player.action_duration) return;

	if(direction == LEFT || direction == RIGHT){

		if(glbs->player.direction != direction) {
			if(glbs->player.sprint_state == 4) return; //can't change direction while slowing down
			if(direction == LEFT){
				if(glbs->player.action == ACT_DUCK)
					player_set_action(ACT_DUCK_TURN_RIGHT);
				else if(glbs->player.action_type == TYPE_STAND)
					player_set_action(ACT_TURN_RIGHT);
				else if(!(glbs->player.action_type & TYPE_FLIP) && !(glbs->player.action_type & TYPE_BALL))
					player_set_action(ACT_AIR_TURN_RIGHT);

			}

			if(direction == RIGHT){
				if(glbs->player.action == ACT_DUCK)
					player_set_action(ACT_DUCK_TURN_LEFT);
				else if(glbs->player.action_type == TYPE_STAND)
					player_set_action(ACT_TURN_LEFT);
				else if(!(glbs->player.action_type & TYPE_FLIP) && !(glbs->player.action_type & TYPE_BALL))
					player_set_action(ACT_AIR_TURN_LEFT);
			}

			glbs->player.direction = direction;
		} else {
				if(glbs->player.action == ACT_DUCK) {
					if(entity_check_size(&(glbs->player.e), &player_size[SIZE_STAND])) player_set_action(ACT_STAND_UP);
				} else if((glbs->player.action_type & TYPE_STAND)) {
					if(glbs->player.action != ACT_RUN/* && entity_check_direction(&(glbs->player.e), direction)*/) {
						//Hasn't pressed key yet
						if(glbs->player.sprint_state == 0) {
							glbs->player.sprint_state = 1;
							glbs->player.sprint_counter = SPRINT_DELAY;
							glbs->player.walk_speed = (WALK_SPEED_START >> glbs->player.in_water);
						//Second press
						} else if(glbs->player.sprint_state == 2 && !glbs->player.in_water) {
							glbs->player.sprint_state = 3;
							glbs->player.sprint_direction = glbs->player.direction;
						}

						//player_set_action(ACT_RUN);
					}
					if(glbs->player.walk_speed < (WALK_SPEED_MAX - (glbs->player.in_water << 5))) {
						if(glbs->player.in_water) glbs->player.walk_speed++;
						else glbs->player.walk_speed += WALK_SPEED_INCREASE;
					}
					else if(glbs->player.sprint_state == 3 && glbs->player.walk_speed < SPRINT_SPEED_MAX)
						glbs->player.walk_speed += WALK_SPEED_INCREASE;
					else if(glbs->player.sprint_state == 3 && player_item_equiped(SPEED_BOOTS) &&
						glbs->player.walk_speed < TURBO_SPEED_MAX)
							glbs->player.walk_speed += TURBO_SPEED_INCREASE;

					if(player_move(direction, glbs->player.walk_speed)) {
						if(glbs->player.action != ACT_RUN && glbs->player.e.x_moved) player_set_action(ACT_RUN);
						else player_continue_action();
					} else player_next_action();
				} else if(glbs->player.action == ACT_BALL)
					player_move(direction, 100 - (glbs->player.in_water << 5));
				else if(glbs->player.action < ACT_FLIP_START) {
					if(glbs->player.sprint_state == 3) //falling while sprinting
						player_move(direction, glbs->player.walk_speed);
					else
						player_move(direction, 75 - (glbs->player.in_water << 5));
				}
		}

	} else if(direction == DOWN){

		if(glbs->player.action == ACT_STAND ||
			(glbs->player.sprint_state == 4 && glbs->player.gun_position != GUN_DIAG_DOWN)) {
			//can duck while decelerating
				if(player_check_elevator(DOWN)) return;
				player_set_action(ACT_DUCK_DOWN);
				glbs->player.down_pressed = TRUE;
		} else if(glbs->player.action == ACT_DUCK && !glbs->player.down_pressed &&
			player_item_equiped(MORPHING_BALL)) {
				player_set_action(ACT_BALL_DOWN);
				player_set_action(ACT_BALL);
				glbs->player.down_pressed = TRUE;
		} else if(glbs->player.action == ACT_JUMP_UP && glbs->player.gun_position == GUN_DOWN && !glbs->player.down_pressed &&
			player_item_equiped(MORPHING_BALL)) {
				player_set_action(ACT_BALL_DOWN);
				player_set_action(ACT_BALL_JUMP_UP);
				glbs->player.down_pressed = TRUE;
		} else if((glbs->player.action == ACT_JUMP_DOWN || glbs->player.action == ACT_FALL) &&
				glbs->player.gun_position == GUN_DOWN && !glbs->player.down_pressed &&
				player_item_equiped(MORPHING_BALL)) {
					player_set_action(ACT_BALL_DOWN);
					player_set_action(ACT_BALL_JUMP_DOWN);
					glbs->player.down_pressed = TRUE;
		}

	} else if(direction == UP){

		if(glbs->player.action == ACT_DUCK && !glbs->player.up_pressed) {
			if(entity_check_size(&(glbs->player.e), &player_size[SIZE_STAND])) player_set_action(ACT_STAND_UP);
			glbs->player.up_pressed = TRUE;
		} else if(glbs->player.action == ACT_BALL) {
			if(entity_check_size(&(glbs->player.e), &player_size[SIZE_DUCK])) {
				player_set_action(ACT_BALL_UP);
				player_set_action(ACT_DUCK);
			}
			glbs->player.up_pressed = TRUE;
		} else if(glbs->player.action == ACT_BALL_JUMP_UP) {
			if(entity_check_size(&(glbs->player.e), &player_size[SIZE_STAND])) {
				player_set_action(ACT_BALL_UP);
				player_set_action(ACT_JUMP_UP);
			}
			glbs->player.up_pressed = TRUE;
		} else if(glbs->player.action == ACT_BALL_JUMP_DOWN) {
			if(entity_check_size(&(glbs->player.e), &player_size[SIZE_STAND])) {
				player_set_action(ACT_BALL_UP);
				player_set_action(ACT_JUMP_DOWN);
			}
			glbs->player.up_pressed = TRUE;
		} else if(glbs->player.action == ACT_STAND) {
			if(player_check_elevator(UP)) return;
		}
	}

}

void player_jump()
{

	if((glbs->player.overide_action == ACT_WALL_KICK_READY) && !glbs->player.jumped && !glbs->player.in_water)
		if((_rowread(ARROWS_ROW) & LEFT_KEY) || (_rowread(ARROWS_ROW) & RIGHT_KEY)) {
			if(player_item_equiped(SPACE_JUMP) || player_item_equiped(SCREW_ATTACK))
				player_set_action(ACT_WALL_KICK_SPACE);
			else
				player_set_action(ACT_WALL_KICK);

			glbs->player.overide_action = NONE;
			glbs->player.jump_hieght = WALL_KICK_HIEGHT;
			glbs->player.jump_speed = 150;
			glbs->player.jumped = TRUE;
		}

	if(glbs->player.turbo_counter > 0 && !glbs->player.jumped && ((glbs->player.action_type & TYPE_FLIP) || glbs->player.action == ACT_FALL)) {
			player_set_action(ACT_TURBO_JUMP_START);
			glbs->player.turbo_counter = 0;
			return;
	}

	if((glbs->player.action == ACT_SPACE_JUMP_DOWN) && player_item_equiped(SPACE_JUMP) && !glbs->player.in_water) {
		if(glbs->player.jumped) return;
		if(glbs->player.fall_distance < 12) return;
		player_set_action(ACT_SPACE_JUMP_UP);
		glbs->player.jumped = TRUE;
		glbs->player.jump_speed = 150;
		glbs->player.jump_hieght = SPACE_JUMP_HIEGHT;
	}

	if(!glbs->player.jumped &&
		(glbs->player.action == ACT_JUMP_UP || glbs->player.action == ACT_JUMP_DOWN || glbs->player.action == ACT_FALL)) {
			if(player_item_equiped(SPACE_JUMP) || player_item_equiped(SCREW_ATTACK))
				player_set_action(ACT_SPACE_JUMP_DOWN);
			else
				player_set_action(ACT_FLIP_JUMP_DOWN);
			glbs->player.jumped = TRUE;
			return;
	}

	if((glbs->player.action_type & TYPE_JUMP_DOWN) || (glbs->player.action_type & TYPE_BOUNCE)) return;

	if(!(glbs->player.action_type & TYPE_JUMP_UP)) {
		if(glbs->player.jumped) return;

		glbs->player.jump_hieght = 0;

		if(glbs->player.action_type & TYPE_BALL) {
			if(!player_item_equiped(SPRING_BALL)) return;
			player_set_action(ACT_BALL_JUMP_UP);
			//glbs->player.jumped = TRUE;
		} else if(glbs->player.action == ACT_DUCK) {
			if(entity_check_size(&(glbs->player.e), &player_size[SIZE_STAND])) player_set_action(ACT_STAND_UP);
			return;
		} else if(glbs->player.action == ACT_RUN) {
			if(player_item_equiped(SPACE_JUMP) || player_item_equiped(SCREW_ATTACK))
				player_set_action(ACT_SPACE_JUMP_START);
			else
				player_set_action(ACT_FLIP_START);
			if(glbs->player.walk_speed > SPRINT_SPEED_MAX)
				glbs->player.jump_hieght += (glbs->player.walk_speed - SPRINT_SPEED_MAX) >> 1;
			if(glbs->player.gun_position != GUN_NONE) glbs->player.flip_start = TRUE;
			glbs->player.jumped = TRUE;
		} else {
			if(glbs->player.turbo_counter > 0) {
				player_set_action(ACT_TURBO_JUMP_START);
				glbs->player.turbo_counter = 0;
				return;
			}
			else player_set_action(ACT_JUMP_UP);
			glbs->player.jumped = TRUE;
		}

		glbs->player.jump_speed = 150 >> glbs->player.in_water;

		if(!player_move(UP, glbs->player.jump_speed))
			return;

		if(player_item_equiped(HIJUMP_BOOTS))
			glbs->player.jump_hieght += HIGHJUMP_HIEGHT;
		else
			glbs->player.jump_hieght += JUMP_HIEGHT;

		glbs->player.jump_hieght = (glbs->player.jump_hieght >> glbs->player.in_water);

	} else {
		if(player_move(UP, glbs->player.jump_speed))
			player_continue_action();




		/*if(glbs->player.action == ACT_JUMP_UP)
			player_set_action(p, ACT_JUMP_UP);
		if(glbs->player.action == ACT_FLIP_JUMP_UP)
			player_set_action(p, ACT_FLIP_JUMP_UP);
		if(glbs->player.action == ACT_BALL_JUMP_UP)
			player_set_action(p, ACT_BALL_JUMP_UP);*/
	}

}

void player_bounce(short hieght, char direction)
{

	if(glbs->player.action_type & TYPE_BALL) player_set_action(ACT_BALL_BOUNCE);
	else player_set_action(ACT_BOUNCE);

	glbs->player.jump_hieght = hieght;
	glbs->player.bounce_direction = direction;
	glbs->player.jump_speed = 150 >> glbs->player.in_water;
	glbs->player.jumped = TRUE;
}

char player_hurt(short damage, short dx, short dy, short blocker)
{
	char d = 0;
	char ok = 0;

	if(glbs->player.invinc_duration && !blocker) return FALSE;
	else if(!glbs->player.invinc_duration) {
		glbs->player.leach_type = LEACH_NONE;

		if(player_item_equiped(VARIA_SUIT)) damage = (damage >> 1);
		if(player_item_equiped(GRAVITY_SUIT)) damage = (damage >> 1);
		if(damage < 1) damage = 1;

		glbs->player.hp -= damage;
		glbs->player.sprint_state = 0;
		glbs->player.invinc_duration = 120;
		bar_update();
	}

	if(glbs->player.action_type & TYPE_BALL) {
		player_set_action(ACT_BALL_BOUNCE);
		glbs->player.bounce_direction = UP;
		ok = 1;
	} else if(entity_check_size(&(glbs->player.e), &player_size[SIZE_STAND])){
		if(dx < glbs->player.e.x + glbs->player.e.x_off) d |= RIGHT;
		else d |= LEFT;
		if(dy < glbs->player.e.y + glbs->player.e.y_off && entity_check_direction(&(glbs->player.e), DOWN)) d |= DOWN;
		else d |= UP;
		player_set_action(ACT_HURT);
		glbs->player.bounce_direction = d;
		ok = 1;
	}

	if(ok) {
		glbs->player.jump_hieght = 12;
		glbs->player.jump_speed = 150 >> glbs->player.in_water;
		//glbs->player.jumped = TRUE;
	}
	
	return (glbs->player.invinc_duration == 0);
}

void player_death()
{
	void *buffer;
	SPRITE_HEADER *hdr, *orig_header, *flip_header;
	unsigned char *gfx, *orig_sprite, *flip_sprite;
	short i, a, f, j;
	unsigned short gfx_size, buffer_size, frame_size;
	short dx = (64 - (glbs->player.e.x - glbs->camera.x)) * 2;
	short dy = (26 - (glbs->player.e.y - glbs->camera.y)) * 2;
	short xc = 0, yc = 0;
	char light[LCD_SIZE];
	char dark[LCD_SIZE];
	unsigned char fade_table[] = {
		0xff, 0xff, 0xff, 0xff,
		0xaa, 0x55, 0xff, 0xff,
		0x00, 0x00, 0xff, 0xff,
		0xff, 0xff, 0xaa, 0x55,
		0xff, 0xff, 0x00, 0x00,
		0xaa, 0x55, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	};

	if(!ttunpack_valid(glbs->samus_death_header) ||
	!ttunpack_valid(glbs->samus_death_gfx)) return;

	gfx_size = ttunpack_size(glbs->samus_death_gfx);
	if(gfx_size % 2) gfx_size++;
	buffer_size = (7 * sizeof(SPRITE_HEADER) + gfx_size) * 2 + 2;
	buffer = malloc(buffer_size);
	if(buffer == NULL) return;
	hdr = buffer + 2;
	gfx = buffer + 7 * sizeof(SPRITE_HEADER) * 2 + 2;
	decompress(glbs->samus_death_header, buffer);
	decompress(glbs->samus_death_gfx, (void *)gfx);

	for(i = 0 ; i < 7 ; i++) {
		orig_header = hdr + i;
		flip_header = hdr + 7 + i;

		memcpy(flip_header, orig_header, sizeof(SPRITE_HEADER));
		flip_header->x_offset = 32 - flip_header->width * 8 - flip_header->x_offset;
		flip_header->offset += gfx_size;

		frame_size = orig_header->hieght * orig_header->width;

		orig_sprite = gfx + orig_header->offset;
		flip_sprite = gfx + flip_header->offset;

		for(a = 0 ; a < 3 ; a++) {
			SpriteX8_MIRROR_H(orig_header->hieght, orig_sprite,
				orig_header->width, flip_sprite);
			orig_sprite += frame_size;
			flip_sprite += frame_size;
		}
	}

	player_set_action(ACT_HURT);
	glbs->player.overide_action = NONE;

	screen_draw();
	memcpy(light, glbs->light_buffer, LCD_SIZE);
	memcpy(dark, glbs->dark_buffer, LCD_SIZE);
	fade_out_player((uint32_t *)light, (uint32_t *)dark);

	while(glbs->player.e.x - glbs->camera.x != 64 && glbs->player.e.y - glbs->camera.y != 26) {
		if(glbs->player.e.x - glbs->camera.x != 64) {
			xc += dx;
			while(xc >= 100 && glbs->player.e.x - glbs->camera.x != 64) { glbs->player.e.x++; xc -= 100; }
			while(xc <= -100 && glbs->player.e.x - glbs->camera.x != 64) { glbs->player.e.x--; xc += 100; }
		}
		if(glbs->player.e.y - glbs->camera.y != 26) {
			yc += dy;
			while(yc >= 100 && glbs->player.e.y - glbs->camera.y != 26) { glbs->player.e.y++; yc -= 100; }
			while(yc <= -100 && glbs->player.e.y - glbs->camera.y != 26) { glbs->player.e.y--; yc += 100; }
		}
		memset(glbs->light_buffer, 0xff, 3000);
		memset(glbs->dark_buffer, 0xff, 3000);
		glbs->player.flash = 0;
		player_draw();
		update_screen();
	}

	if(glbs->player.direction == RIGHT) hdr += 7;

	i = 0, f = 0;

	for(a = 0 ; a < 45 ; a++) {
		glbs->timer = 25;
		for(j = 0 ; j < 50 ; j++) {
			memset(glbs->light_buffer + 60 * j, fade_table[f * 4], 20);
			memset(glbs->dark_buffer + 60 * j, fade_table[f * 4 + 2], 20);
			memset(glbs->light_buffer + 60 * j + 30, fade_table[f * 4 + 1], 20);
			memset(glbs->dark_buffer + 60 * j + 30, fade_table[f * 4 + 3], 20);
		}
		if(f < 6) GraySpriteClipX8_MASK(64 + hdr[i].x_offset, 26 + hdr[i].y_offset, hdr[i].hieght,
			gfx + hdr[i].offset, hdr[i].width, glbs->player.flash);
		update_screen();
		if(a < 24 && a % 2) glbs->player.flash = !glbs->player.flash;
		else if(a >= 30 && a < 42 && a % 2) i++;
		if(a >= 38) f++;

		while(glbs->timer) PLATFORM_YIELD();
	}

	free(buffer);
}

void player_shoot()
{
	short type = 6;
	short x = glbs->player.e.x;
	short y = glbs->player.e.y;
	char direction = 0;

	if((glbs->player.shot_delay && glbs->player.charge_level == 0)) return;

	if(glbs->player.action_type & TYPE_BALL) {
		if(!player_item_equiped(BOMBS)) return;

		if(glbs->player.weapon_selected == POWERBOMB) {
			if(bombs_placed) return;
			bar_update();
			type = POWER_BOMB;
			glbs->player.ammunition[2]--;
			if(glbs->player.ammunition[2] == 0) glbs->player.weapon_selected = BEAM;
		} else type = NORMAL_BOMB;

		bomb_create(x + glbs->player.e.x_off + glbs->player.e.width / 2,
			y + glbs->player.e.y_off + glbs->player.e.hieght / 2, type);

		glbs->player.shot_delay = -10;
		return;
	}

	if(glbs->player.weapon_selected == MISSILE) {
		bar_update();
		type = SHOT_MISSILE,
		glbs->player.shot_delay = -20;
		glbs->player.ammunition[0]--;
		if(glbs->player.ammunition[0] == 0) glbs->player.weapon_selected = BEAM;
	} else if(glbs->player.weapon_selected == SUPERMISSILE) {
		bar_update();
		type = SHOT_SUPERMISSLE;
		glbs->player.shot_delay = -30;
		glbs->player.ammunition[1]--;
		if(glbs->player.ammunition[1] == 0) glbs->player.weapon_selected = BEAM;
	} else if(glbs->player.weapon_selected == HOMING_MISSILE) {
		bar_update();
		type = SHOT_HOMING_MISSILE;
		glbs->player.shot_delay = -30;
		glbs->player.ammunition[0] -= 3;
		if(glbs->player.ammunition[0] < 3) glbs->player.weapon_selected = BEAM;
	} else if(glbs->player.weapon_selected == ENERGY_LEACH) {
		if(glbs->player.leach_type == LEACH_NONE) {
			bar_update();
			type = SHOT_ENERGY_LEACH;
			glbs->player.shot_delay = -30;
			glbs->player.ammunition[1]--;
			if(glbs->player.ammunition[1] == 0) glbs->player.weapon_selected = BEAM;
		} else {
			glbs->player.leach_type = LEACH_NONE;
			glbs->player.shot_delay = -1;
			return;
		}
	} else if(glbs->player.charge_level == 0 || glbs->player.charge_level == 4) {
		if(player_item_equiped(PLASMA_BEAM)) type = SHOT_PLASMA;
		else if(player_item_equiped(SPAZER_BEAM)) type = SHOT_SPAZER;
		else type = SHOT_NORMAL;
		if(player_item_equiped(WAVE_BEAM)) type++;
		if(player_item_equiped(ICE_BEAM)) type += 2;
		if(glbs->player.charge_level == 4) type += 12;
		glbs->player.shot_delay = 20;
	}

	if(glbs->player.gun_position == GUN_FOWARD || glbs->player.gun_position == GUN_NONE) {
		y += 27;
		x += 3;
		if(glbs->player.direction == RIGHT) x += 25;
		if(glbs->player.action == ACT_RUN) y -= 3;
		if(glbs->player.action == ACT_DUCK) y += 9;
		direction = glbs->player.direction;
	} else if(glbs->player.gun_position == GUN_UP){
		y += 4;
		x += 16;
		if(glbs->player.direction == RIGHT) x++;
		if(glbs->player.action == ACT_DUCK) y += 9;
		direction = UP;
	} else if(glbs->player.gun_position == GUN_DOWN) {
		y += 38;
		x += 10;
		if(glbs->player.direction == RIGHT) x += 12;
		direction = DOWN;
	} else if(glbs->player.gun_position == GUN_DIAG_UP) {
		y += 10;
		x += 2;
		if(glbs->player.direction == RIGHT) x += 29;
		if(glbs->player.action == ACT_DUCK) y += 9;
		direction = glbs->player.direction | UP;
	} else if(glbs->player.gun_position == GUN_DIAG_DOWN) {
		y += 33;
		x += 2;
		if(glbs->player.direction == RIGHT) x += 29;
		if(glbs->player.action == ACT_DUCK) y += 9;
		direction = glbs->player.direction | DOWN;
	}

	glbs->player.gun_x = x;
	glbs->player.gun_y = y;

	if(glbs->player.charge_level == 4 && !(glbs->player.action_type & TYPE_FLIP)) {
		shot_create(TRUE, TRUE, x, y, direction, type);
		glbs->player.charge_level = 0;
		return;
	} else if(glbs->player.charge_level > 0) {
		if(glbs->player.charge_level != 3) glbs->player.charge_counter--;
		if(glbs->player.charge_counter == 0) {
			glbs->player.charge_level++;
			glbs->player.charge_counter = CHARGE_TIME;
		}
		return;
	} else if(player_item_equiped(CHARGE_BEAM) &&
		glbs->player.weapon_selected == BEAM && !(glbs->player.action_type & TYPE_FLIP)) {
			glbs->player.charge_level = 1;
			glbs->player.charge_frame = 0;
			glbs->player.charge_frame_counter = 0;
			glbs->player.charge_counter = CHARGE_DELAY;
	}

	if(!(glbs->player.action_type & TYPE_FLIP)) shot_create(TRUE, TRUE, x, y, direction, type);
}

/*char player_fall_through()
{
	short x;
	short y = glbs->player.e.tile_y1 + 1;
	char counter = 0;
	unsigned char data;
	char ok = TRUE;


	if(glbs->player.e.tile_yoff1 != 11) return FALSE;

	for(x = glbs->player.e.tile_x0 ; x <= glbs->player.e.tile_x1 ; x++) {
		data = glbs->current_map.prop_data[y * glbs->current_map.width + x];
		if((data & 0x7e) >= SOLID && data != (SOLID | 0x80)) {
			ok = FALSE;
			break;
		} else if(data == (SOLID | 0x80)) counter++;
	}

	if(!ok || counter == 0) return FALSE;

	glbs->player.e.tile_y1++;
	glbs->player.e.tile_yoff1 = 0;

	glbs->player.e.tile_yoff0++;
	if(glbs->player.e.tile_yoff0 == 12) {
	 glbs->player.e.tile_yoff0 = 0;
	 glbs->player.e.tile_y0++;
	}

	glbs->player.e.y++;

	return TRUE;
}*/

void player_set_action(short action)
{
	if(action == NONE) {
		glbs->player.input_disabled = player_action[glbs->player.action].input_disabled;
		return;
	}

	if(player_action[action].overide_action){
		glbs->player.overide_action = action;
		glbs->player.overide_frame = 0;
		glbs->player.overide_frame_counter = 0;
		glbs->player.input_disabled = player_action[action].input_disabled;
		return;
	}

	glbs->player.action_duration = player_action[action].duration;

	if(action == glbs->player.action) return;

	if(action == ACT_FLIP_LAND && !entity_check_size(&(glbs->player.e), &player_size[SIZE_STAND]))
		action = ACT_DUCK;
	if(action == ACT_TURBO_CHARGE_END) entity_reset_slope(&(glbs->player.e)); //end of turbo charge

	//released first press
	if(glbs->player.action == ACT_RUN && action == ACT_STAND && glbs->player.sprint_state == 1) {
		glbs->player.sprint_state = 2;
		glbs->player.sprint_counter = SPRINT_DELAY;
	}

	if(!(glbs->player.action_type & TYPE_JUMP_UP) && (player_action[action].type & TYPE_JUMP_DOWN))
		glbs->player.jump_speed = (FALL_SPEED_START >> glbs->player.in_water);

	entity_set_size(&(glbs->player.e), &player_size[player_action[action].size]);
	glbs->player.action = action;
	glbs->player.input_disabled = player_action[action].input_disabled;
	glbs->player.frame = 0;
	glbs->player.frame_counter = 0;
	glbs->player.action_type = player_action[action].type;
	if(action != ACT_RUBBER_BALL_CHARGE)
		glbs->player.fall_distance = 0; //save fall distance if rubber ball is charging

	if(glbs->player.action_type & TYPE_BALL) {
		glbs->player.charge_level = glbs->player.charge_counter = 0;
	}
}

void player_process()
{
	short last_position = glbs->player.gun_position;
	char old_weapon = glbs->player.weapon_selected;
	short k;
	short x, y;
	//char ok;
	char up, down, diag_up, diag_down;

	if(glbs->ti92) k = player_get_input_92();
	else k = player_get_input_89();

	//adjust grayscale
	if((k & PLUS_PRESSED) && !plus_down && glbs->gray_adjust < 10) {
		glbs->gray_adjust++;
		GrayAdjust(glbs->gray_adjust);
		plus_down = TRUE;
	} else if(!(k & PLUS_PRESSED)) plus_down = FALSE;

	if((k & MINUS_PRESSED) && !minus_down && glbs->gray_adjust > -10) {
		glbs->gray_adjust--;
		GrayAdjust(glbs->gray_adjust);
		minus_down = TRUE;
	} else if(!(k & MINUS_PRESSED)) minus_down = FALSE;

	if(k & MENU_PRESSED) status_screen();

	//check for water stuff
	y = glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght;

	if(y <= glbs->water.top && glbs->player.hit_water) {
		glbs->player.hit_water = FALSE;
		if(!player_item_equiped(GRAVITY_SUIT)) {
			glbs->player.in_water = FALSE;
			glbs->player.jump_hieght = 12;
			glbs->player.jump_speed = 88;
		}
		if(glbs->water.type == WATER_NORMAL)
			animation_create(glbs->player.e.x + glbs->player.e.x_off + (glbs->player.e.width >> 1),
			glbs->water.top - 1, SPLASH, NONE);
	}

	if(glbs->water.type != WATER_NONE) {
		if(y > glbs->water.top && !glbs->player.hit_water) {
			glbs->player.hit_water = TRUE;
			if(!player_item_equiped(GRAVITY_SUIT)) {
				glbs->player.in_water = TRUE;
				glbs->player.walk_speed = (WALK_SPEED_START >> 1);
				glbs->player.sprint_state = 0;
				glbs->player.jump_speed = 75;
			}
			if(glbs->water.type == WATER_NORMAL)
				animation_create(glbs->player.e.x + glbs->player.e.x_off + (glbs->player.e.width >> 1),
				glbs->water.top - 1, SPLASH, NONE);
		}
	}

	//dangerous water
	if(y > glbs->water.top && glbs->player.hp_drain_counter <= 0) {
		short damage = 0;
		if(glbs->water.type == WATER_LAVA) damage = 8;
		if(glbs->water.type == WATER_ACID) damage = 16;
		if(player_item_equiped(VARIA_SUIT)) damage = damage >> 1;
		if(player_item_equiped(GRAVITY_SUIT)) damage = damage >> 1;
		if(damage == 2) damage = 0;
		if(damage != 0) {
			glbs->player.hp -= damage;
			glbs->player.hp_drain = TRUE;
			glbs->player.flash = TRUE;
			bar_update();
		}
		glbs->player.hp_drain_counter = HP_DRAIN_DELAY;
	} else {
		if(y <= glbs->water.top) {
			glbs->player.hp_drain_counter = 0;
			glbs->player.hp_drain = FALSE;
		}
		else if(glbs->player.hp_drain_counter > 0) glbs->player.hp_drain_counter--;
	}

	//energy leach
	if(glbs->player.leach_type == LEACH_ENEMY) {
		if(glbs->player.leach_counter <= 0) {
			bar_update();
			if(glbs->player.ammunition[1] > 0) {
				enemy_hit(glbs->player.leach_target, 10, FALSE, DAMAGE_SUPERMISSILE, get_combo_id());
				glbs->player.hp += 10;
				if(glbs->player.hp > glbs->player.hp_max) glbs->player.hp = glbs->player.hp_max;
				glbs->player.ammunition[1]--;
				if(enemies[glbs->player.leach_target].hp <= 0) glbs->player.leach_type = LEACH_NONE;
				glbs->player.leach_counter = 10;
			} else {
				glbs->player.leach_type = LEACH_NONE;
				if(glbs->player.weapon_selected == SUPERMISSILE || glbs->player.weapon_selected == ENERGY_LEACH)
					glbs->player.weapon_selected = BEAM;
			}
		} else glbs->player.leach_counter--;
	}

	//gun position
	if(!(glbs->player.action_type & TYPE_JUMP_UP) && !(glbs->player.action_type & TYPE_JUMP_DOWN))
		glbs->player.gun_position = GUN_NONE;
	else if(glbs->player.gun_position != GUN_DOWN) // && glbs->player.gun_position != GUN_FOWARD)
		glbs->player.gun_position = GUN_NONE;

	if(!(glbs->player.action_type & TYPE_BALL)) {
		if(glbs->player.e.gravity == DOWN) {
			up = k & UP_PRESSED;
			down = k & DOWN_PRESSED;
			diag_up = k & DIAG_UP_PRESSED;
			diag_down = k & DIAG_DOWN_PRESSED;
		} else {
			up = k & DOWN_PRESSED;
			down = k & UP_PRESSED;
			diag_up = k & DIAG_DOWN_PRESSED;
			diag_down = k & DIAG_UP_PRESSED;
		}

		if(up && !(glbs->player.action == ACT_DUCK) && glbs->player.action != ACT_RUN)
			glbs->player.gun_position = GUN_UP;

		if(down && ((glbs->player.action_type & TYPE_JUMP_UP) ||
			(glbs->player.action_type & TYPE_JUMP_DOWN))) {
				glbs->player.gun_position = GUN_DOWN;
		}

		if(diag_up && diag_down && glbs->player.action != ACT_RUN)
			glbs->player.gun_position = GUN_UP;
		else if(diag_up)
			glbs->player.gun_position = GUN_DIAG_UP;
		else if(diag_down)
			glbs->player.gun_position = GUN_DIAG_DOWN;

		if((k & LEFT_PRESSED) || (k & RIGHT_PRESSED)) {
			if(up) glbs->player.gun_position = GUN_DIAG_UP;
			if(down) glbs->player.gun_position = GUN_DIAG_DOWN;
			if((glbs->player.action == ACT_JUMP_UP || glbs->player.action == ACT_JUMP_DOWN) &&
				(glbs->player.gun_position == GUN_NONE || glbs->player.gun_position == GUN_DOWN))
					glbs->player.gun_position = GUN_FOWARD;
		}

		if(glbs->player.gun_position == GUN_NONE && (k & SHOOT_PRESSED))
			glbs->player.gun_position = GUN_FOWARD;

		if((glbs->player.action_type & TYPE_FLIP) && !glbs->player.flip_start && ((k & SHOOT_PRESSED) ||
			(glbs->player.gun_position != GUN_NONE && glbs->player.gun_position != GUN_FOWARD))) {
				if(glbs->player.action_type & TYPE_JUMP_UP) player_set_action(ACT_JUMP_UP);
				if(glbs->player.action_type & TYPE_JUMP_DOWN) player_set_action(ACT_JUMP_DOWN);
		} else if((glbs->player.action_type & TYPE_FLIP) && glbs->player.flip_start && glbs->player.gun_position == GUN_NONE)
			glbs->player.flip_start = FALSE;

		if(glbs->player.gun_position == GUN_DOWN && last_position != GUN_DOWN && !glbs->player.down_pressed)
			glbs->player.down_pressed = TRUE;

		if(glbs->player.gun_position != last_position && player_action[glbs->player.action].frame_reset){
			glbs->player.frame = 0;
			glbs->player.frame_counter = 0;
		}
	}

	if((k & UP_PRESSED) || glbs->player.gun_position == GUN_DIAG_UP) {
		if(glbs->player.look_counter == 0 && glbs->player.y_thresh == glbs->player.y_thresh_base) glbs->player.look_counter = 1;
	} else if(glbs->player.look_counter > 0) glbs->player.look_counter = 0;

	if((k & DOWN_PRESSED) || glbs->player.gun_position == GUN_DIAG_DOWN) {
		if(glbs->player.look_counter == 0 && glbs->player.y_thresh == glbs->player.y_thresh_base) glbs->player.look_counter = -1;
	} else if(glbs->player.look_counter < 0) glbs->player.look_counter = 0;

	if(k & BEAM_SELECT_PRESSED) {
		if(beam_select_wait == 0) {
			glbs->player.weapon_selected = BEAM;
			beam_select_wait = 1;
		}
	} else
		beam_select_wait = 0;

	if((k & MISSILE_SELECT_PRESSED)) {
		if(missile_select_wait == 0) {
			if(glbs->player.weapon_selected != MISSILE && glbs->player.ammunition[0] != 0)
				glbs->player.weapon_selected = MISSILE;
			else if(glbs->player.weapon_selected == MISSILE && glbs->player.ammunition[0] > 2 &&
				player_item_found(HOMING_MISSILE))
				glbs->player.weapon_selected = HOMING_MISSILE;
			missile_select_wait = 1;
			glbs->player.charge_level = 0;
		}
	} else
		missile_select_wait = 0;

	if((k & SUPERMISSILE_SELECT_PRESSED)) {
		if(supermissile_select_wait == 0 && glbs->player.ammunition[1] != 0) {
			if(glbs->player.weapon_selected != SUPERMISSILE && glbs->player.ammunition[1] != 0)
				glbs->player.weapon_selected = SUPERMISSILE;
			else if(glbs->player.weapon_selected == SUPERMISSILE && glbs->player.ammunition[1] != 0 &&
				player_item_found(ENERGY_LEACH))
				glbs->player.weapon_selected = ENERGY_LEACH;
			supermissile_select_wait = 1;
			glbs->player.charge_level = 0;
		}
	} else
		supermissile_select_wait = 0;

	if((k & POWERBOMB_SELECT_PRESSED)) {
		if(powerbomb_select_wait == 0 && glbs->player.ammunition[2] != 0) {
			if(glbs->player.weapon_selected != POWERBOMB) glbs->player.weapon_selected = POWERBOMB;
			else glbs->player.weapon_selected = BEAM;
			powerbomb_select_wait = 1;
			glbs->player.charge_level = 0;
		}
	} else
		powerbomb_select_wait = 0;

	if((k & REVERSE_G_SELECT_PRESSED) && player_item_found(REVERSE_GRAVITY)) {
		if(reverse_g_select_wait == 0 && !glbs->player.gravity_cooldown) {
			x = glbs->player.e.x + glbs->player.e.x_off;
			y = glbs->player.e.y + glbs->player.e.y_off;
			if(glbs->player.e.gravity == DOWN) {
				entity_set_gravity(&(glbs->player.e), UP);
				glbs->player.gravity_counter = 1024;
			} else {
				entity_set_gravity(&(glbs->player.e), DOWN);
				glbs->player.gravity_cooldown = TRUE;
			}
			x -= glbs->player.e.x_off;
			y -= glbs->player.e.y_off;
			player_set_position(x, y);
			reverse_g_select_wait = 1;
		}
	} else
		reverse_g_select_wait = 0;

	if(glbs->player.gravity_cooldown) {
		glbs->player.gravity_counter++;
		if(glbs->player.gravity_counter > 1024) {
			glbs->player.gravity_counter = 0;
			glbs->player.gravity_cooldown = FALSE;
		}
	} else if(glbs->player.gravity_counter > 0) {
		glbs->player.gravity_counter--;
		if(glbs->player.gravity_counter == 1) {
			entity_set_gravity(&(glbs->player.e), DOWN);
			glbs->player.gravity_cooldown = TRUE;
		}
	}

	if(old_weapon != glbs->player.weapon_selected) bar_update();

	if(glbs->player.shot_delay > 0) glbs->player.shot_delay--;
	else if(glbs->player.shot_delay != -1) glbs->player.shot_delay++; //if negative, climb up till -1 and stop

	if(glbs->player.sprint_counter) {
		glbs->player.sprint_counter--;
		if(glbs->player.sprint_counter == 0 && glbs->player.sprint_state != 3) glbs->player.sprint_state = 0;
	}

	if(glbs->player.sprint_state == 3) {
		if(!(k & LEFT_PRESSED) && glbs->player.sprint_direction == LEFT) glbs->player.sprint_state = 4;
		if(!(k & RIGHT_PRESSED) && glbs->player.sprint_direction == RIGHT) glbs->player.sprint_state = 4;
		if((k & LEFT_PRESSED) && glbs->player.sprint_direction == RIGHT) glbs->player.sprint_state = 4;
		if((k & RIGHT_PRESSED) && glbs->player.sprint_direction == LEFT) glbs->player.sprint_state = 4;
		if(glbs->player.sprint_state == 4) {
			if(glbs->player.walk_speed >= TURBO_SPEED_MAX) glbs->player.turbo_counter = 150;
			glbs->player.sprint_direction = NONE;
		}
	} else
		if(glbs->player.turbo_counter) glbs->player.turbo_counter--;

	if(player_item_equiped(SPEED_BOOTS) &&
		((glbs->player.walk_speed >= TURBO_SPEED_MAX && glbs->player.sprint_state == 3) ||
		(glbs->player.action_type & TYPE_TURBO))) {

			glbs->player.e.water_walking = TRUE;

			glbs->player.blur_counter--;
			if(glbs->player.blur_counter <= 0) {
				blur_create(glbs->player.e.x, glbs->player.e.y, glbs->player.e.sprite, player_gfx);
				if(glbs->player.action == ACT_RUN) {
					if(glbs->player.gun_position == GUN_NONE) glbs->player.e.sprite -= 10;
					if(glbs->player.gun_position == GUN_FOWARD) glbs->player.e.sprite -= 20;
					if(glbs->player.gun_position == GUN_DIAG_UP) glbs->player.e.sprite -= 40;
					if(glbs->player.gun_position == GUN_DIAG_DOWN) glbs->player.e.sprite -= 30;
					blur_create(glbs->player.e.x, glbs->player.e.y, glbs->player.e.sprite, player_gfx);
					if(glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght == glbs->water.top)
						animation_create(glbs->player.e.x + glbs->player.e.x_off + (glbs->player.e.width >> 1), glbs->water.top - 1, SPLASH, NONE);
				}
				glbs->player.blur_counter = BLUR_DELAY;
			}
	} else glbs->player.e.water_walking = FALSE;

	if(glbs->player.sprint_state == 4) {
		if(glbs->player.action == ACT_RUN) {
			if(!player_move(glbs->player.direction, glbs->player.walk_speed)) {
				player_next_action();
				glbs->player.sprint_state = 0;
			} else player_continue_action();
		} else glbs->player.sprint_state = 0; //dont carry momentum unless running
		glbs->player.walk_speed -= WALK_SPEED_DECREASE;
		if(glbs->player.walk_speed <= 100) glbs->player.sprint_state = 0;
	}

	if(glbs->player.action_type & TYPE_TURBO) {
		if(glbs->player.action == ACT_TURBO_JUMP) {
			if(!player_move(UP, 400)) player_next_action();
			else player_continue_action();
		} else if(glbs->player.action == ACT_TURBO_CHARGE) {
			if(!player_move(glbs->player.bounce_direction, 400)) player_next_action();
			else player_continue_action();
		} else if(glbs->player.action == ACT_TURBO_BALL_BOUNCE) {
			glbs->player.rubber_charge -= 5;
			if(glbs->player.rubber_charge < 0 || !player_move(UP, 400)) player_next_action();
			else player_continue_action();
		}
	}

	if(glbs->player.jumped && !(k & JUMP_PRESSED)) glbs->player.jumped = FALSE;

	if(glbs->player.action_type & TYPE_JUMP_UP) {
		if(glbs->player.jump_hieght < (34 >> glbs->player.in_water)) {
			if(glbs->player.in_water) glbs->player.jump_speed--;
			else glbs->player.jump_speed -= 3;
		}
		if(glbs->player.jump_hieght <= 0) player_next_action();
	} else if(!(glbs->player.action_type & TYPE_TURBO) && !glbs->player.input_disabled) {
		if(!(glbs->player.action_type & TYPE_JUMP_DOWN) && entity_check_direction(&(glbs->player.e), DOWN)) {
			if(!(glbs->player.action_type & TYPE_BALL))
				player_set_action(ACT_FALL);
			else
				player_set_action(ACT_BALL_JUMP_DOWN);
		} else if((glbs->player.action_type & TYPE_JUMP_DOWN) && player_move(DOWN, glbs->player.jump_speed)) {
			player_continue_action();
			if(glbs->player.jump_speed < (FALL_SPEED_MAX >> glbs->player.in_water)) {
				if(glbs->player.in_water) glbs->player.jump_speed++;
				else glbs->player.jump_speed += FALL_SPEED_INCREASE;
			}
		} else if(glbs->player.action == ACT_BALL_JUMP_DOWN && glbs->player.fall_distance > 24) {
			if(player_item_equiped(RUBBER_BALL) && (k & DOWN_PRESSED) &&
			glbs->player.fall_distance > 48 && !glbs->player.in_water) {
				glbs->player.rubber_charge = 0;
				player_set_action(ACT_RUBBER_BALL_CHARGE);
			}
			else player_bounce(6, UP);
		}
	}

	//Code to go from rubber ball charging to rubber ball bouncing
	if(glbs->player.action == ACT_RUBBER_BALL_CHARGE) {
		if((k & DOWN_PRESSED) && glbs->player.rubber_charge <= glbs->player.fall_distance) {
			glbs->player.rubber_charge += 2;
			player_continue_action();
		} else
			player_set_action(ACT_TURBO_BALL_BOUNCE);
	}

	if((glbs->player.action_type & TYPE_BOUNCE) && player_move(glbs->player.bounce_direction, glbs->player.jump_speed))
		player_continue_action();

	if(glbs->player.invinc_duration) glbs->player.invinc_duration--;
	if(glbs->player.flash && glbs->player.invinc_duration == 0 && glbs->player.turbo_counter == 0 &&
		glbs->player.charge_level != 3 && !(glbs->player.e.damage_type & DAMAGE_SCREW) &&
		glbs->player.action != ACT_SIDE_WAIT && glbs->player.hp_drain == FALSE)
			glbs->player.flash = FALSE;

	if(glbs->player.direction == glbs->player.wall_kick_direction) {
		player_set_action(ACT_WALL_KICK_READY);
		glbs->player.wall_kick_direction = NONE;
	}

	if(glbs->player.action_type & TYPE_FLIP) {
		if(glbs->player.walk_speed < WALK_SPEED_MAX - (glbs->player.in_water << 5)) {
			if(glbs->player.in_water) glbs->player.walk_speed++;
			else glbs->player.walk_speed += WALK_SPEED_INCREASE;
		}
		if(!player_move(glbs->player.direction, glbs->player.walk_speed)) {
			if(glbs->player.direction == LEFT) glbs->player.wall_kick_direction = RIGHT;
			if(glbs->player.direction == RIGHT) glbs->player.wall_kick_direction = LEFT;
		} else if(glbs->player.wall_kick_direction != NONE)
			glbs->player.wall_kick_direction = NONE;
	} else if(glbs->player.wall_kick_direction != NONE)
		glbs->player.wall_kick_direction = NONE;

	if(glbs->player.action_duration == 0)
		player_set_action(player_action[glbs->player.action].next_action);
	else
		glbs->player.action_duration--;


	if(glbs->player.look_counter > 0) {
		if(glbs->player.look_counter < 60) {
			glbs->player.look_counter++;
			if(glbs->player.look_counter > 30) {
				glbs->player.y_thresh++;
				camera_process_up(glbs->player.y_thresh, 1);
			}
		}
	} else if(glbs->player.look_counter < 0) {
		if(glbs->player.look_counter > -60) {
			glbs->player.look_counter--;
			if(glbs->player.look_counter < -30) {
				glbs->player.y_thresh--;
				camera_process_down(glbs->player.y_thresh, 1);
			}
		}
	} else if(glbs->player.y_thresh != glbs->player.y_thresh_base) {
		if(glbs->player.y_thresh > glbs->player.y_thresh_base) {
			glbs->player.y_thresh--;
			if(glbs->player.y_thresh > glbs->player.y_thresh_base) glbs->player.y_thresh--;
			camera_process_down(glbs->player.y_thresh, 2);
		} else {
			glbs->player.y_thresh++;
			if(glbs->player.y_thresh < glbs->player.y_thresh_base) glbs->player.y_thresh++;
			camera_process_up(glbs->player.y_thresh, 2);
		}
	}

	player_animate();
	check_items();

	if(glbs->player.action == ACT_FRONT_READY) {
		if(k & LEFT_PRESSED) {
			glbs->player.direction = LEFT;
			player_set_action(ACT_STAND);
		} else if(k & RIGHT_PRESSED) {
			glbs->player.direction = RIGHT;
			player_set_action(ACT_STAND);
		}
	} else if(glbs->player.action == ACT_TURBO_JUMP_START) {
		if(k & LEFT_PRESSED) {
			glbs->player.direction = glbs->player.bounce_direction = LEFT;
			if(glbs->player.gun_position == GUN_UP || glbs->player.gun_position == GUN_DIAG_UP)
				glbs->player.bounce_direction = glbs->player.direction | UP;
			else if(glbs->player.gun_position == GUN_DOWN || glbs->player.gun_position == GUN_DIAG_DOWN)
				glbs->player.bounce_direction = glbs->player.direction | DOWN;
			//else
			//	glbs->player.bounce_direction = glbs->player.direction;
			glbs->player.e.slope_right_up = NONE;
			player_set_action(ACT_TURBO_CHARGE);
		} else if(k & RIGHT_PRESSED) {
			glbs->player.direction = glbs->player.bounce_direction = RIGHT;
			if(glbs->player.gun_position == GUN_UP || glbs->player.gun_position == GUN_DIAG_UP)
				glbs->player.bounce_direction = glbs->player.direction | UP;
			else if(glbs->player.gun_position == GUN_DOWN || glbs->player.gun_position == GUN_DIAG_DOWN)
				glbs->player.bounce_direction = glbs->player.direction | DOWN;
			//else
			//	glbs->player.bounce_direction = glbs->player.direction;
			glbs->player.e.slope_left_up = NONE;
			player_set_action(ACT_TURBO_CHARGE);
		}
	}

	if(glbs->player.sprint_direction != glbs->player.direction &&
		glbs->player.walk_speed > WALK_SPEED_MAX) glbs->player.walk_speed = WALK_SPEED_MAX;

	glbs->player.e.damage_type = DAMAGE_STAND;

	if((glbs->player.sprint_state == 3 && glbs->player.walk_speed >= TURBO_SPEED_MAX) ||
		(glbs->player.action_type & TYPE_TURBO)) glbs->player.e.damage_type |= DAMAGE_SPEED;
	if((glbs->player.action_type & TYPE_FLIP) && player_item_equiped(SCREW_ATTACK))
		glbs->player.e.damage_type |= DAMAGE_SCREW;

	if(glbs->player.up_pressed && ((!(k & UP_PRESSED) && glbs->player.e.gravity == DOWN) ||
		(!(k & DOWN_PRESSED) && glbs->player.e.gravity == UP))) glbs->player.up_pressed = FALSE;
	if(glbs->player.down_pressed && ((!(k & DOWN_PRESSED) && glbs->player.e.gravity == DOWN) ||
		(!(k & UP_PRESSED) && glbs->player.e.gravity == UP))) glbs->player.down_pressed = FALSE;

	if(!glbs->player.input_disabled) {
		if(k & LEFT_PRESSED) player_step(LEFT);
		else if(k & RIGHT_PRESSED) player_step(RIGHT);
		if(k & UP_PRESSED) {
			if(glbs->player.e.gravity == DOWN) player_step(UP);
			else player_step(DOWN);
		} else if(k & DOWN_PRESSED) player_step(glbs->player.e.gravity);

		if(k & SHOOT_PRESSED)
			player_shoot();
		else if(!(glbs->player.action_type & TYPE_FLIP)) {
			if(glbs->player.charge_level > 1 ) {
				if(glbs->player.charge_level == 3) glbs->player.charge_level++;
				else glbs->player.charge_level = 0;
				glbs->player.shot_delay = 0;
				player_shoot();
			}
			if(glbs->player.shot_delay < 0) glbs->player.shot_delay *= -1;
			glbs->player.charge_level = glbs->player.charge_counter = 0;
		}

		if(k & JUMP_PRESSED) {
			//if(k & DOWN_PRESSED) ok = player_fall_through();
			//else ok = FALSE;
			//if(!ok)
			player_jump();
		}
	}
}

void player_animate()
{
	ENTITY_ANIMATION *animation;

	//if(player_action[glbs->player.action].animation[glbs->player.gun_position]->frames[glbs->player.frame * 2 + 1] == 0) return;

	if(glbs->player.overide_action != NONE){
		animation = ((ENTITY_ANIMATION *)(glbs->metpack_base + player_action[glbs->player.overide_action].animation[0]));

		glbs->player.overide_frame_counter++;
		if(glbs->player.overide_frame_counter == animation->frames[glbs->player.overide_frame * 2 + 1]) {

				glbs->player.overide_frame++;
				if(glbs->player.overide_frame == animation->frame_number) glbs->player.overide_action = NONE;

			glbs->player.overide_frame_counter = 0;
		}
	}

	animation = ((ENTITY_ANIMATION *)(glbs->metpack_base + player_action[glbs->player.action].animation[glbs->player.gun_position]));

	if(glbs->player.action == ACT_RUN) glbs->player.frame_counter += glbs->player.walk_speed;
	else if(!glbs->player.in_water) glbs->player.frame_counter += 100;
	else glbs->player.frame_counter += 50;

	if(glbs->player.frame_counter >=
		animation->frames[glbs->player.frame * 2 + 1] * 100) {
			glbs->player.frame++;
			if(glbs->player.frame == animation->frame_number) glbs->player.frame = animation->repeat_frame;

			glbs->player.frame_counter = 0;
	}

	if(glbs->player.charge_level > 1) {
		glbs->player.charge_frame_counter++;
		if(glbs->player.charge_frame_counter == CHARGE_ANIM_SPEED) {
			glbs->player.charge_frame_counter = 0;
			glbs->player.charge_frame++;
			if(glbs->player.charge_level == 2 && glbs->player.charge_frame == 4) glbs->player.charge_frame = 0;
			else if(glbs->player.charge_level == 3 && glbs->player.charge_frame == 5) glbs->player.charge_frame = 3;
		}
	}
}

#define LINK_COUNT 7
void draw_link()
{
	ENTITY *e = &glbs->player.e;
	short tx, ty;
	short x = e->x + e->x_off + e->width / 2, y = e->y + e->y_off + e->hieght / 2;
	short dx, dy;
	short t = glbs->player.leach_target;
	short i;

	if(glbs->player.leach_type == LEACH_SHOT) {
		tx = glbs->shots[t].x + glbs->shot_data[glbs->shots[t].type].width / 2;
		ty = glbs->shots[t].y + glbs->shot_data[glbs->shots[t].type].hieght / 2;
	} else {
		e = &enemies[t].e;
		tx = e->x + e->x_off + e->width / 2;
		ty = e->y + e->y_off + e->hieght / 2;
	}

	dx = (tx - x) / (LINK_COUNT - 1);
	dy = (ty - y) / (LINK_COUNT - 1);

	for(i = 0 ; i < LINK_COUNT - 1 ; i++, x += dx, y += dy)
		GraySpriteClip8_MASK(x - glbs->camera.x, y - glbs->camera.y, 6, leach_link,
			glbs->light_buffer, glbs->dark_buffer);

	GraySpriteClip8_MASK(tx - glbs->camera.x, ty - glbs->camera.y, 6, leach_link,
		glbs->light_buffer, glbs->dark_buffer);
}

void player_draw()
{
	ENTITY_ANIMATION *animation;

	if(glbs->player.overide_action != NONE)
		animation = ((ENTITY_ANIMATION *)(glbs->metpack_base + player_action[glbs->player.overide_action].animation[0]));
	else
		animation = ((ENTITY_ANIMATION *)(glbs->metpack_base + player_action[glbs->player.action].animation[glbs->player.gun_position]));

	if(glbs->player.action == ACT_RUN) {
		glbs->player.e.sprite =
			player_header + 10 + animation->frames[glbs->player.frame * 2];
		if(glbs->player.direction == RIGHT) glbs->player.e.sprite += 108;
		entity_draw(&(glbs->player.e), player_gfx, glbs->player.flash, TRUE, FALSE);

		if(glbs->player.gun_position == GUN_NONE) glbs->player.e.sprite += 10;
		if(glbs->player.gun_position == GUN_FOWARD) glbs->player.e.sprite += 20;
		if(glbs->player.gun_position == GUN_DIAG_UP) glbs->player.e.sprite += 40;
		if(glbs->player.gun_position == GUN_DIAG_DOWN) glbs->player.e.sprite += 30;

		entity_draw(&(glbs->player.e), player_gfx, glbs->player.flash, TRUE, FALSE);
	} else {

		if(glbs->player.overide_action != NONE) {
			glbs->player.e.sprite =
				player_header + animation->frames[glbs->player.overide_frame * 2];
			if(glbs->player.direction == RIGHT && player_action[glbs->player.overide_action].change_direction)
				glbs->player.e.sprite += 108;
			if(player_action[glbs->player.overide_action].change_direction) glbs->player.e.sprite += 10;
		} else {
			glbs->player.e.sprite =
				player_header + animation->frames[glbs->player.frame * 2];
			if(player_action[glbs->player.action].change_direction) glbs->player.e.sprite += 10;
			if(glbs->player.direction == RIGHT && player_action[glbs->player.action].change_direction)
				glbs->player.e.sprite += 108;
		}
		entity_draw(&(glbs->player.e), player_gfx, glbs->player.flash, TRUE, FALSE);
	}

	//e = &(glbs->player.e);

	//InvertGrayRect2B(
	//	(e->x + e->x_off) + x_buffer - glbs->camera.x,
	//	(e->y + e->y_off) + y_buffer + 12 - glbs->camera.y,
	//	(e->x + e->x_off + e->width) + x_buffer - glbs->camera.x,
	//	(e->y + e->y_off + e->hieght) + 12 + y_buffer - glbs->camera.y, glbs->light_buffer, glbs->dark_buffer);

	if((glbs->player.invinc_duration && glbs->player.action != ACT_HURT)) {
		if(glbs->player.flash) glbs->player.flash = 0;
		else glbs->player.flash = 1;
	} else if(glbs->player.hp_drain) {
		if(glbs->player.hp_drain_counter < HP_DRAIN_DELAY - 1) glbs->player.flash = FALSE;
	} else if(glbs->player.turbo_counter != 0 || glbs->player.charge_level == 3 ||
		(glbs->player.e.damage_type & DAMAGE_SCREW) || glbs->player.action == ACT_SIDE_WAIT) {
			if(glbs->player.flash) glbs->player.flash = 0;
			else glbs->player.flash = 2;
	}

	if(glbs->player.charge_level > 1 && !(glbs->player.action_type & TYPE_FLIP)) {
		GraySpriteClipX8_MASK(
			glbs->player.gun_x - 8 - glbs->camera.x, glbs->player.gun_y - 8 - glbs->camera.y,
			16, glbs->charge_gfx + glbs->player.charge_frame * 96, 2, FALSE);
	}
	if(glbs->player.leach_type != LEACH_NONE) draw_link();
}

/*SPRITE_HEADER *player_get_hit_detection()
{
	SPRITE_HEADER *header = player_header;
	ENTITY_ANIMATION *animation;

	animation = (ENTITY_ANIMATION *)(metpack_base + player_action[glbs->player.action].animation[0]);
	if(player_action[glbs->player.action].frame_reset) header += animation->frames[0];
	else header += animation->frames[glbs->player.frame * 2];

	if(player_action[glbs->player.action].change_direction) {
		header += 10;
		if(glbs->player.direction == RIGHT)	header += 108;
	}

	return header;
}*/

void player_draw_gravity_bar()
{
	void *light = glbs->light_buffer + 17 * 30 + 16;
	void *dark = glbs->dark_buffer + 17 * 30 + 16;
	short i;
	short len = (24 * glbs->player.gravity_counter) / 1024;
	unsigned long row = 0x00ffffff >> (24 - len);

	for(i = 0 ; i < 5 ; i++) {
		*(long *)light &= 0b11111100000000000000000000000000;
		*(long *)dark &= 0b11111100000000000000000000000000;
		if(i > 0 && i < 4) {
			*(long *)light |= (0x00ffffff << 1);
			*(long *)dark |= (row << 1);
		}
		light += 30;
		dark += 30;
	}
}

void player_continue_action()
{
	if(player_action[glbs->player.action].duration == 1)
		glbs->player.action_duration = 1;
}

void player_next_action()
{
	player_set_action(player_action[glbs->player.action].next_action);
}

typedef struct {
	short x;
	short y;
} POSITION;

void item_menu()
{
	MENU *m;
	long r, new_items;
	short number = 0;
	short i, j;
	char s = 0;

	POSITION pos[] = {
		{4, 62},
		{4, 69},
		{4, 76},
		{4, 83},
		{4, 90},

		{87, 19},
		{87, 26},

		{87, 41},
		{87, 48},
		{87, 55},
		{87, 62},
		{87, 69},
		{87, 76},
		{87, 83},
		{87, 90}
	};

	for(i = 0 ; i < 15 ; i++)
		if(player_item_found(i)) number++;

	if(number == 0) return;

	m = menu_create(number);

	for(i = 0 ; i < 15 ; i++) {
		if(player_item_equiped(i)) s = TRUE;
		else if(player_item_found(i)) s = FALSE;
		else continue;

		menu_add_choice(m, pos[i].x, pos[i].y, s, item_names[i]);
	}

	memset(glbs->light_buffer, 0xff, 3000);
	memset(glbs->dark_buffer, 0xff, 3000);
	draw_border(0, 13, 79, 36, TRUE);
	draw_border(0, 50, 79, 50, TRUE);
	draw_border(83, 13, 77, 22, TRUE);
	draw_border(83, 36, 77, 64, TRUE);
	draw_status_bar();

	for(i = 15 ; i < 19 ; i++) {
		if(player_item_found(i))
			draw_string(4, 13 + 5 + 7 * i - 105, item_names[i], glbs->font, A_NORMAL);
	}

	r = menu_process(m, glbs->font);
	free(m);

	new_items = 0;
	j = 0;
	for(i = 0 ; i < HEAT_SEEKERS ; i++) {
		if(!player_item_found(i)) continue;
		if(r & (1L << j)) new_items |= (1L << i);
		j++;
	}
	glbs->player.items_equiped = new_items;
	/*if(glbs->player.items_equiped & GRAVITY_SUIT) glbs->player.in_water = FALSE;
	else if(glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght > glbs->water.top && !glbs->player.in_water) {
		glbs->player.in_water = TRUE;
		glbs->player.walk_speed = (WALK_SPEED_START >> 1);
		glbs->player.sprint_state = 0;
		glbs->player.jump_speed = 75;
	}*/
}
