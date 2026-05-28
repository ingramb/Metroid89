// C Source File
// Created 1/20/02; 5:12:30 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
//#include "extgraph.h"
#include "dllclipsprites.h"
#include "utility.h"
#include "entity.h"
#include "shots.h"
#include "enemy.h"
#include "player.h"
#include "door.h"
#include "pipe.h"
#include "tiles.h"
#include "block.h"
#include "map.h"
//#include "bar.h"
#include "animation.h"
#include "globals.h"

#define SHOT_ANIMATION_SPEED 7

const unsigned char shot_pattern0[] = {0, 0, 0, 0, 0, 0, 0, 0};
const unsigned char shot_pattern1[] = {0, 1, 2, 3, 4, 5, 6, 7};
const unsigned char shot_pattern2[] = {0, 1, 0, 1, 2, 3, 2, 3};
const unsigned char shot_pattern3[] = {0, 0, 1, 0, 0, 0, 0, 0};

const unsigned char *shot_patterns[] = {shot_pattern0, shot_pattern1, shot_pattern2, shot_pattern3};

short first_shot;
short first_empty_shot;

char shot_setup()
{
	glbs->shot_data =
		(SHOT_DATA *)(glbs->metpack_base + glbs->metpack_header->shot_data_offset);
	glbs->combo_data =
		(SHOT_COMBO *)(glbs->metpack_base + glbs->metpack_header->combo_data_offset);

	shot_reset();

	return TRUE;
}

void shot_reset()
{
	short i;

	memset(glbs->shots, 0, sizeof(SHOT) * SHOT_MAX);
	for(i = 0 ; i < SHOT_MAX - 1 ; i++) glbs->shots[i].next = i + 1;
	glbs->shots[SHOT_MAX - 1].next = -1;
	first_shot = -1;
	first_empty_shot = 0;
}

void shot_cleanup()
{
	//if(glbs->shots != NULL) {
	//	free(glbs->shots);
	//	glbs->shots = NULL;
	//}
}

unsigned short get_combo_id()
{
	static unsigned short id_counter = 0;

	id_counter++;
	if(id_counter == 0) id_counter = 1;
	return id_counter;
}

char tsin(short angle)
{
	if(angle < 0) angle = 256 + angle;
	angle &= 255;
	if(angle < 128) return glbs->trig_table[angle];
	else return -glbs->trig_table[angle - 128];
}

inline char tcos(short angle)
{
	return tsin(angle + 64);
}

char tatan(short h)
{
	if(h > 127) return 64;
	else if(h < -128) return -64;
	else return glbs->atan_table[h + 128];
}

/*typedef struct {
	char player;
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
*/

short get_angle(short x, short y, short x_target, short y_target)
{
	short dx, dy, angle;

	dx = x_target - x;
	dy = -y_target + y;
	angle = tatan((dy * 16) / (dx ? dx : 1));
	if(dx < 0) angle += 128;

	return angle;
}

void shot_create_angle(short x, short y, short angle, short combo)
{
	SHOT_DATA *d;
	SHOT *s;
	short i = first_empty_shot;
	short x1, y1;
	short type = glbs->combo_data[combo].shot_type;

	if(i < 0) return;

	s = glbs->shots + i;

	first_empty_shot = s->next;

	memset(glbs->shots + i, 0, sizeof(SHOT));
	s->next = first_shot;
	first_shot = i;

	d = glbs->shot_data + type;

	x1 = x - (d->width >> 1);
	y1 = y - (d->hieght >> 1);
	s->x = x1;
	s->y = y1;
	s->width = d->width;
	s->hieght = d->hieght;
	s->combo_id = get_combo_id();
	s->type = type;
	s->combo = combo;
	s->frame_direction = 1;
	s->tile_x = (x1 + (s->width >> 1)) / 12;
	s->tile_y = (y1 + (s->hieght >> 1)) / 12;
	s->x_off = (x1 + (s->width >> 1)) % 12;
	s->y_off = (y1 + (s->hieght >> 1)) % 12;
	s->speed = d->initial_speed;
	s->animation_counter = 1;//glbs->shot_data[type].anim_spawn_delay;
	s->gfx = 0;
	s->angle = angle;

	if(glbs->shot_data[type].special == LEACH) {
		glbs->player.leach_type = LEACH_SHOT;
		glbs->player.leach_target = i;
	}
	if(glbs->shot_data[type].special == HEAT_SEAKING) s->target = TARGET_NEEDED;
	else s->target = NONE;
	//if(x_target > s->x) angle += 128;
}

void shot_create_directed(short x, short y, short x_target, short y_target, short combo)
{
	shot_create_angle(x, y, get_angle(x, y, x_target, y_target), combo);
}


void shot_add(SHOT_ARGS *args, char x_direction, char y_direction, char wave_range, char wave_cont)
{
	SHOT *s;
	short i = first_empty_shot;
	short x, y;

	if(i < 0) return;

	s = glbs->shots + i;

	first_empty_shot = s->next;

	memset(glbs->shots + i, 0, sizeof(SHOT));
	s->next = first_shot;
	first_shot = i;

	s->width = args->width;
	s->hieght = args->hieght;

	x = args->x - (args->width >> 1);
	y = args->y - (args->hieght >> 1);
	s->x = x;
	s->y = y;
	s->combo_id = args->id;
	s->type = args->type;
	s->combo = args->combo;
	s->x_direction = x_direction;
	s->y_direction = y_direction;
	s->frame_direction = 1;
	s->tile_x = (x + (s->width >> 1)) / 12;
	s->tile_y = (y + (s->hieght >> 1)) / 12;
	s->x_off = (x + (s->width >> 1)) % 12;
	s->y_off = (y + (s->hieght >> 1)) % 12;
	s->player = args->player;
	s->remove = args->remove;
	s->speed = glbs->shot_data[args->type].initial_speed;
	s->animation_counter = 1;//glbs->shot_data[type].anim_spawn_delay;
	s->gfx = args->gfx;
	s->angle = args->angle;
	s->wave_range = wave_range;
	s->wave_cont = wave_cont;

	if(glbs->shot_data[args->type].special == LEACH) {
		glbs->player.leach_type = LEACH_SHOT;
		glbs->player.leach_target = i;
	}
	if(glbs->shot_data[args->type].special == ANGLE_DECAY) s->decay_counter = 24;
	if(glbs->shot_data[args->type].special == HEAT_SEAKING) s->target = TARGET_NEEDED;
	else s->target = NONE;

	//ClrScr();
	//printf_xy(0, 0, "%d, %d", tsin(glbs->shots[i].angle), tcos(glbs->shots[i].angle));
	//pause();
}

/*
typedef struct {
	char player;
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
*/

void shot_create(char player, char remove, short x, short y, char direction, short combo)
{
	SHOT_ARGS args;
	short pattern = glbs->combo_data[combo].pattern_type;
	char x_dir = 0, y_dir = 0;
	char wave_range = 0;

	args.type = glbs->combo_data[combo].shot_type;
	args.gfx = 0;
	args.angle = 0;
	args.player = player;
	args.remove = remove;
	args.id = get_combo_id();
	args.x = x;
	args.y = y;
	args.combo = combo;

	if(direction == LEFT) {
		args.gfx = 0;
		args.angle = 128;
	} else if(direction == UP) {
		args.gfx = 1;
		args.angle = 64;
	} else if(direction == RIGHT) {
		args.gfx = 2;
		args.angle = 0;
	} else if(direction == DOWN) {
		args.gfx = 3;
		args.angle = 192;
	} else if(direction == (RIGHT | UP)) {
		args.gfx = 4;
		args.angle = 32;
	} else if(direction == (RIGHT | DOWN)) {
		args.gfx = 5;
		args.angle = 224;
	} else if(direction == (LEFT | DOWN)) {
		args.gfx = 6;
		args.angle = 160;
	} else if(direction == (LEFT | UP)) {
		args.gfx = 7;
		args.angle = 96;
	}

	if(direction == LEFT || direction == RIGHT) {
		y_dir = 8;
		args.width = glbs->shot_data[args.type].width;
		args.hieght = glbs->shot_data[args.type].hieght;
		wave_range = 48;
	} else if(direction == UP || direction == DOWN) {
		x_dir = 8;
		args.width = glbs->shot_data[args.type].hieght;
		args.hieght = glbs->shot_data[args.type].width;
		wave_range = 48;
	} else {
		args.width = glbs->shot_data[args.type].diagonal;
		args.hieght = glbs->shot_data[args.type].diagonal;
		wave_range = 40;
		if(direction == (UP | LEFT) || direction == (DOWN | RIGHT)) {
			x_dir = 8;
			y_dir = -8;
		} else {
			x_dir = -8;
			y_dir = -8;
		}
	}

	if(pattern == COMBO_NORMAL)
		shot_add(&args, 0, 0, 0, FALSE);
	else if(pattern == COMBO_WAVE)
		shot_add(&args, x_dir, y_dir, wave_range, TRUE);
	else if(pattern == COMBO_SPREAD) {
		shot_add(&args, x_dir, y_dir, wave_range, FALSE);
		shot_add(&args, 0, 0, 0, FALSE);
		shot_add(&args, -x_dir, -y_dir, wave_range, FALSE);
	} else if(pattern == COMBO_SPLIT) {
		shot_add(&args, x_dir, y_dir, wave_range, TRUE);
		shot_add(&args, -x_dir, -y_dir, wave_range, TRUE);
	} else if(pattern == COMBO_TRISPLIT) {
		shot_add(&args, x_dir, y_dir, wave_range, TRUE);
		shot_add(&args, 0, 0, 0, FALSE);
		shot_add(&args, -x_dir, -y_dir, wave_range, TRUE);
	}
}

void shot_delete(short a)
{
	short i;

	if(glbs->player.leach_type == LEACH_SHOT && glbs->player.leach_target == a)
		glbs->player.leach_type = LEACH_NONE;

	if(a == first_shot)
		first_shot = glbs->shots[a].next;
	else
		for(i = first_shot ; i >= 0 ; i = glbs->shots[i].next)
			if(glbs->shots[i].next == a) {
				glbs->shots[i].next = glbs->shots[a].next;
				break;
			}

	glbs->shots[a].next = first_empty_shot;
	first_empty_shot = a;
}

/*char shot_collide(short x0, short y0, short xdim0, short ydim0, short i)
{
	char direction;
	short diag_width = glbs->shot_data[glbs->shots[i].type].diag_width;

	if(glbs->shots[i].gfx <= 3 || diag_width == NONE)
		return rect_overlap(x0, y0, xdim0, ydim0, glbs->shots[i].x, glbs->shots[i].y, glbs->shots[i].width, glbs->shots[i].hieght);

	if(glbs->shots[i].gfx == 4 || glbs->shots[i].gfx == 6) direction = 0;
	else direction = 1;

	return rect_diag_overlap(x0, y0, xdim0, ydim0,
		glbs->shots[i].x, glbs->shots[i].y, glbs->shots[i].width, diag_width, direction);
}*/

#define ABS(a) ((a) < 0 ? -(a) : (a))
long distance(short x0, short y0, short x1, short y1)
{
	short dx = ABS(x0 - x1);
	short dy = ABS(y0 - y1);

	return dx * dx + dy * dy;
}

void shot_untarget(short e)
{
	short i;

	for(i = first_shot ; i >= 0 ; i = glbs->shots[i].next) {
		if(glbs->shots[i].target == e) glbs->shots[i].target = NONE;
	}
}

void shots_process()
{
	short i, a, target_angle, angle;
	ENTITY *e, *e1;
	short data;
	short type;
	short next;
	short x_pos;
	short y_pos;
	char ok;
	//SPRITE_HEADER *player_header = player_get_hit_detection();
	//unsigned char *player_frame = player_gfx + player_header->offset;
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short pw = glbs->player.e.width;
	short ph = glbs->player.e.hieght;
	short sx, sy, sw, sh, tx, ty;

	for(i = first_shot ; i >= 0 ; i = next) {
		next = glbs->shots[i].next;
		type = glbs->shots[i].type;

		//update speed counters
		glbs->shots[i].x_speed_counter +=
			128 * glbs->shots[i].delta_x + (glbs->shots[i].speed * tcos(glbs->shots[i].angle));
		glbs->shots[i].y_speed_counter -=
			(glbs->shots[i].speed * tsin(glbs->shots[i].angle)) - 128 * glbs->shots[i].delta_y;

		if(glbs->shots[i].angle != 192 && glbs->shots[i].angle != -64)
			glbs->shots[i].angle += glbs->shots[i].angle_decay;

		//possible angle decay
		if(glbs->shots[i].decay_counter) {
			glbs->shots[i].decay_counter--;
			if(glbs->shots[i].decay_counter == 0) {
				if(glbs->shots[i].angle < 64) glbs->shots[i].angle_decay = -2;
				else glbs->shots[i].angle_decay = 2;
			}
		}

		glbs->shots[i].animation_counter--;

		//spawn animation
		if(glbs->shots[i].animation_counter <= 0 && glbs->shot_data[type].spawn_animation != NONE) {
			x_pos = glbs->shots[i].x + (glbs->shots[i].width >> 1);
			y_pos = glbs->shots[i].y + (glbs->shots[i].hieght >> 1);

			animation_create(x_pos, y_pos, glbs->shot_data[type].spawn_animation, NONE);
			glbs->shots[i].animation_counter = glbs->shot_data[type].anim_spawn_delay;
		}

		//regular shot animation
		if(glbs->shot_data[type].frames[1] != -1) {
			glbs->shots[i].frame_counter++;
			if(glbs->shots[i].frame_counter == SHOT_ANIMATION_SPEED) {
				glbs->shots[i].frame += glbs->shots[i].frame_direction;
				if(glbs->shots[i].frame == 2) glbs->shots[i].frame_direction = -1;
				else if(glbs->shots[i].frame == 0) glbs->shots[i].frame_direction = 1;
				glbs->shots[i].frame_counter = 0;
			}
		}

		//handle x direction and y direction
		if(glbs->shots[i].x_direction != 0) {
			glbs->shots[i].delta_x += glbs->shots[i].x_direction;
			if(ABS(glbs->shots[i].delta_x) == glbs->shots[i].wave_range) glbs->shots[i].x_direction *= -1;
		}
		if(glbs->shots[i].x_direction && glbs->shots[i].delta_x == 0 && !glbs->shots[i].wave_cont) glbs->shots[i].x_direction = 0;

		if(glbs->shots[i].y_direction != 0) {
			glbs->shots[i].delta_y += glbs->shots[i].y_direction;
			if(ABS(glbs->shots[i].delta_y) == glbs->shots[i].wave_range) glbs->shots[i].y_direction *= -1;
		}
		if(glbs->shots[i].y_direction && glbs->shots[i].delta_y == 0 && !glbs->shots[i].wave_cont) glbs->shots[i].y_direction = 0;

		//accelerate shot
		if(glbs->shots[i].speed < glbs->shot_data[type].top_speed) glbs->shots[i].speed += 30;

		//update shot position
		while(glbs->shots[i].x_speed_counter >= 3200) {
			glbs->shots[i].x_speed_counter -= 3200;
			glbs->shots[i].x++;
			glbs->shots[i].x_off++;
			if(glbs->shots[i].x_off > 11) {
				glbs->shots[i].x_off = 0;
				glbs->shots[i].tile_x++;
			}
		}
		while(glbs->shots[i].x_speed_counter <= -3200) {
			glbs->shots[i].x_speed_counter += 3200;
			glbs->shots[i].x--;
			glbs->shots[i].x_off--;
			if(glbs->shots[i].x_off < 0) {
				glbs->shots[i].x_off = 11;
				glbs->shots[i].tile_x--;
			}
		}
		while(glbs->shots[i].y_speed_counter >= 3200) {
			glbs->shots[i].y_speed_counter -= 3200;
			glbs->shots[i].y++;
			glbs->shots[i].y_off++;
			if(glbs->shots[i].y_off > 11) {
				glbs->shots[i].y_off = 0;
				glbs->shots[i].tile_y++;
			}
		}
		while(glbs->shots[i].y_speed_counter <= -3200) {
			glbs->shots[i].y_speed_counter += 3200;
			glbs->shots[i].y--;
			glbs->shots[i].y_off--;
			if(glbs->shots[i].y_off < 0) {
				glbs->shots[i].y_off = 11;
				glbs->shots[i].tile_y--;
			}
		}

		//see if shot needs removing
		if(glbs->shots[i].remove && (glbs->shots[i].x - glbs->camera.x < -30 || glbs->shots[i].x - glbs->camera.x > 190 ||
			glbs->shots[i].y - glbs->camera.y < -30 || glbs->shots[i].y - glbs->camera.y > 130)) {
				shot_delete(i);
				continue;
		} else if(glbs->shots[i].tile_x < 0 || glbs->shots[i].tile_x >= glbs->current_map.width ||
			glbs->shots[i].tile_y < 0 || glbs->shots[i].tile_y >= glbs->current_map.hieght) {
				shot_delete(i);
				continue;
		}

		sx = glbs->shots[i].x + glbs->shots[i].header->x_offset;
		sy = glbs->shots[i].y + glbs->shots[i].header->y_offset;
		sw = glbs->shots[i].header->width;
		sh = glbs->shots[i].header->hieght;

		//heat seaking stuff
		if(glbs->shots[i].target != NONE && glbs->shots[i].x_direction == 0 && glbs->shots[i].y_direction == 0) {
			if(glbs->shots[i].target == TARGET_NEEDED) {
				if(glbs->shots[i].player) {
					if(active_enemy_number == 0) glbs->shots[i].target = NONE;
					else {
						short t = 0;
						for(a = 0 ; a < active_enemy_number ; a++) {
							e = &get_enemy(a)->e;
							e1 = &get_enemy(t)->e;
							if(distance(sx, sy, e->x + e->x_off + e->width / 2, e->y + e->y_off + e->hieght / 2) <
								distance(sx, sy, e1->x + e1->x_off + e1->width / 2, e1->y + e1->y_off + e1->hieght / 2))
								t = a;
						}
						glbs->shots[i].target = active_enemies[t];
					}
				} else
					glbs->shots[i].target = TARGET_PLAYER;

				//clrscr();printf("%d: %d", i, glbs->shots[i].target);pause();
			}
			if(glbs->shots[i].target_counter == 0 && glbs->shots[i].target != NONE) {

				if(glbs->shots[i].target == TARGET_PLAYER) { tx = px + pw / 2; ty = py + ph / 2; }
				else {
					e = &enemies[glbs->shots[i].target].e;
					tx = e->x + e->x_off + e->width / 2;
					ty = e->y + e->y_off + e->hieght / 2;
				}
				target_angle = get_angle(sx + sw / 2, sy + sh / 2, tx, ty);

				if(((glbs->shots[i].angle - target_angle) & 255) < 8 ||
				((target_angle - glbs->shots[i].angle) & 255) < 8) glbs->shots[i].angle = target_angle;
				else if(((glbs->shots[i].angle - target_angle) & 255) < 128)
					glbs->shots[i].angle = (glbs->shots[i].angle - 8) & 255;
				else
					glbs->shots[i].angle = (glbs->shots[i].angle + 8) & 255;

				angle = glbs->shots[i].angle;
				if(angle < 16) glbs->shots[i].gfx = 2;
				else if(angle < 48) glbs->shots[i].gfx = 4;
				else if(angle < 80) glbs->shots[i].gfx = 1;
				else if(angle < 112) glbs->shots[i].gfx = 7;
				else if(angle < 144) glbs->shots[i].gfx = 0;
				else if(angle < 176) glbs->shots[i].gfx = 6;
				else if(angle < 208) glbs->shots[i].gfx = 3;
				else if(angle < 240) glbs->shots[i].gfx = 5;
				else glbs->shots[i].gfx = 2;

				glbs->shots[i].target_counter = 1;
			} else glbs->shots[i].target_counter--;
		}

		//hit detection stuff
		data = get_prop_data(glbs->shots[i].tile_x, glbs->shots[i].tile_y);

		if(data >= SOLID || (!glbs->combo_data[glbs->shots[i].combo].wave &&
		block_test(glbs->shots[i].tile_x * 12, glbs->shots[i].tile_y * 12, 12, 12))) {
			if(glbs->shots[i].player) {
				if(data >= BREAK_SHOOT && data <= BREAK_SHOOT_BOMB_SWITCH) {
					hole_create(glbs->shots[i].tile_x, glbs->shots[i].tile_y, glbs->shot_data[type].damage_type);
				}
				for(a = 0 ; a < door_number ; a++)
					if(doors[a].status == DOOR_CLOSED &&
						rect_overlap(glbs->shots[i].tile_x * 12, glbs->shots[i].tile_y * 12, 12, 12,
							doors[a].x, doors[a].y,
							doors[a].width, doors[a].hieght)) {
								door_open(doors + a, glbs->shot_data[type].damage_type);
								break;
					}
				for(a = 0 ; a < glbs->pipe_number ; a++)
					if(rect_overlap(glbs->shots[i].tile_x * 12, glbs->shots[i].tile_y * 12, 12, 12,
						glbs->pipes[a].switch_x, glbs->pipes[a].switch_y, 12, 12)) {
							pipe_open(glbs->pipes + a, glbs->shot_data[type].damage_type);
							break;
					}
			}
			if(!glbs->combo_data[glbs->shots[i].combo].wave) {
				shot_delete(i);
				animation_create(glbs->shots[i].x + glbs->shots[i].width / 2,
					glbs->shots[i].y + glbs->shots[i].hieght / 2, glbs->shot_data[type].death_animation, NONE);
			}
		}

		if(glbs->shots[i].player) {
			ok = FALSE;
			for(a = 0 ; a < active_enemy_number ; a++) {
				if(glbs->shots[i].enemy_hit & (1 << active_enemies[a])) continue;

				e = &get_enemy(a)->e;
				if(TestCollideX8(sx, sy, sw, sh,
					e->x + e->sprite->x_offset, e->y + e->sprite->y_offset,
					e->sprite->width, e->sprite->hieght,
					glbs->shot_gfx + glbs->shots[i].header->offset, enemy_gfx[get_enemy(a)->type].gfx + e->sprite->offset)) {
					if(!glbs->shot_data[type].penetrate && get_enemy(a)->hp > 0) ok = TRUE;
				//shot_collide(e->x + e->x_off, e->y + e->y_off, e->width, e->hieght, i)) {
					if(!enemy_hit(active_enemies[a],
						glbs->combo_data[glbs->shots[i].combo].damage, glbs->shot_data[type].freeze,
						glbs->shot_data[type].damage_type, glbs->shots[i].combo_id)) {
							animation_create(glbs->shots[i].x + glbs->shots[i].width / 2,
								glbs->shots[i].y + glbs->shots[i].hieght / 2, glbs->shot_data[type].death_animation, NONE);
					} else if(glbs->shot_data[type].special == LEACH && glbs->player.leach_type == LEACH_SHOT &&
					glbs->player.leach_target == i) {
						glbs->player.leach_type = LEACH_ENEMY;
						glbs->player.leach_target = active_enemies[a];
						ok = TRUE;
					}
					glbs->shots[i].enemy_hit |= (1 << active_enemies[a]);
				}
			}
			if(ok) shot_delete(i);
		} else if(glbs->player.e.damage_type == DAMAGE_STAND &&
		TestCollideX8(sx, sy, sw, sh, px, py, 2, ph,
		glbs->shot_gfx + glbs->shots[i].header->offset, player_size_mask)) {
			if(player_hurt(glbs->combo_data[glbs->shots[i].combo].damage, glbs->shots[i].x, glbs->shots[i].y))
				shot_delete(i);
		}

	}
}

void shots_draw()
{
	//SPRITE_HEADER *header;
	short type;
	short x, y;
	register short i;

	for(i = first_shot ; i >= 0 ; i = glbs->shots[i].next) {
		//if(glbs->shots[i].leach_target != NONE) continue;
		type = glbs->shots[i].type;
		glbs->shots[i].header = glbs->shot_header + glbs->shot_data[type].frames[glbs->shots[i].frame] +
			shot_patterns[glbs->shot_data[type].pattern][glbs->shots[i].gfx];

		x = glbs->shots[i].header->x_offset + glbs->shots[i].x - glbs->camera.x;
		y = glbs->shots[i].header->y_offset + glbs->shots[i].y - glbs->camera.y;

		GraySpriteClipX8_MASK(x, y, glbs->shots[i].header->hieght,
			(glbs->shot_gfx + glbs->shots[i].header->offset), glbs->shots[i].header->width, FALSE);
	}
}
