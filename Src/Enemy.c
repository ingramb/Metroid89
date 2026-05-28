// C Source File
// Created 1/22/02; 6:42:18 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "extgraph.h"
#include "entity.h"
#include "player.h"
#include "utility.h"
#include "animation.h"
#include "tiles.h"
#include "enemy.h"
#include "door.h"
#include "map.h"
#include "shots.h"
#include "block.h"
#include "powerup.h"
#include "clipsprites.h"
#include "bar.h"
#include "miscgfx.h"
#include "globals.h"

#define ACTIVE_ENEMY_MAX 15
#define FREEZE_DURATION 500

void enemy_set_action(ENEMY *e, short animation);
void enemy_fall(ENEMY *e);
char ai_check_platform(ENEMY *e);
void ai_face_player(ENEMY *e);

char ship_ai(ENEMY *e);
void ship_init(ENEMY *e);
char pirate_ai(ENEMY *e);
void pirate_get_hit(ENEMY *e, short damage);
char climber_ai(ENEMY *e);
char crawler_ai(ENEMY *e);
char skree_ai(ENEMY *e);
void skree_killed(ENEMY *e);
char hopper_ai(ENEMY *e);
char chozo_ai(ENEMY *e);
void chozo_get_hit(ENEMY *e, short damage);
char spikey_ai(ENEMY *e);
char ripper_ai(ENEMY *e);
char acid_ai(ENEMY *e);
void acid_init(ENEMY *e);
void acid_get_hit(ENEMY *e, short damage);
void acid_killed(ENEMY *e);
char plant_ai(ENEMY *e);
char robot_ai(ENEMY *e);
char ridley_ai(ENEMY *e);
char floater_ai(ENEMY *e);
char kraid_ai(ENEMY *e);
void kraid_get_hit(ENEMY *e, short damage);
void kraid_init(ENEMY *e);
void kraid_killed(ENEMY *e);
char kraid_spike_ai(ENEMY *e);
void kraid_spike_killed(ENEMY *e);
char kraid_claw_ai(ENEMY *e);
void kraid_claw_killed(ENEMY *e);
void swooper_init(ENEMY *e);
char swooper_ai(ENEMY *e);
char sucker_ai(ENEMY *e);
char lavahorse_walker_ai(ENEMY *e);
char pipespawn_ai(ENEMY *e);
char pipeguy_ai(ENEMY *e);
void pipeguy_killed(ENEMY *e);
char nukeball_ai(ENEMY *e);
char skree_blob_ai(ENEMY *e);

void ridley_draw_tail(ENEMY *e);

const ENEMY_AI enemy_ai[ENEMY_END_OF_LIST] = {
	{ship_ai, ship_init, NULL, NULL},
	{pirate_ai, NULL, pirate_get_hit, NULL},
	{climber_ai, NULL, NULL, NULL},
	{crawler_ai, NULL, NULL, NULL},
	{skree_ai, NULL, NULL, skree_killed},
	{hopper_ai, NULL, NULL, NULL},
	{chozo_ai, NULL, chozo_get_hit, NULL},
	{spikey_ai, NULL, NULL, NULL},
	{ripper_ai, NULL, NULL, NULL},
	{acid_ai, acid_init, acid_get_hit, NULL},
	{plant_ai, NULL, NULL, NULL},
	{robot_ai, NULL, NULL, NULL},
	{ridley_ai, NULL, NULL, NULL},
	{floater_ai, NULL, NULL, NULL},
	{kraid_ai, kraid_init, kraid_get_hit, kraid_killed},
	{swooper_ai, swooper_init, NULL, NULL},
	{pipespawn_ai, NULL, NULL, NULL},
	{sucker_ai, NULL, NULL, NULL},
	{lavahorse_walker_ai, NULL, NULL, NULL},
	
	{pipeguy_ai, NULL, NULL, pipeguy_killed},
	{pipeguy_ai, NULL, NULL, pipeguy_killed},
	{pipeguy_ai, NULL, NULL, pipeguy_killed},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL},
	{kraid_spike_ai, NULL, NULL, kraid_spike_killed},
	{kraid_claw_ai, NULL, NULL, kraid_claw_killed},
	{nukeball_ai, NULL, NULL, NULL},
	{skree_blob_ai, NULL, NULL, NULL},
};

ENEMY_DATA *enemy_data = NULL;
ENEMY_ACTION *enemy_action = NULL;

char killed_enemies[KILLED_ENEMIES_SIZE];

ENEMY_GFX enemy_gfx[ENEMY_END_OF_LIST];

void *enemy_file;

ENEMY *enemies = NULL;
short enemy_number;

short active_enemies[ACTIVE_ENEMY_MAX];
short active_enemy_number;
short enemy_kills;
short first_enemy;
short first_empty_enemy;
short checked_tiles;

char enemy_setup()
{
	short i;

	enemy_number = 0;

	if(!file_exists(SYMSTR("enemy"))) return FALSE;

	enemy_file = file_pointer(SYMSTR("enemy"));

	if(!ttarchive_valid(enemy_file)) return FALSE;

	enemy_data = (ENEMY_DATA *)(glbs->metpack_base + glbs->metpack_header->enemy_data_offset);
	enemy_action = (ENEMY_ACTION *)(glbs->metpack_base + glbs->metpack_header->enemy_action_offset);

	for(i = 0 ; i < ENEMY_END_OF_LIST ; i++) {
		enemy_gfx[i].header = NULL;
		enemy_gfx[i].frame_number = 0;
		enemy_gfx[i].buffer = H_NULL;
		enemy_gfx[i].gfx = NULL;
		enemy_gfx[i].used = FALSE;
	}

	return TRUE;
}

void enemy_cleanup()
{
	enemy_reset(NULL, 0);
}

void enemy_reset(ENEMY_HEADER *header, short number)
{
	short i;

	if(enemies != NULL) {
		for(i = 0 ; i < enemy_number ; i++) {
			if(enemies[i].frozen_counter) enemy_unfreeze(enemies + i);
		}
		free(enemies);
		enemies = NULL;
	}

	for(i = 0 ; i < ENEMY_END_OF_LIST ; i++) enemy_gfx[i].used = FALSE;
	for(i = 0 ; i < number ; i++) {
		short byte = header[i].index >> 3;
		char bit = header[i].index & 7;
		if(enemy_data[header[i].type].gfx != 0xff && !(killed_enemies[byte] & (1 << bit)))
			enemy_gfx[header[i].type].used = TRUE;
	}

	for(i = 0 ; i < ENEMY_END_OF_LIST ; i++)
		if(enemy_gfx[i].used == FALSE && enemy_gfx[i].buffer != H_NULL) {
			HeapFree(enemy_gfx[i].buffer);
			enemy_gfx[i].buffer = H_NULL;
		}

	checked_tiles = FALSE;
	enemy_kills = 0;
}

void enemy_set_action(ENEMY *e, short action)
{
	if(action == NONE) return;

	e->action = action;
	e->action_duration = enemy_action[action].duration;
	e->frame = 0;
	e->frame_counter = 0;
}

void enemy_set_extra_anim(ENEMY *e, ENTITY_ANIMATION *a)
{
	e->extra_anim = a;
	e->extra_frame = 0;
	e->extra_frame_counter = 0;
}

void enemy_set_path(ENEMY *e, PROJECTILE_PATH *p, char direction, short horiz_speed, short vert_speed)
{
	e->path = p;
	e->path_horiz_speed = horiz_speed;
	e->path_vert_speed = vert_speed;
	e->path_vert_direction = UP;
	e->path_horiz_direction = direction;
}

ENEMY *get_enemy(short i)
{
	return enemies + active_enemies[i];
}

void enemy_init(ENEMY_HEADER *header, short number)
{
	ENTITY_SIZE *size;
	ENTITY *e;
	short i, l, frame_size, type;
	unsigned short a;
	unsigned short gfx_size, old_gfx_size;
	unsigned short hdr_size, old_hdr_size;
	unsigned short skip_offset;
	void *gfx_ptr, *hdr_ptr;
	SPRITE_HEADER *orig_header, *flip_header;
	unsigned char *orig_sprite, *flip_sprite;
	short total_number = number;
	char pages;
	char byte, bit;
	char killed_flags[number], killed_number = 0;

	active_enemy_number = 0;
	enemy_number = number;

	memset(killed_flags, 0, number * sizeof(char));

	for(i = 0 ; i < number ; i++) {
		if(header[i].index >= 0) {
			byte = header[i].index >> 3;
			bit = header[i].index & 7;
			if(killed_enemies[byte] & (1 << bit)) {
				killed_flags[i] = TRUE;
				killed_number++;
				continue;
			}
		}

		if(enemy_data[header[i].type].extra_enemies > 0) {
			total_number += enemy_data[header[i].type].extra_enemies;
			a = enemy_data[header[i].type].extra_enemy_type;
			while(a != NONE) {
				if(enemy_data[a].gfx != 0xff) enemy_gfx[a].used = TRUE;
				a = enemy_data[a].extra_enemy_type;
			}
		}
	}

	if(number) {
		enemies = malloc(sizeof(ENEMY) * total_number);
		memset(enemies, 0, sizeof(ENEMY) * total_number);
	} else
		enemies = NULL;

	for(i = 0 ; i < number ; i++) {
		enemies[i].index = i;
		enemies[i].next = i + 1;

		if(killed_flags[i]) {
			enemies[i].hp = 0;
			enemy_kills++;
			continue;
		}
		type = header[i].type;
		e = &enemies[i].e;
		size = &enemy_data[type].size;

		entity_init(e, header[i].x, header[i].y, size, enemy_data[type].simple_movement);
		if(header[i].gravity != DOWN) entity_set_gravity(e, header[i].gravity);

		enemies[i].tail_type = NONE;
		enemies[i].path = NULL;
		enemies[i].immune_combo = 0;
		enemies[i].killed_index = header[i].index;
		enemies[i].powerup = -1;
		enemies[i].direction = header[i].direction;
		enemies[i].type = header[i].type;
		enemies[i].subtype = header[i].subtype;
		enemies[i].hp = enemy_data[type].hp;
		enemies[i].immune = enemy_data[type].immune;
		enemies[i].frozen_handle = H_NULL;
		enemies[i].extra_anim = NULL;
		enemy_set_action(enemies + i, enemy_data[type].init_action);
		if(enemy_data[type].disable_slope) {
			enemies[i].e.slope_left_up = NONE;
			enemies[i].e.slope_right_up = NONE;
		}
	}

	if(number != 0) {
		enemies[number - 1].next = -1;

		for(i = number ; i < total_number - 1 ; i++)
			enemies[i].next = i + 1;
		enemies[total_number - 1].next = -1;
	}

	first_enemy = 0;
	if(total_number > number) first_empty_enemy = number;
	else first_empty_enemy = -1;

	for(i = 0 ; i < number ; i++) {
		if(enemy_ai[header[i].type].init_ai != NULL)
			(*enemy_ai[header[i].type].init_ai)(enemies + i);
	}

	for(i = 0 ; i < ENEMY_END_OF_LIST ; i++) {
		HeapCompress();

		if(!enemy_gfx[i].used) continue;
		if(enemy_gfx[i].buffer != H_NULL) continue;

		hdr_ptr = ttarchive_data(enemy_file, enemy_data[i].gfx * 2);
		gfx_ptr = ttarchive_data(enemy_file, enemy_data[i].gfx * 2 + 1);

		if(ttunpack_valid(gfx_ptr) && ttunpack_valid(hdr_ptr)) {
			hdr_size = ttunpack_size(hdr_ptr); old_hdr_size = hdr_size;
			gfx_size = ttunpack_size(gfx_ptr);
			if(gfx_size & 1) gfx_size++;
			old_gfx_size = gfx_size;

			enemy_gfx[i].buffer = HeapAlloc(hdr_size);
			decompress(hdr_ptr, HeapDeref(enemy_gfx[i].buffer));
			enemy_gfx[i].header = HLock(enemy_gfx[i].buffer) + 2;

			skip_offset = 0;
			if(enemy_data[i].extra_frame_skip != NO_EXTRA_FRAMES) {
				hdr_size = hdr_size * 2 - sizeof(SPRITE_HEADER) * enemy_data[i].extra_frame_skip - 2;
				gfx_size = gfx_size * 2;
				skip_offset = enemy_gfx[i].header[enemy_data[i].extra_frame_skip].offset;
				gfx_size -= skip_offset;
			}

			HeapUnlock(enemy_gfx[i].buffer);
			/*if(1) {
				ClrScr();
				printf_xy(0, 0, "ALLOC:%u", hdr_size + gfx_size);
				printf_xy(0, 8, "AVAIL:%lu", HeapAvail());
				printf_xy(0, 16, "BLOCK:%lu", HeapMax());

				pause();
			}*/
			enemy_gfx[i].buffer = HeapRealloc(enemy_gfx[i].buffer, hdr_size + gfx_size);
			if(enemy_gfx[i].buffer == H_NULL) {ClrScr();printf_xy(0, 0, "Enemy alloc failed");pause();}

			enemy_gfx[i].header = HLock(enemy_gfx[i].buffer) + 2;
			enemy_gfx[i].gfx = HeapDeref(enemy_gfx[i].buffer) + hdr_size;

			decompress(gfx_ptr, enemy_gfx[i].gfx);

			enemy_gfx[i].frame_number = *(short *)(HeapDeref(enemy_gfx[i].buffer));

			if(enemy_data[i].extra_frame_skip != NO_EXTRA_FRAMES) {

				size = &enemy_data[i].size;

				for(a = 0 ; a < enemy_gfx[i].frame_number - enemy_data[i].extra_frame_skip ; a++) {
					orig_header = enemy_gfx[i].header + a + enemy_data[i].extra_frame_skip;
					flip_header = (SPRITE_HEADER *)(HeapDeref(enemy_gfx[i].buffer) + old_hdr_size) + a;

					memcpy(flip_header, orig_header, sizeof(SPRITE_HEADER));
					flip_header->x_offset = size->frame_width - flip_header->width * 8 - flip_header->x_offset;
					flip_header->offset += old_gfx_size - skip_offset;

					frame_size = orig_header->hieght * orig_header->width;

					orig_sprite = enemy_gfx[i].gfx + orig_header->offset;
					flip_sprite = enemy_gfx[i].gfx + old_gfx_size - skip_offset + orig_header->offset;

					if(enemy_data[i].wmask) pages = 2;
					else pages = 3;

					for(l = 0 ; l < pages ; l++) {
						SpriteX8_MIRROR_H(orig_header->hieght, orig_sprite,
							orig_header->width, flip_sprite);
						orig_sprite += frame_size;
						flip_sprite += frame_size;
					}
				}
			}
		}
	}

	if(number > ACTIVE_ENEMY_MAX) number = ACTIVE_ENEMY_MAX;

	/*for(i = 0 ; i < enemy_gfx[2].frame_number * 2 ; i++) {
		clrscr();
		printf("offset:%d width:%d", enemy_gfx[2].header[i].offset, enemy_gfx[2].header[i].width);
		pause();
	}*/

	for(i = 0 ; i < number ; i++)
		active_enemies[i] = i;

	active_enemy_number = number;
}

void enemy_delete(short a)
{
	short i;
	
	if(enemy_ai[enemies[a].type].killed_ai != NULL) (*enemy_ai[enemies[a].type].killed_ai)(enemies + a);
	if(enemies[a].frozen_counter != 0) enemy_unfreeze(enemies + a);
	shot_untarget(a);
	enemy_kills++;
	
	if(enemies[a].killed_index >= 0) {
		char byte = enemies[a].killed_index >> 3;
		char bit = enemies[a].killed_index & 7;
		killed_enemies[byte] |= (1 << bit);
	}

	if(a == first_enemy)
		first_enemy = enemies[a].next;
	else
		for(i = first_enemy ; i >= 0 ; i = enemies[i].next)
			if(enemies[i].next == a) {
				enemies[i].next = enemies[a].next;
				break;
			}

	enemies[a].next = first_empty_enemy;
	first_empty_enemy = a;
}

ENEMY *enemy_create_path(short x, short y, char direction, short type, PROJECTILE_PATH *p, short horiz_speed, short vert_speed)
{
	short i = first_empty_enemy;
	short xpos;
	short ypos;
	ENEMY *e;
	ENTITY_SIZE *size;

	if(i < 0) return NULL;

	enemy_kills--;

	e = enemies + i;

	first_empty_enemy = e->next;

	memset(e, 0, sizeof(ENEMY));

	e->next = first_enemy;
	e->index = i;
	first_enemy = i;

	size = &enemy_data[type].size;
	xpos = x - size->x_off - size->width / 2;
	ypos = y - size->y_off - size->hieght / 2;
	entity_init(&e->e, xpos, ypos, size, enemy_data[type].simple_movement);
	e->powerup = -1;
	e->killed_index = -1;
	e->path = NULL;
	e->tail_type = NONE;
	e->direction = direction;
	e->type = type;
	e->hp = enemy_data[type].hp;
	e->immune = enemy_data[type].immune;
	e->frozen_handle = H_NULL;
	enemy_set_path(e, p, direction, horiz_speed, vert_speed);
	enemy_set_action(e, enemy_data[type].init_action);
	
	return e;
}

ENEMY *enemy_create(short x, short y, char direction, short type)
{
	return enemy_create_path(x, y, direction, type, NULL, 0, 0);
}

void enemy_drop_powerup(ENEMY *e)
{
	ENEMY_DATA *data = enemy_data + e->type;
	char roll = random(100) + 1;
	char a;
	short i;
	short x = e->e.x + e->e.x_off + e->e.width / 2;
	short y = e->e.y + e->e.y_off + e->e.hieght / 2;

	if(roll > data->powerup_drop_rate) return;

	for(i = 0 ; i < 4 ; i++) {
		roll = random(15);
		if(roll <= i) continue;
		a = data->powerup_priority[i];
		if(a == POWERUP_HEALTH && glbs->player.hp != glbs->player.hp_max) {
			roll = random(10);
			if(roll  < 6) e->powerup = powerup_create(x, y, 0);
			else e->powerup = powerup_create(x, y, 1);
			break;
		} else if(a == POWERUP_MISSILE && glbs->player.ammunition[0] != glbs->player.ammunition_max[0]) {
			e->powerup = powerup_create(x, y, 2);
			break;
		} else if(a == POWERUP_SUPERMISSILE && glbs->player.ammunition[1] != glbs->player.ammunition_max[1]) {
			e->powerup = powerup_create(x, y, 3);
			break;
		} else if(a == POWERUP_POWERBOMB && glbs->player.ammunition[2] != glbs->player.ammunition_max[2]) {
			e->powerup = powerup_create(x, y, 4);
			break;
		} else if(a == NONE) return;
	}
}

void enemy_explode(ENEMY *e, short damage_type)
{
	short type;

	e->hp = 0;
	if(damage_type < DAMAGE_CHARGE_SCREW) type = enemy_data[e->type].death_anim;
	else type = SCREW_EXPLODE;
	animation_create(
		e->e.x + e->e.x_off + e->e.width / 2,
		e->e.y + e->e.y_off + e->e.hieght / 2, type, NONE);
}

void ai_reverse(ENEMY *e);

char enemy_do_path(ENEMY *e)
{
	PROJECTILE_PATH *p = e->path;
	char simple = enemy_data[e->type].simple_movement;

	if(p == NULL) return FALSE;

	if(!entity_move(&e->e, e->path_horiz_direction, e->path_horiz_speed)) {
		if(simple && !p->horiz_bounce) {
			e->hit_horiz_wall = TRUE;
			return TRUE;
		} else if(!simple) {
			if(p->horiz_bounce) ai_reverse(e);
			//e->path_vert_direction = DOWN;
		}
	}
	if(!entity_move(&e->e, e->path_vert_direction, e->path_vert_speed)) {
		if((e->path_vert_direction == DOWN && !simple) || (simple && !p->vert_bounce)) {
			e->hit_vert_wall = TRUE;
			return TRUE;
		} else if(!simple) e->path_vert_direction = DOWN;
	}

	if(e->path_vert_direction == UP) {
		e->path_vert_speed -= p->vert_accel;
		if(e->path_vert_speed <= p->vert_speed_min) e->path_vert_direction = DOWN;
	} else if(e->path_vert_speed < p->vert_speed_max) e->path_vert_speed += p->vert_accel;

	return FALSE;
}

void enemy_process()
{
	ENTITY_ANIMATION *animation;
	ENEMY *enemy;
	ENTITY *e;
	short i;
	short next = 0;
	//SPRITE_HEADER *player_header = player_get_hit_detection();
	//unsigned char *player_frame = player_gfx + player_header->offset;
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	//short pw = glbs->player.e.width;
	short ph = glbs->player.e.hieght;

	if(enemy_kills == enemy_number && checked_tiles == 0) {
		hole_enemy_check();
		checked_tiles = TRUE;
	}

	active_enemy_number = 0;
	if(enemy_number == 0) return;

	for(i = first_enemy ; i >= 0 ; i = next) {
		enemy = enemies + i;
		next = enemy->next;

		if(!enemy->hp) {
			enemy_delete(i);
			continue;
		}

		if(active_enemy_number == ACTIVE_ENEMY_MAX) continue;

		e = &enemy->e;

		if(enemy_data[enemy->type].disable && (e->x - glbs->camera.x < -80 || e->x - glbs->camera.x > 240 ||
			e->y - glbs->camera.y < -80 || e->y - glbs->camera.y > 180)) {
				if(enemy_data[enemy->type].disable == DISABLE_KILL) enemy->hp = 0;
				continue;
		}

		active_enemies[active_enemy_number] = i;
		active_enemy_number++;

		if(enemy->frozen_counter) {
			enemy->frozen_counter--;
			if(enemy->frozen_counter == 0) enemy_unfreeze(enemy);
			continue;
		}

		if(enemy_ai[enemy->type].ai != NULL) {
			if((*enemy_ai[enemy->type].ai)(enemy)) {
				enemy->hp = 0;
				continue;
			}
		}

		animation = ((ENTITY_ANIMATION *)(glbs->metpack_base + enemy_action[enemy->action].animation));

		enemy->hit_horiz_wall = FALSE;
		enemy->hit_vert_wall = FALSE;

		if(enemy->action_duration > 0) enemy->action_duration--;

		if(enemy->action_duration) {
			enemy->frame_counter++;

			//if(enemy->type == ENEMY_KRAID_FOOT) {
			//	clrscr();
			//	printf("frame:%d, max:%d", enemy->frame, animation->frame_number);
			//	pause();
			//}
			if(enemy->frame_counter >=
				animation->frames[enemy->frame * 2 + 1]) {
					enemy->frame++;
					enemy->frame_counter = 0;

				if(enemy->frame == animation->frame_number)
					enemy->frame = animation->repeat_frame;
			}

			if(enemy_action[enemy->action].move_speed != 0) {
				if(!entity_move(&enemy->e, enemy->direction, enemy_action[enemy->action].move_speed)) {
					enemy->action_duration = 0;
					enemy->hit_horiz_wall = TRUE;
				}
			} else if(enemy_do_path(enemy)) {
				enemy->action_duration = 0;
				enemy->path = NULL;
			}
		}

		animation = enemy->extra_anim;
		if(animation != NULL) {
			enemy->extra_frame_counter++;
			if(enemy->extra_frame_counter >=
					animation->frames[enemy->extra_frame * 2 + 1]) {
						enemy->extra_frame++;
						enemy->extra_frame_counter = 0;

						if(enemy->extra_frame == animation->frame_number)
							enemy->extra_frame = animation->repeat_frame;
			}
		}

		if(e->sprite && TestCollideX8(e->x + e->sprite->x_offset, e->y + e->sprite->y_offset,
			e->sprite->width, e->sprite->hieght, px, py, 2, ph,
			enemy_gfx[enemy->type].gfx + e->sprite->offset, player_size_mask)) {
		//rect_overlap(e->x + e->x_off, e->y + e->y_off, e->width, e->hieght, px, py, pw, ph)) {
			if(glbs->player.e.damage_type & DAMAGE_SCREW)
				enemy_hit(i, enemy->hp, FALSE, DAMAGE_SCREW, get_combo_id());
			else if(glbs->player.e.damage_type & DAMAGE_SPEED)
				enemy_hit(i, enemy->hp, FALSE, DAMAGE_SPEED, get_combo_id());
			else if((glbs->player.charge_level == 3 && (glbs->player.action_type & TYPE_FLIP))) {
				enemy_hit(i, 5, FALSE, DAMAGE_CHARGE_SCREW, get_combo_id());
				glbs->player.charge_level = 0;
				glbs->player.charge_counter = 0;
			} else if(enemy_data[enemy->type].damage != 0) {
				player_hurt(enemy_data[enemy->type].damage, e->x + e->x_off + e->width / 2,
					e->y + e->y_off + e->hieght / 2, enemy_data[enemy->type].solid);
				if(enemy_data[enemy->type].explosive) enemy_explode(enemy, NONE);
			} else if(enemy->type == ENEMY_SUCKER && enemy->action != ACT_SUCKER_SUCK)
				enemy_set_action(enemy, ACT_SUCKER_SUCK); //little hack
		}
	}
}

//type = 0 : regular animation, type = 1 : extra animation
SPRITE_HEADER *get_enemy_frame(ENEMY *e, char animation_type)
{
	ENTITY_ANIMATION *animation;
	SPRITE_HEADER *header;
	short type = e->type;

	if(animation_type == 0) {
		animation = ((ENTITY_ANIMATION *)(glbs->metpack_base + enemy_action[e->action].animation));
		header = enemy_gfx[type].header + animation->frames[e->frame * 2];
	} else {
		animation = e->extra_anim;
		header = enemy_gfx[type].header + animation->frames[e->extra_frame * 2];
	}

	if(e->e.gravity == LEFT) header += enemy_data[type].gravity_left_offset;
	else if(e->e.gravity == RIGHT) header += enemy_data[type].gravity_right_offset;

	if(enemy_action[e->action].reverse_flip) {
		if(e->direction == RIGHT && enemy_data[type].reverse_offset) {
			if(enemy_data[type].reverse_offset == REVERSE_NORMAL) header += enemy_gfx[type].frame_number;
			else header += enemy_data[type].reverse_offset;
		} else if(enemy_data[type].extra_frame_skip != NO_EXTRA_FRAMES)
			header += enemy_data[type].extra_frame_skip;
	}

	return header;
}

void enemy_draw()
{
	//ENTITY_ANIMATION *animation;
	SPRITE_HEADER *header;
	ENEMY *e;
	register short i;
	short type;

	for(i = 0 ; i < active_enemy_number ; i++) {
		e = enemies + active_enemies[i];
		type = e->type;
		if(enemy_data[type].gfx == 0xff) continue;

		if(e->frozen_counter != 0 && e->frozen_handle != H_NULL) {
			entity_draw(&e->e, HeapDeref(e->frozen_handle), e->flash, FALSE, FALSE);
			if(e->frozen_counter < 75) e->flash = !e->flash;
			else if(e->flash) e->flash = FALSE;
			continue;
		}

		/*animation = ((ENTITY_ANIMATION *)(metpack_base + enemy_action[e->action].animation));
		header = enemy_gfx[type].header + animation->frames[e->frame * 2];

		if(e->e.gravity == LEFT) header += enemy_data[type].gravity_left_offset;
		if(e->e.gravity == RIGHT) header += enemy_data[type].gravity_right_offset;

		if(enemy_action[e->action].reverse_flip) {
			if(e->direction == RIGHT && enemy_data[type].reverse_offset) {
				if(enemy_data[type].reverse_offset == REVERSE_NORMAL) header += enemy_gfx[type].frame_number;
				else header += enemy_data[type].reverse_offset;
			} else if(enemy_data[type].extra_frame_skip != NO_EXTRA_FRAMES)
				header += enemy_data[type].extra_frame_skip;
		}*/

		e->e.sprite = get_enemy_frame(e, 0); header = e->e.sprite;
		entity_draw(&e->e, enemy_gfx[type].gfx, e->flash, TRUE, enemy_data[type].wmask);

		if(e->extra_anim != NULL) {
			/*header = enemy_gfx[type].header + animation->frames[e->extra_frame * 2];

			if(e->e.gravity == LEFT) header += enemy_data[type].gravity_left_offset;
			if(e->e.gravity == RIGHT) header += enemy_data[type].gravity_right_offset;

			if(enemy_action[e->action].reverse_flip) {
				if(e->direction == RIGHT && enemy_data[type].reverse_offset) {
					if(enemy_data[type].reverse_offset == REVERSE_NORMAL) header += enemy_gfx[type].frame_number;
					else header += enemy_data[type].reverse_offset;
				} else if(enemy_data[type].extra_frame_skip != NO_EXTRA_FRAMES)
					header += enemy_data[type].extra_frame_skip;
			}*/

			e->e.sprite = get_enemy_frame(e, 1);
			entity_draw(&e->e, enemy_gfx[type].gfx, e->flash, TRUE, enemy_data[type].wmask);
			e->e.sprite = header;
		}

		if(e->tail_type != NONE) ridley_draw_tail(e);

		if(e->flash) e->flash = FALSE;
	}
}

void enemy_next_action(ENEMY *e)
{
	enemy_set_action(e, enemy_action[e->action].next_action);
}

char enemy_hit(short i, short damage, char freeze, short damage_type, unsigned short combo_id)
{
	ENEMY *e = enemies + i;

	if(i < 0) return TRUE;
	if(e->hp <= 0 || e->immune_combo == combo_id) return TRUE;

	if(freeze && e->hp <= damage && e->frozen_counter == 0) {
		enemy_freeze(e, combo_id);
		return TRUE;
	}

	if(e->immune & damage_type) {
		if(enemy_ai[e->type].hit_ai != NULL) (*enemy_ai[e->type].hit_ai)(e, -1);
		return FALSE;
	}

	e->hp -= damage;
	e->flash = TRUE;

	if(e->hp > 0) {
		if(enemy_ai[e->type].hit_ai != NULL && e->frozen_counter == 0)
			(*enemy_ai[e->type].hit_ai)(e, damage);
	} else {
		enemy_explode(e, damage_type);
		enemy_drop_powerup(e);
	}

	return TRUE;
}

void enemy_freeze(ENEMY *e, unsigned short combo_id)
{
	SPRITE_HEADER *header = e->e.sprite;
	ENTITY *pos = &e->e;
	short frame_size = header->width * header->hieght;
	char *src_light, *src_dark, *src_mask, *dst_light, *dst_dark, *dst_mask;
	short i;

	if(e->frozen_counter) return;

	e->frozen_counter = FREEZE_DURATION;
	e->immune_combo = combo_id;

	e->frozen_block_handle =
		block_create(pos->x + pos->x_off, pos->y + pos->y_off, pos->width, pos->hieght);

	e->frozen_handle = HeapAlloc(frame_size * 3);
	dst_light = HeapDeref(e->frozen_handle);
	dst_dark = dst_light + frame_size;
	dst_mask = dst_dark + frame_size;
	src_light = enemy_gfx[e->type].gfx + header->offset;
	src_dark = src_light + frame_size;
	src_mask = src_dark + frame_size;

	for(i = 0 ; i < frame_size ; i++) {
		*dst_dark++ = (*src_light & *src_dark);
		*dst_light++ = *src_dark;
		*dst_mask++ = *src_mask++;
		src_light++;
		src_dark++;
	}
}

void enemy_unfreeze(ENEMY *e)
{
	block_delete(e->frozen_block_handle);
	e->frozen_block_handle = -1;
	if(e->frozen_handle != H_NULL) HeapFree(e->frozen_handle);
	e->frozen_handle = H_NULL;
	e->flash = FALSE;
}

char ai_check_platform(ENEMY *e)
{
	short tile_x;

	if(e->direction == LEFT) tile_x = e->e.tile_x0;
	else tile_x = e->e.tile_x1;

	if(get_prop_data(tile_x, e->e.tile_y1) == WAYPOINT ||
	tile_x < 0 || tile_x >= glbs->current_map.width) {
		e->hit_horiz_wall = TRUE;
		return TRUE;
	} else
		return FALSE;
}

void ai_face_player(ENEMY *e)
{
	if((glbs->player.e.x < e->e.x && (e->e.gravity == DOWN || e->e.gravity == RIGHT)) ||
		(glbs->player.e.x > e->e.x && (e->e.gravity == UP || e->e.gravity == LEFT)))
			e->direction = LEFT;
	else
		e->direction = RIGHT;
}

void ai_reverse(ENEMY *e)
{
	if(e->direction == LEFT) e->direction = RIGHT;
	else e->direction = LEFT;
	e->path_horiz_direction = e->direction;
}

char ai_facing_player(ENEMY *e)
{
	return ((e->direction == LEFT && glbs->player.e.x < e->e.x) ||
		(e->direction == RIGHT && glbs->player.e.x > e->e.x));
}

char ai_near_player(ENEMY *e, short disx, short disy)
{
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short ex = e->e.x + e->e.x_off;
	short ey = e->e.y + e->e.y_off;
	short xdis = px - ex;
	short ydis = py - ey;

	if(px < ex) xdis = -xdis - glbs->player.e.width;
	else if(px > ex) xdis = xdis - e->e.width;

	if(py < ey) ydis = -ydis - glbs->player.e.hieght;
	else if(py > ey) ydis = ydis - e->e.hieght;

	return (xdis < disx && ydis < disy);
}

char ai_aligned_player(ENEMY *e)
{
	return
		(glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght > e->e.y + e->e.y_off &&
		glbs->player.e.y + glbs->player.e.y_off < e->e.y + e->e.y_off + e->e.hieght);
}

short ai_player_horiz_dis(ENEMY *e)
{
	short px = glbs->player.e.x + glbs->player.e.x_off;
	short ex = e->e.x + e->e.x_off;
	short dis = px - ex;

	if(px < ex) dis = -dis - glbs->player.e.width;
	else if(px > ex) dis = dis - e->e.width;

	return dis;
}

short ai_player_vert_dis(ENEMY *e)
{
	short py = glbs->player.e.y + glbs->player.e.y_off;
	short ey = e->e.y + e->e.y_off;
	short dis = py - ey;

	if(py < ey) {
		dis += glbs->player.e.hieght;
		if(dis > 0) dis = 0;
	}
	else if(py > ey) {
		dis -= e->e.hieght;
		if(dis < 0) dis = 0;
	}

	return dis;
}

short ai_player_angle(short x, short y)
{
	return get_angle(x, y, glbs->player.e.x + glbs->player.e.x_off + glbs->player.e.width / 2,
		glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght / 2);
}

void enemy_attack_area(ENEMY *e, short y_off, short xdim, short ydim, short damage)
{
	ENTITY *p = &glbs->player.e;
	short x0 = e->e.x + e->e.x_off;
	short y0 = e->e.y + e->e.y_off + y_off;

	if(e->direction == LEFT) x0 -= xdim;
	else x0 += e->e.width;

	if(glbs->player.invinc_duration == 0 &&
		rect_overlap(x0, y0, xdim, ydim, p->x + p->x_off, p->y + p->y_off, p->width, p->hieght)) {
			player_hurt(damage, e->e.x + e->e.x_off, e->e.y + e->e.y_off, FALSE);
	}
}

char pirate_ai(ENEMY *e)
{
	short x = 0, y = 0;

	if(e->action_duration == 0) {

		if(e->action == ACT_PIRATE_LOOK) {
			ai_reverse(e);
		}

		enemy_next_action(e);
	}

	if(e->action == ACT_PIRATE_SHOOT) {
		x = e->e.x + e->e.x_off;
		if(e->action_duration == 76)
			y = e->e.y + e->e.y_off + 10;
		else if(e->action_duration == 66)
			y = e->e.y + e->e.y_off + 15;
		else if(e->action_duration == 40)
			y = e->e.y + e->e.y_off + 20;

		if(y) {
			if(e->direction == RIGHT) x += e->e.width;
			animation_create(x, y, TINY_EXPLODE, NONE);
			shot_create(FALSE, TRUE, x, y, e->direction, SHOT_ENEMY_PLASMA);
		}
		return FALSE;
	}

	if(ai_near_player(e, 24, 5)) ai_face_player(e);

	if(e->action != ACT_PIRATE_SHOOT && e->action != ACT_PIRATE_HIT &&
		ai_player_vert_dis(e) == 0 && ai_facing_player(e)) {
		enemy_set_action(e, ACT_PIRATE_SHOOT);
		return FALSE;
	}

	if(e->action == ACT_PIRATE_WALK && ai_check_platform(e)) {
		enemy_set_action(e, ACT_PIRATE_LOOK);
		return FALSE;
	}

	return FALSE;
}

void pirate_get_hit(ENEMY *e, short damage)
{
	if(e->action == ACT_PIRATE_SHOOT || e->action == ACT_PIRATE_HIT || damage <= 0) return;
	ai_face_player(e);
	enemy_set_action(e, ACT_PIRATE_HIT);
}

char climber_ai(ENEMY *e)
{
	short x = 0, y = 0;
	char dir;

	if(e->action == ACT_CLIMBER_SHOOT && e->action_duration == 36) {
		y = e->e.y + 16;
		if(e->e.gravity == LEFT) {
			x = e->e.x + e->e.width;
			dir = RIGHT;
		} else {
			x = e->e.x;
			dir = LEFT;
		}

		animation_create(x, y, TINY_EXPLODE, NONE);
		shot_create(FALSE, TRUE, x, y, dir, SHOT_ENEMY_PLASMA);
	}

	if(e->action_duration == 0) {
		ai_reverse(e);
		enemy_next_action(e);
		if(e->action == ACT_CLIMBER_SHOOTING_DONE && e->subtype == 1)
			enemy_set_action(e, ACT_CLIMBER_JUMP);
	}

	if(e->action == ACT_CLIMBER_CLIMB && ai_aligned_player(e)) {
		enemy_set_action(e, ACT_CLIMBER_SHOOT);
		return FALSE;
	}

	if(e->action == ACT_CLIMBER_JUMP) {
		if(e->var[0] == 0) {
			if(e->e.gravity == LEFT) entity_set_gravity(&e->e, RIGHT);
			else entity_set_gravity(&e->e, LEFT);
			e->var[0] = 1;
		}
		if(!entity_move(&e->e, DOWN, 300)) {
			enemy_next_action(e);
			e->var[0] = 0;
		}
	}

	return FALSE;
}

char crawler_ai(ENEMY *e)
{
	if(e->action == ACT_CRAWLER_FALL) {
		if(!entity_move(&e->e, DOWN, 150)) enemy_next_action(e);
		return FALSE;
	}

	if(e->action_duration == 0) {
		entity_gravity_rotate_right(&e->e);
		enemy_next_action(e);
	} else if(entity_check_direction(&e->e, DOWN)) {
		entity_gravity_rotate_left(&e->e);
		if(entity_check_direction(&e->e, DOWN)) {
			entity_set_gravity(&e->e, DOWN);
			enemy_set_action(e, ACT_CRAWLER_FALL);
		}
	}

	return FALSE;
}

PROJECTILE_PATH skree_blob = {20, 300, 4, TRUE, TRUE};

void skree_killed(ENEMY *e)
{
	short x = e->e.x + e->e.x_off + e->e.width / 2;
	short y = e->e.y + e->e.y_off + 1;
	enemy_create_path(x, y, LEFT, ENEMY_SKREEBLOB, &skree_blob, 30, 140);
	enemy_create_path(x, y ,LEFT, ENEMY_SKREEBLOB, &skree_blob, 15, 120);
	enemy_create_path(x, y, RIGHT, ENEMY_SKREEBLOB, &skree_blob, 30, 140);
	enemy_create_path(x, y, RIGHT, ENEMY_SKREEBLOB, &skree_blob, 15, 120);
}

char skree_ai(ENEMY *e)
{
	if(e->action_duration == 0) enemy_next_action(e);

	if(e->var[0]) {
		e->var[0]--;
		e->e.y++;

		if(e->var[0] == 3) skree_killed(e);
		else if(e->var[0] == 0) return TRUE;

	} else	if(e->action == ACT_SKREE_DIVE) {
		if(!entity_move(&e->e, UP, 300)) {
			e->var[0] = 20;
		} else {
			entity_move(&e->e, e->direction, 50);
		}
	} else if(e->action == ACT_SKREE_HANG && ai_player_horiz_dis(e) < 18) {
		enemy_next_action(e);
		ai_face_player(e);
	}

	return FALSE;
}

PROJECTILE_PATH hopper_path = {20, 200, 6, TRUE, TRUE};

char hopper_ai(ENEMY *e)
{
	if(e->action_duration == 0) {
		enemy_next_action(e);
		if(e->action == ACT_HOPPER_HOP) {
			ai_face_player(e);
			enemy_set_path(e, &hopper_path, e->direction, 80, (random(2) ? 180 : 150));
		}
	}

	return FALSE;
}

PROJECTILE_PATH chozo_path = {50, 200, 3, FALSE, TRUE};
//var[0] : Counts down until ready to stand up
//var[1] : Number of shots to fire.  Set to -1 after firing shots, reset to zero after player gets
// close to enemy again
//var[2] : Counts up while player near enemy, enemy will swipe if reaches 15.  Resets if player gets
// too far away
char chozo_ai(ENEMY *e)
{
	short horiz_dis = ai_player_horiz_dis(e);
	short xpos;
	short ypos;
	short i;

	//goto next action
	if(e->action_duration == 0) {
		if(e->action == ACT_CHOZO_SHOOT) {
			e->var[1]--;
			if(e->var[1] == 0) { enemy_next_action(e); e->var[1] = -1; }
			else enemy_set_action(e, ACT_CHOZO_SHOOT);
		} else
			enemy_next_action(e);
	}

	if(e->action == ACT_CHOZO_SIT) {
		//get ready to stand up...
		if(e->var[0] == 0 && ai_near_player(e, 36, 36)) e->var[0] = 140;
		if(e->var[0]) e->var[0]--;
		if(e->var[0] == 1) enemy_next_action(e);
		return FALSE;
	} else if(e->action == ACT_CHOZO_BREATH) {
		//shoot orbs...
		xpos = e->e.x + 22;
		ypos = e->e.y + 38;
		if(e->direction == RIGHT) xpos += 28;
		if(e->action_duration <= 85 && e->action_duration >= 35)
			if(!((e->action_duration - 35) & 7)) enemy_create(xpos, ypos, e->direction, ENEMY_NUKEBALL);
	} else if(e->action == ACT_CHOZO_SHOOT && e->action_duration == 35) {
		//shoot shockwaves...
		xpos = e->e.x + 24;
		ypos = e->e.y + 48 + 24 * (e->var[1] & 1);
		if(e->direction == RIGHT) xpos += 28;
		shot_create(FALSE, FALSE, xpos, ypos, e->direction, SHOT_ENEMY_SHOCKWAVE);
	} else if(e->action == ACT_CHOZO_SWIPE && e->action_duration <= 25) {
		//do damage with swipe...
		enemy_attack_area(e, 16, 16, 76, 25);
	} else if(e->action <= ACT_CHOZO_WAIT_LEFT) {

		//init swipe counter
		if(ai_near_player(e, 16, 6) && e->var[2] == 0) e->var[2] = 1;

		//process swipe_counter
		if(e->var[2] > 0) {
			e->var[2]++;
			if(!ai_near_player(e, 12, 6)) e->var[2] = 0;
			if(e->var[2] == 15) {
				ai_face_player(e);
				enemy_set_action(e, ACT_CHOZO_SWIPE);
				e->var[2] = -1;
			}
		} else if(e->var[2] < 0 && !ai_near_player(e, 12, 6)) e->var[2] = 0;

		//if gets too far away...
		if(horiz_dis > 60) {
			ai_face_player(e);
			i = random(2);
			if((i == 0 || horiz_dis > 108) && e->var[1] != -1) {
				enemy_set_action(e, ACT_CHOZO_SHOOT);
				e->var[1] = 6;
			} else {
				enemy_set_action(e, ACT_CHOZO_JUMP);
				enemy_set_path(e, &chozo_path, e->direction, 100, 200);
			}
		} else if(e->var[1] == -1 && horiz_dis <= 60) e->var[1] = 0;

		//if hits wall...
		if(e->hit_horiz_wall) {
			i = random(2);
			if(i == 0 || horiz_dis > 48) {
				enemy_set_action(e, ACT_CHOZO_REVERSE);
				ai_reverse(e);
			} else {
				enemy_set_action(e, ACT_CHOZO_JUMP);
				if(e->direction == LEFT) i = RIGHT;
				else i = LEFT;
				enemy_set_path(e, &chozo_path, i, 100, 200);
			}
		}

	}

	return FALSE;
}

void chozo_get_hit(ENEMY *e, short damage)
{
	if(e->action < ACT_CHOZO_REVERSE && !ai_facing_player(e) && damage > 0) {
			enemy_set_action(e, ACT_CHOZO_REVERSE);
			ai_reverse(e);
	}
}

char spikey_ai(ENEMY *e)
{
	if(e->action == ACT_SPIKEY_FALL) {
		if(!entity_move(&e->e, UP, 250)) {
			enemy_next_action(e);
			entity_set_gravity(&e->e, DOWN);
			entity_move(&e->e, DOWN, 500);
		}
		return FALSE;
	}

	if(e->e.gravity == UP && ai_player_horiz_dis(e) < 4 && e->e.y < glbs->player.e.y) {
		enemy_set_action(e, ACT_SPIKEY_FALL);
		entity_set_gravity(&e->e, UP);
		return FALSE;
	}

	if(e->action_duration == 0) {
		entity_gravity_rotate_right(&e->e);
		enemy_next_action(e);
	} else if(entity_check_direction(&e->e, DOWN)) {
		entity_gravity_rotate_left(&e->e);
		enemy_next_action(e);
		if(entity_check_direction(&e->e, DOWN)) {
			entity_set_gravity(&e->e, UP);
			enemy_set_action(e, ACT_SPIKEY_FALL);
		}
	}

	return FALSE;
}

char ripper_ai(ENEMY *e)
{
	if(e->action_duration == 0) {
		if(e->action == ACT_RIPPER_FLY) enemy_next_action(e);
		else if(e->action == ACT_RIPPER_TURN && e->var[0] == 0) {
			ai_reverse(e);
			enemy_set_action(e, ACT_RIPPER_TURN);
			e->var[0] = 1;
		} else {
			enemy_next_action(e);
			e->var[0] = 0;
		}
	}

	return FALSE;
}

ENTITY_ANIMATION acid_wings = {4, 0, {13, 2, 14, 2, 15, 2, 14, 2}};
ENTITY_ANIMATION acid_spurt = {5, 0, {16, 4, 17, 4, 18, 4, 19, 4, 20, 4}};

void acid_init(ENEMY *e)
{
	enemy_set_extra_anim(e, &acid_wings);
}

char acid_ai(ENEMY *e)
{
	short a, b;

	if(e->action < ACT_ACID_FLY) {
		if(e->action_duration == 0) {
			enemy_next_action(e);
			if(e->action == ACT_ACID_HOP_START) ai_face_player(e);
			if(e->action == ACT_ACID_HOP)
				enemy_set_path(e, &hopper_path, e->direction, 90, (random(2) ? 160 : 130));
		}
		if(e->action == ACT_ACID_STAND && ai_facing_player(e) &&
			ai_player_vert_dis(e) == 0 && ai_player_horiz_dis(e) < 48) {
				enemy_set_action(e, ACT_ACID_SHOOT);
		} else if(e->action == ACT_ACID_SHOOT) {
			if(e->action_duration == 54) enemy_set_extra_anim(e, &acid_spurt);
			else if(e->action_duration == 42) {
				short x = e->e.x;
				if(e->direction == RIGHT) x += 58;
				else x += 8;
				shot_create(FALSE, TRUE, x, e->e.y + 21, e->direction, SHOT_ENEMY_ACID);
			} else if(e->action_duration == 34) enemy_set_extra_anim(e, NULL);
		}
		return FALSE;
	}

	if(e->action == ACT_ACID_SWOOP) {
		entity_move(&e->e, DOWN, 150);
		entity_move(&e->e, e->direction, e->var[2]);
		e->var[1]--;
		if(e->var[1] <= 0) enemy_next_action(e);
		return FALSE;
	}

	if(e->action_duration == 0) {
		ai_reverse(e);
		enemy_next_action(e);
	}

	if(e->var[0] == 0) {
		a = ai_player_horiz_dis(e);
		b = ai_player_vert_dis(e);
		if(b > 6 && b < 108 && a < 72 && ai_facing_player(e)) {
			e->var[1] = b + 6;
			e->var[2] = (a * 150) / e->var[1];
			if(e->var[2] > 250) e->var[2] = 250;
			enemy_set_action(e, ACT_ACID_SWOOP);
			return FALSE;
		}
		a = random(2);
		if(a == 0) e->var[0] = -36 - random(12);
		else e->var[0] = 24 + random(24);
	} else if(e->var[0] < 0) {
		e->var[0]++;
		if(!entity_move(&e->e, UP, 30)) e->var[0] = 48;
	} else {
		e->var[0]--;
		if(!entity_move(&e->e, DOWN, 30)) e->var[0] = -48;
	}

	return FALSE;
}

void acid_get_hit(ENEMY *e, short damage)
{
	if(e->action >= ACT_ACID_FLY && e->hp < 6 && damage > 0) {
		enemy_set_action(e, ACT_ACID_HOP);
		enemy_set_path(e, &hopper_path, e->direction, 80, 20);
		e->path_vert_direction = DOWN;
		enemy_set_extra_anim(e, NULL);
	}
}

char plant_ai(ENEMY *e)
{
	if(e->action_duration == 0) {
		if(e->action == ACT_PLANT_WALK) ai_reverse(e);
		enemy_next_action(e);
	}
	if(ai_check_platform(e)) ai_reverse(e);

	if(e->action == ACT_PLANT_SHOOT && e->action_duration == 12) {
		short x = e->e.x + e->e.x_off + (e->e.width >> 1);
		short y = e->e.y + e->e.y_off + (e->e.hieght >> 1);
		shot_create(FALSE, TRUE, x, y, LEFT, SHOT_ENEMY_SPIKE);
		shot_create(FALSE, TRUE, x, y, RIGHT, SHOT_ENEMY_SPIKE);
		shot_create(FALSE, TRUE, x, y, UP, SHOT_ENEMY_SPIKE);
		shot_create(FALSE, TRUE, x, y, LEFT | UP, SHOT_ENEMY_SPIKE);
		shot_create(FALSE, TRUE, x, y, RIGHT | UP, SHOT_ENEMY_SPIKE);
		return FALSE;
	} else if(e->action == ACT_PLANT_SHOOT) return FALSE;

	e->var[0]--;
	if(random(e->var[0]) <= 0) {
		enemy_set_action(e, ACT_PLANT_SHOOT);
		e->var[0] = 400;
	}

	return FALSE;
}

void ship_init(ENEMY *e)
{
	static unsigned char data[60] = {
		EMPTY,EMPTY,EMPTY,SLOPE_RIGHT_UP,SLOPE_LEFT_UP,EMPTY,EMPTY,SLOPE_RIGHT_UP,SLOPE_LEFT_UP,EMPTY,EMPTY,EMPTY,
		EMPTY,SLOPE_RIGHT_UP,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SLOPE_LEFT_UP,EMPTY,
		SLOPE_RIGHT_UP,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SLOPE_LEFT_UP,
		EMPTY,SLOPE_RIGHT_DOWN,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SOLID,SLOPE_LEFT_DOWN,EMPTY,
		EMPTY,EMPTY,EMPTY,EMPTY,SLOPE_RIGHT_DOWN,SOLID,SOLID,SLOPE_LEFT_DOWN,EMPTY,EMPTY,EMPTY,EMPTY};
	short x, y;

	for(x = 0 ; x < 12 ; x++) {
		for(y = 0 ; y < 5 ; y++) {
			glbs->current_map.prop_data[(e->e.tile_y0 + y + 1) * glbs->current_map.width + e->e.tile_x0 + x] = data[y * 12 + x];
		}
	}
}

char ship_ai(ENEMY *e)
{
	if(e->var[0] <= 0) e->var[1] = DOWN;
	else if(e->var[0] >= 2) e->var[1] = UP;

	entity_move(&e->e, e->var[1], 8);

	e->var[0] += e->e.y_moved;

	return FALSE;
}

char robot_ai(ENEMY *e)
{
	char platform = ai_check_platform(e);

	if(e->action == ACT_ROBOT_BACKUP)
		if(!entity_move(&e->e, (e->direction == LEFT ? RIGHT : LEFT), 30)) {
			e->action_duration = 0;
			e->hit_horiz_wall = TRUE;
		}

	if(platform || e->action_duration == 0) {
		if(e->hit_horiz_wall) ai_reverse(e);
		if(e->action == ACT_ROBOT_WALK)
			e->action_duration = -1;
		else enemy_next_action(e);
	}

	if(e->var[0] == 0 && ai_facing_player(e)) {
		short x = e->e.x + e->e.x_off + e->e.width / 2;
		short y = e->e.y + e->e.y_off;
		if(!platform) enemy_set_action(e, ACT_ROBOT_BACKUP);
		shot_create_directed(x, y,
			glbs->player.e.x + glbs->player.e.x_off + glbs->player.e.width / 2,
			glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght / 2, SHOT_ENEMY_WAVE);
		e->var[0] = 200;
	}
	if(e->action == ACT_ROBOT_WALK && e->var[0]) e->var[0]--;

	return FALSE;
}

ENTITY_ANIMATION ridley_wings = {10, 0, {9, 2, 10, 2, 11, 2, 12, 2, 13, 2, 14, 2, 13, 2, 12, 2, 11, 2, 10, 2}};
ENTITY_ANIMATION ridley_wings_still = {1, 0, {9, 1}};
#define RIDLEY_X_SPEED 0
#define RIDLEY_Y_SPEED 1
#define RIDLEY_STATE 2
#define RIDLEY_LAST_STATE 3
#define RIDLEY_REPEAT 4
#define RIDLEY_FIREBALL_DELAY 5
#define RIDLEY_FIREBALL_ANGLE 6
#define RIDLEY_DURATION 7

enum RIDLEY_STATES {
	RS_IDLE,
	RS_FLY_VERT,
	RS_FLY_ACROSS,
	RS_CHARGE_ACROSS,
	RS_CHARGE_BACK,
	RS_BOUNCE_DOWN,
	RS_BOUNCE_UP,
	RS_CHILL,
};

TAIL_DATA ridley_tail_data[] = {
	{NULL, 0, 8, RTAIL_IDLE},
	{NULL, 0, 2, RTAIL_IDLE},
	{NULL, 0, 0, 0},
	{NULL, 0, 0, 0},
};

void ridley_set_tail(ENEMY *e, short type)
{
	e->tail_type = type;
	e->tail_frame = 0;
	e->tail_counter = 0;
}

void ridley_process_tail(ENEMY *e)
{
	TAIL_DATA *tail = ridley_tail_data + e->tail_type;
	short x = e->e.x - 92, y = e->e.y - 86;

	if(e->direction == RIGHT) x += 255 - 11 - tail->frames[e->tail_frame].xpos[6];
	else x += tail->frames[e->tail_frame].xpos[6];
	y += tail->frames[e->tail_frame].ypos[6];

	if(rect_overlap(x, y, 12, 12,
	glbs->player.e.x + glbs->player.e.x_off, glbs->player.e.y + glbs->player.e.y_off,
	glbs->player.e.width, glbs->player.e.hieght)) player_hurt(120, x + 6, y + 6, FALSE);

	e->tail_counter++;
	if(e->tail_counter >= tail->speed) {
		e->tail_counter = 0;
		e->tail_frame++;
		if(e->tail_frame == tail->frame_number) ridley_set_tail(e, tail->next);
	}
}

void ridley_draw_tail(ENEMY *e)
{
	TAIL_DATA *tail = ridley_tail_data + e->tail_type;
	short i;
	short x = e->e.x - 92, y = e->e.y - 86;
	short x0, y0;
	short type = e->type;
	SPRITE_HEADER *header = enemy_gfx[type].header + 16;
	unsigned char tail_convert[] = {0, 7, 6, 5, 4, 3, 2, 1};

	if(e->direction == RIGHT) x += 255 - 11;

	for(i = 0 ; i < 7 ; i++) {
		y0 = y + tail->frames[e->tail_frame].ypos[i];
		if(e->direction == RIGHT) x0 = x - tail->frames[e->tail_frame].xpos[i];
		else x0 = x + tail->frames[e->tail_frame].xpos[i];

		GraySpriteClip16_MASK(x0 - glbs->camera.x, y0 - glbs->camera.y, header->hieght,
			(unsigned short *)(enemy_gfx[type].gfx + header->offset),
			glbs->light_buffer, glbs->dark_buffer);

		if(i == 5) {
			header = enemy_gfx[type].header + 18;
			if(e->direction == LEFT) header += tail->frames[e->tail_frame].spike_frame;
			else header += tail_convert[tail->frames[e->tail_frame].spike_frame];
		}

		if((i & 1) != 0) header++;
	}
}

void ridley_face_player(ENEMY *e)
{
	if(e->action == ACT_RIDLEY_FLY && !ai_facing_player(e)) {
		enemy_set_action(e, ACT_RIDLEY_REVERSE);
		ai_reverse(e);
	}
}

char ridley_ai(ENEMY *e)
{
	short ex0 = e->e.x + e->e.x_off, ex1 = ex0 + e->e.width - 1;
	short ey0 = e->e.y + e->e.y_off, ey1 = ey0 + e->e.hieght - 1;
	short px0 = glbs->player.e.x + glbs->player.e.x_off, px1 = px0 + glbs->player.e.width - 1;
	short py0 = glbs->player.e.y + glbs->player.e.y_off, py1 = py0 + glbs->player.e.hieght - 1;
	short horiz_dis, vert_dis, r, x, y;
	char hit_vert = 0, hit_horiz = 0;

	if(e->action_duration == 0) {
		if(e->action == ACT_RIDLEY_REVERSE) enemy_set_extra_anim(e, &ridley_wings);
		enemy_next_action(e);
		if(e->action == ACT_RIDLEY_BREATH_INTRO) enemy_set_extra_anim(e, &ridley_wings_still);
		else if(e->action == ACT_RIDLEY_TAKE_OFF) enemy_set_extra_anim(e, &ridley_wings);
	}
	if(e->action == ACT_RIDLEY_TAKE_OFF) {
		entity_move(&e->e, UP, 100);
		e->var[RIDLEY_STATE] = RS_IDLE;
		e->var[RIDLEY_LAST_STATE] = RS_IDLE;
		//e->var[RIDLEY_FIREBALL_DELAY] = 0;
	} else if(e->action >= ACT_RIDLEY_FLY) {
		horiz_dis = ai_player_horiz_dis(e);
		vert_dis = ai_player_vert_dis(e);
		if(e->var[RIDLEY_STATE] == RS_IDLE) {
			r = random(100);
			if(vert_dis > 0 && e->var[RIDLEY_REPEAT] < 3) {
				e->var[RIDLEY_STATE] = RS_BOUNCE_DOWN;
				if(e->var[RIDLEY_REPEAT] == 0) {
					if(ex0 > px0) e->var[RIDLEY_X_SPEED] = -40;
					else e->var[RIDLEY_X_SPEED] = 40;
				}
				e->var[RIDLEY_Y_SPEED] = 100;
			} else if(vert_dis < 0) {
				e->var[RIDLEY_STATE] = RS_FLY_VERT;
				if(ex0 > px0) e->var[RIDLEY_X_SPEED] = -25;
				else e->var[RIDLEY_X_SPEED] = 25;
				e->var[RIDLEY_Y_SPEED] = -100;
			} else if(horiz_dis > 12 && e->var[RIDLEY_LAST_STATE] != RS_CHARGE_ACROSS &&
			e->var[RIDLEY_LAST_STATE] != RS_FLY_ACROSS) {
				ridley_face_player(e);
				if(r < 30) {
					e->var[RIDLEY_STATE] = RS_CHARGE_ACROSS;
					e->var[RIDLEY_X_SPEED] = 200;
				} else {
					e->var[RIDLEY_STATE] = RS_FLY_ACROSS;
					e->var[RIDLEY_X_SPEED] = 110;
				}
				if(e->direction == LEFT) e->var[RIDLEY_X_SPEED] *= -1;
				e->var[RIDLEY_Y_SPEED] = 0;
			} else { //If we can't find anything better to do...
				ridley_face_player(e);
				if(r < 50) e->var[RIDLEY_Y_SPEED] = 100;
				else e->var[RIDLEY_Y_SPEED] = -100;
				e->var[RIDLEY_X_SPEED] = 0;
				e->var[RIDLEY_STATE] = RS_CHILL;
				e->var[RIDLEY_DURATION] = random(25) + 25;
				e->var[RIDLEY_LAST_STATE] = RS_IDLE; //clear out repeat counter
			}

			if(e->var[RIDLEY_STATE] == e->var[RIDLEY_LAST_STATE]) e->var[RIDLEY_REPEAT]++;
			else e->var[RIDLEY_REPEAT] = 0;
			e->var[RIDLEY_LAST_STATE] = e->var[RIDLEY_STATE];
		}

		if(e->var[RIDLEY_Y_SPEED] < 0) hit_vert = !entity_move(&e->e, UP, -e->var[RIDLEY_Y_SPEED]);
		else if(e->var[RIDLEY_Y_SPEED] > 0) hit_vert = !entity_move(&e->e, DOWN, e->var[RIDLEY_Y_SPEED]);

		if(e->var[RIDLEY_X_SPEED] < 0) hit_horiz = !entity_move(&e->e, LEFT, -e->var[RIDLEY_X_SPEED]);
		else if(e->var[RIDLEY_X_SPEED] > 0) hit_horiz = !entity_move(&e->e, RIGHT, e->var[RIDLEY_X_SPEED]);

		if(e->var[RIDLEY_STATE] == RS_FLY_VERT) {
			//if(e->var[RIDLEY_Y_SPEED] < 0 && e->var[RIDLEY_Y_SPEED] > -100) e->var[RIDLEY_Y_SPEED]--;
			//else if(e->var[RIDLEY_Y_SPEED] > 0 && e->var[RIDLEY_Y_SPEED] < 100) e->var[RIDLEY_Y_SPEED]++;

			if(e->var[RIDLEY_X_SPEED] < 0 && e->var[RIDLEY_X_SPEED] > -100) e->var[RIDLEY_X_SPEED] -= 2;
			else if(e->var[RIDLEY_X_SPEED] > 0 && e->var[RIDLEY_X_SPEED] < 100) e->var[RIDLEY_X_SPEED] +=2;

			if(hit_vert == 0 || vert_dis == 0) e->var[RIDLEY_STATE] = RS_IDLE;
		} else if(e->var[RIDLEY_STATE] == RS_FLY_ACROSS) {
			if(vert_dis > 0 && e->var[RIDLEY_Y_SPEED] < 25) e->var[RIDLEY_Y_SPEED] += 2;
			else if(vert_dis < 0 && e->var[RIDLEY_Y_SPEED] > -25) e->var[RIDLEY_Y_SPEED] -= 2;
			if(hit_horiz) {
				e->var[RIDLEY_STATE] = RS_IDLE;
				ridley_face_player(e);
			}
		} else if(e->var[RIDLEY_STATE] == RS_CHARGE_ACROSS) {
			if(e->direction == LEFT && e->var[RIDLEY_X_SPEED] > -400) e->var[RIDLEY_X_SPEED] -= 12;
			else if(e->direction == RIGHT && e->var[RIDLEY_X_SPEED] < 400) e->var[RIDLEY_X_SPEED] += 12;
			if(hit_horiz) {
				e->var[RIDLEY_X_SPEED] *= -1;
				e->var[RIDLEY_STATE] = RS_CHARGE_BACK;
			}
		} else if(e->var[RIDLEY_STATE] == RS_CHARGE_BACK) {
			if(e->direction == RIGHT && e->var[RIDLEY_X_SPEED] < -50) e->var[RIDLEY_X_SPEED] += 24;
			else if(e->direction == LEFT && e->var[RIDLEY_X_SPEED] > -50) e->var[RIDLEY_X_SPEED] -= 24;
			if(hit_horiz || abs(e->var[RIDLEY_X_SPEED]) < 50) e->var[RIDLEY_STATE] = RS_IDLE;
		} else if(e->var[RIDLEY_STATE] == RS_BOUNCE_DOWN) {
			if(e->var[RIDLEY_Y_SPEED] < 250) e->var[RIDLEY_Y_SPEED] += 6;
			if(hit_vert) {
				e->var[RIDLEY_STATE] = RS_BOUNCE_UP;
				e->var[RIDLEY_Y_SPEED] *= -1;
			}
		} else if(e->var[RIDLEY_STATE] == RS_BOUNCE_UP) {
			if(e->var[RIDLEY_Y_SPEED] < -50) e->var[RIDLEY_Y_SPEED] += 6;
			else e->var[RIDLEY_STATE] = RS_IDLE;
		} else if(e->var[RIDLEY_STATE] == RS_CHILL) {
			e->var[RIDLEY_DURATION]--;
			if(e->var[RIDLEY_DURATION] <= 0 || hit_vert) e->var[RIDLEY_STATE] = RS_IDLE;
		} else
			e->var[RIDLEY_STATE] = RS_IDLE;

		if(e->var[RIDLEY_FIREBALL_DELAY] == 0 && ai_facing_player(e) &&
		e->action == ACT_RIDLEY_FLY && horiz_dis > 12) {
			//short tx = px0 + glbs->player.e.width / 2, ty = py0 + glbs->player.e.hieght / 2;
			x = e->e.x + 21;
			y = e->e.y + 24;

			if(e->direction == RIGHT) x += 31;
			e->var[RIDLEY_FIREBALL_ANGLE] = ai_player_angle(x, y);
			e->var[RIDLEY_FIREBALL_DELAY] = 125;
			enemy_set_action(e, ACT_RIDLEY_BREATH);
		} else if(e->var[RIDLEY_FIREBALL_DELAY] != 0) e->var[RIDLEY_FIREBALL_DELAY]--;

		if(e->action == ACT_RIDLEY_BREATH) {
			if(e->action_duration == 44 - 12 || e->action_duration == 44 - 20 ||
			e->action_duration == 44 - 28 || e->action_duration == 44 - 36) {
				x = e->e.x + 21;
				y = e->e.y + 24;
				if(e->direction == RIGHT) x += 31;
				shot_create_angle(x, y, e->var[RIDLEY_FIREBALL_ANGLE], SHOT_ENEMY_FIREBALL);
			}
		}

		if(e->tail_type == NONE)
			ridley_set_tail(e, RTAIL_IDLE);
		else if(e->tail_type == RTAIL_IDLE && horiz_dis < 24 && vert_dis > 6 && ai_facing_player(e))
			ridley_set_tail(e, RTAIL_SWIPE);

		ridley_process_tail(e);
	}

	if(e->action == ACT_RIDLEY_REVERSE) enemy_set_extra_anim(e, NULL);

	return FALSE;
}

//e->var[0] = jump hieght
//e->var[1] = xspeed
//e->var[2] = yspeed
//e->var[3] = falling state
//	0: <--- decel
//	1: ---> accel
//	2: ---> decel
//	3: <--- accel
#define FLOATER_JUMP_HIEGHT 0
#define FLOATER_XSPEED 1
#define FLOATER_YSPEED 2
#define FLOATER_FALL_STATE 3
char floater_ai(ENEMY *e)
{
	if(ai_player_horiz_dis(e) <= 36 && e->action == ACT_FLOATER_WAIT) {
		enemy_next_action(e);
		e->var[FLOATER_JUMP_HIEGHT] = e->e.y - 80;
		e->var[FLOATER_YSPEED] = 200;
	} else if(e->action == ACT_FLOATER_JUMP) {
		if(!entity_move(&e->e, UP, e->var[FLOATER_YSPEED]) || e->e.y <= e->var[FLOATER_JUMP_HIEGHT]) {
			enemy_next_action(e);
			e->var[FLOATER_XSPEED] = 100;
			e->var[FLOATER_YSPEED] = 50;
			e->var[FLOATER_FALL_STATE] = 0;
		}
		else if(e->e.y < e->var[FLOATER_JUMP_HIEGHT] + 50) e->var[FLOATER_YSPEED]--;
	} else if(e->action == ACT_FLOATER_FALL) {
		if(!entity_move(&e->e, DOWN, e->var[FLOATER_YSPEED])) enemy_next_action(e);
		entity_move(&e->e, e->direction, e->var[FLOATER_XSPEED]);
		e->var[FLOATER_YSPEED] -= 3;
		if(e->var[FLOATER_YSPEED] < 0) e->var[FLOATER_YSPEED] = 0;
		if(e->var[FLOATER_FALL_STATE] == 0 || e->var[FLOATER_FALL_STATE] == 2) {
			e->var[FLOATER_XSPEED] -= 4;
			if(e->var[FLOATER_XSPEED] == 0) {
				e->var[FLOATER_FALL_STATE]++;
				e->var[FLOATER_YSPEED] = 50;
				ai_reverse(e);
			}
		} else {
			e->var[FLOATER_XSPEED] += 4;
			if(e->var[FLOATER_XSPEED] == 100) {
				e->var[FLOATER_FALL_STATE]++;
				e->var[FLOATER_YSPEED] = 50;
				if(e->var[FLOATER_FALL_STATE] == 4) e->var[FLOATER_FALL_STATE] = 0;
			}
		}
	}

	return FALSE;
}

#define KRAID_ARMOR DAMAGE_NORMAL | DAMAGE_BOMB | DAMAGE_SCREW | DAMAGE_CHARGE_SCREW | DAMAGE_SPEED
//because of these, there can only be one kraid per map
ENEMY *kraid_arm, *kraid_foot, *kraid_body;
const short launcher_y_off[] = {41, 85, 135};
const short launcher_x_off[] = {20, 1, 3};

char kraid_move(ENEMY *e, char dir, short speed)
{
	entity_move(&kraid_arm->e, dir, speed);
	entity_move(&kraid_foot->e, dir, speed);
	entity_move(&kraid_body->e, dir, speed);
	return entity_move(&e->e, dir, speed);
}

void kraid_init(ENEMY *e)
{
	short ex = e->e.x;
	short ey = e->e.y;

	kraid_arm = enemy_create(ex - 42 + 15, ey + 48 + 24, e->direction, ENEMY_KRAID_ARM);
	kraid_foot = enemy_create(ex - 72 + 54, ey + 185 + 27, e->direction, ENEMY_KRAID_FOOT);
	kraid_body = enemy_create(ex - 42 + 56, ey + 66 + 81, e->direction, ENEMY_KRAID_BODY);
}

void kraid_killed(ENEMY *e)
{
	enemy_explode(kraid_arm, 0);
	enemy_explode(kraid_foot, 0);
	enemy_explode(kraid_body, 0);
}

char kraid_check_bounds(char dir)
{
	ENEMY_DATA *d = &enemy_data[kraid_body->type];
	ENTITY *e = &kraid_body->e;
	ENTITY_SIZE *s = &d->size;
	short x0 = (e->x + s->x_off) / 12;
	short x1 = (e->x + s->x_off + s->width - 1) / 12;
	short y = (e->y + s->y_off + s->hieght - 1) / 12;
	
	if(dir == LEFT && get_prop_data(x0, y) == WAYPOINT) return TRUE;
	else if(dir == RIGHT && get_prop_data(x1, y) == WAYPOINT) return TRUE;
	else return FALSE;
}

void kraid_get_hit(ENEMY *e, short damage)
{
	if(damage < 0 && e->var[0] == 0 && e->action == ACT_PARTICLE) {
		enemy_set_action(e, ACT_KRAID_HEAD_BURP);
		e->var[0] = 80;
	}
}

//e->var[0] - roar timer
//spike launcher offsets - 40, 80, 134
char kraid_ai(ENEMY *e)
{
	ENEMY *spike;
	//roar counter
	if(e->var[0] > 0) {
		e->var[0]--;
		if(e->var[0] == 0) {
			enemy_set_action(e, ACT_KRAID_HEAD_ROAR);
			e->immune = KRAID_ARMOR;
		}
	}
	
	//spike launchers
	if(e->var[1] == 0) {
		e->var[1] = 200;
		if(e->var[2] == 2) e->var[2] = 0;
		else e->var[2]++;
		spike = enemy_create(kraid_body->e.x + launcher_x_off[e->var[2]],
			kraid_body->e.y + launcher_y_off[e->var[2]],
			LEFT, ENEMY_KRAID_SPIKE);
		spike->var[2] = e->var[2];
	} else e->var[1]--;
	
	//claw launcher
	if(kraid_arm->frame == 0 && kraid_arm->frame_counter == 0) {	
		if(e->var[3] == 0 && kraid_arm->var[0] < 5) {
			enemy_create(kraid_arm->e.x + 12, kraid_arm->e.y + 4, LEFT, ENEMY_KRAID_CLAW);
			e->var[3] = random(3);
			kraid_arm->var[0]++;
		} else if(e->var[3] > 0) e->var[3]--;
	}

	if(kraid_foot->action == ACT_KRAID_FOOT_IDLE) {
		if(e->direction == RIGHT)
			enemy_set_action(kraid_foot, ACT_KRAID_FOOT_STEP_BACK);
		else
			enemy_set_action(kraid_foot, ACT_KRAID_FOOT_STEP_FORWARD);
	} else if(kraid_foot->action != ACT_KRAID_FOOT_PAUSE) {
		kraid_move(e, e->direction, 50);
		if(kraid_foot->action_duration > 24) kraid_move(e, UP, 20);
		else kraid_move(e, DOWN, 20);
		
		if(kraid_check_bounds(e->direction)) {
			ai_reverse(e);
			enemy_set_action(kraid_foot, ACT_KRAID_FOOT_IDLE);
		}
	}
	
	if(kraid_foot->action_duration == 0) enemy_next_action(kraid_foot);
	if(e->action_duration == 0) {
		enemy_next_action(e);
		e->immune = 0xffff;
	}

	return FALSE;
}

char kraid_spike_ai(ENEMY *e)
{
	if(e->var[0] < 50) {
		entity_set_pos(&e->e, kraid_body->e.x + launcher_x_off[e->var[2]] - e->e.width / 2,
			kraid_body->e.y + launcher_y_off[e->var[2]] - e->e.hieght / 2);
		e->var[0]++;
		e->flash = !e->flash;
	} else {
		if(e->var[0] == 50) { 
			e->var[1] = block_create(e->e.x + e->e.x_off + 1, e->e.y + e->e.y_off,
				e->e.width - 2, 1);//e->e.hieght - 1);
			e->var[0]++;
		}
		entity_move(&e->e, e->direction, 100);
		block_move(e->var[1], e->direction, 100);
		e->flash = 0;
	}
	return (e->e.x < 0 || e->e.y < 0);
}

void kraid_spike_killed(ENEMY *e)
{
	block_delete(e->var[1]);
}

char kraid_claw_ai(ENEMY *e)
{
	if(e->var[0] == 0) {
		if(random(2) == 0) e->direction |= UP;
		else e->direction |= DOWN;
		e->var[0]++;
	} else if(e->var[0] == 500) enemy_explode(e, 0);
	else e->var[0]++;
	
	if(!entity_move(&e->e, e->direction & ~(LEFT|RIGHT), 75)) {
		if(e->direction & UP) e->direction = ((e->direction & ~UP) | DOWN);
		else e->direction = ((e->direction & ~DOWN) | UP);
	}
	if(!entity_move(&e->e, e->direction & ~(UP|DOWN), 75)) {
		if(e->direction & LEFT) e->direction = ((e->direction & ~LEFT) | RIGHT);
		else e->direction = ((e->direction & ~RIGHT) | LEFT);
	}
	return FALSE;
}

void kraid_claw_killed(ENEMY *e)
{
	kraid_arm->var[0]--;
}

ENTITY_ANIMATION swooper_wings = {2, 0, {4, 6, 5, 6}};

void swooper_init(ENEMY *e)
{
	enemy_set_extra_anim(e, &swooper_wings);
}

char swooper_ai(ENEMY *e)
{
	if(e->action_duration == 0) {
		if(e->action == ACT_SWOOPER_PAUSE) ai_face_player(e);	
		enemy_next_action(e);
	} else if(e->action == ACT_SWOOPER_DOWN) {
		entity_move(&e->e, e->direction, 50);
		if(!entity_move(&e->e, DOWN, 200)) enemy_next_action(e);
	} else if(e->action == ACT_SWOOPER_UP) {
		entity_move(&e->e, e->direction, 50);
		if(!entity_move(&e->e, UP, 200)) enemy_next_action(e);
	}
	return FALSE;
}

char pipespawn_ai(ENEMY *e)
{
	ENEMY *spawn;
	
	if(e->var[1] > 0) e->var[1]--;
	if(e->var[0] == 0 && e->var[1] == 0 && ai_near_player(e, 48, 48)) {
		spawn = enemy_create(e->e.x + 6, e->e.y + 6, 0, ENEMY_PIPEGUY0 + e->subtype);
		spawn->var[0] = e->index;
		
		if(e->e.gravity == DOWN) spawn->var[1] = UP;
		else if(e->e.gravity == UP) spawn->var[1] = DOWN;
		else if(e->e.gravity == LEFT) spawn->var[1] = RIGHT;
		else spawn->var[1] = LEFT;
		
		if(e->e.gravity == UP || e->e.gravity == DOWN) ai_face_player(spawn);
		else spawn->direction = spawn->var[1];
		
		e->var[0] = 1;
		e->var[1] = 150;
	}
	return FALSE;
}

char pipeguy_ai(ENEMY *e)
{
	short pmid = glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght / 2 - e->e.hieght / 2;
	
	if(e->action < ACT_PIPEGUY0_ATTACK) {
		if(e->var[1] == UP || e->var[1] == DOWN) {
			entity_move(&e->e, e->var[1], 50);
			if((e->var[1] == UP && e->e.y <= pmid) || (e->var[1] == DOWN && e->e.y >= pmid))
				enemy_next_action(e);
		} else {
			entity_move(&e->e, e->var[1], 100);
			if(ai_near_player(e, 24, 24)) {
				e->var[2] = DOWN; e->var[3] = 36;
				enemy_next_action(e);
			}
		}
	} else {
		entity_move(&e->e, e->direction, 100);
		if(e->var[1] == LEFT || e->var[1] == RIGHT) {
			if(e->var[2]) entity_move(&e->e, e->var[2], 100);
			e->var[3]--;
			if(e->var[3] == 0) {
				e->var[3] = 36;
				if(e->var[2] == DOWN) e->var[2] = 0;
				else if(e->var[2] == 0) e->var[2] = UP;
				else e->var[1] = 0;
			}
		}
	}
	
	return FALSE;
}

void pipeguy_killed(ENEMY *e)
{
	if(e->powerup >= 0) powerup_set_signal(e->powerup, e->var[0]);
	else enemies[e->var[0]].var[0]--;
}

char sucker_ai(ENEMY *e)
{
	if(e->action == ACT_SUCKER_WALK) {
		if(ai_near_player(e, 48, 24)) {
			ai_face_player(e);
			e->var[1] = (ai_player_horiz_dis(e) + e->e.width) * 100 / 30;
			enemy_next_action(e);
		} else if(entity_move(&e->e, DOWN, 100)) {
			enemy_set_action(e, ACT_SUCKER_JUMP_DOWN);
		} else if(e->action_duration == 0) {
			ai_reverse(e);
			enemy_set_action(e, ACT_SUCKER_WALK);
		} else if(e->var[2] == 0) {
			if(random(2) == 0) ai_reverse(e);
			else { e->var[0] = 300; enemy_next_action(e); }
			e->var[2] = 20 + random(100);
		} else e->var[2]--;
	} else if(e->action == ACT_SUCKER_JUMP_UP) {
		if(e->var[0] < 10 || !entity_move(&e->e, UP, e->var[0])) enemy_next_action(e);
		else e->var[0] -= 20;
	} else if(e->action == ACT_SUCKER_JUMP_DOWN) {
		if(!entity_move(&e->e, DOWN, e->var[0])) {e->var[1] = 0; enemy_next_action(e); }
		if(e->var[0] < 300) e->var[0] += 20;
	} else if(e->action == ACT_SUCKER_SUCK) {
		e->var[1] = 0;
		entity_set_pos(&e->e, glbs->player.e.x + glbs->player.e.x_off + glbs->player.e.width / 2 - 16,
			glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght / 2 - 8);
		if(e->var[2] == 0) {
			e->var[2] = HP_DRAIN_DELAY;
			glbs->player.hp -= 4;
			bar_update();
			if(!glbs->player.hp_drain) {
				glbs->player.hp_drain = TRUE;
				glbs->player.flash = TRUE;
				glbs->player.hp_drain_counter = HP_DRAIN_DELAY;
			}
		} else e->var[2]--;
	}
	if(e->var[1] > 0) entity_move(&e->e, e->direction, e->var[1]);

	return FALSE;
}

//e->var[0] = y speed
//e->var[1] = y target of jump
//e->var[2] = shot counter
//e->var[3] = shot angle
enum LAVAHORSE_VARIABLES {
	LAVAHORSE_Y_SPEED,
	LAVAHORSE_JUMP_TARGET,
	LAVAHORSE_SHOT_COUNTER,
	LAVAHORSE_SHOT_ANGLE,
};

char lavahorse_walker_ai(ENEMY *e)
{
	short x = e->e.x;
	if(e->direction == RIGHT) x += 29; //x_pos of shooter
	
	if(e->action == ACT_LAVAHORSE_WAIT) {
		if(ai_near_player(e, 48, 48)) {// && ai_player_vert_dis(e) > 0) {
			e->e.simple_movement = TRUE;
			e->var[LAVAHORSE_Y_SPEED] = -500;
			e->var[LAVAHORSE_JUMP_TARGET] = e->e.y - e->e.hieght;
			ai_face_player(e);
			enemy_next_action(e);
		}
	} else if(e->action == ACT_LAVAHORSE_JUMP) {
		if(e->var[LAVAHORSE_Y_SPEED] < 0) entity_move(&e->e, UP, -e->var[LAVAHORSE_Y_SPEED]);
		else entity_move(&e->e, DOWN, e->var[LAVAHORSE_Y_SPEED]);
		if(e->var[LAVAHORSE_Y_SPEED] < 300) e->var[LAVAHORSE_Y_SPEED] += 20;
		if(e->e.y == e->var[LAVAHORSE_JUMP_TARGET]) {
			e->var[LAVAHORSE_Y_SPEED] = 100;
			e->e.simple_movement = FALSE;
			entity_set_pos(&e->e, e->e.x, e->e.y);
			enemy_next_action(e);
		}
	} else if(e->action == ACT_LAVAHORSE_WALK) {
		if(ai_check_platform(e) || e->action_duration == 0) {
			ai_reverse(e);
			enemy_set_action(e, ACT_LAVAHORSE_WALK);
		}
		//try to fall...if can't fall...check for shot
		if(entity_move(&e->e, DOWN, e->var[LAVAHORSE_Y_SPEED])) {
			if(e->var[LAVAHORSE_Y_SPEED] < 300) e->var[LAVAHORSE_Y_SPEED] += 20;
		} else {
			e->var[LAVAHORSE_Y_SPEED] = 100;
			if(ai_near_player(e, 64, 12) && e->var[LAVAHORSE_SHOT_COUNTER] == 0) {
				ai_face_player(e);
				e->var[LAVAHORSE_SHOT_ANGLE] = ai_player_angle(x, e->e.y + 12);
				e->var[LAVAHORSE_SHOT_COUNTER] = 50 + random(50);
				enemy_next_action(e);
			} else if(e->var[LAVAHORSE_SHOT_COUNTER]) e->var[LAVAHORSE_SHOT_COUNTER]--;
		}
	} else if(e->action == ACT_LAVAHORSE_SHOOT) {
		if(e->action_duration < 18 && e->action_duration >= 12 && (e->action_duration & 1)) {
			shot_create_angle(x, e->e.y + 12, e->var[LAVAHORSE_SHOT_ANGLE], SHOT_ENEMY_FIREBLIP);
			e->var[LAVAHORSE_SHOT_ANGLE] += random(30) - 15;
		}
		else if(e->action_duration == 0) enemy_next_action(e);
	}
	return FALSE;
}
		
PROJECTILE_PATH nukeball_path = {50, 300, 4, FALSE, FALSE};
char nukeball_ai(ENEMY *e)
{
	if(e->var[0] == 0) {
		enemy_set_path(e, &nukeball_path, e->direction, random(50) + 70, random(50) + 150);
		e->var[0] = 1;
	}

	if(e->action_duration == 0) {
		short x = e->e.x + 5;
		short y = e->e.y;
		if(e->hit_horiz_wall || e->path_vert_direction == UP)
			animation_create(x, y + 5, SMALL_EXPLODE, NONE);
		if(e->hit_vert_wall) animation_create(x, y - 8, NUKE_EXPLODE, NONE);

		return TRUE;
	} else
	return FALSE;
}

char skree_blob_ai(ENEMY *e)
{
	e++;
	return FALSE;
}

