// C Source File
// Created 6/13/02; 3:40:15 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "utility.h"
#include "bitops.h"           // big-endian 16/32-bit framebuffer access
//#include "extgraph.h"
#include "entity.h"
#include "tiles.h"
#include "enemy.h"
#include "player.h"
#include "shots.h"
#include "door.h"
#include "pipe.h"
#include "bomb.h"
#include "map.h"
#include "animation.h"
#include "clipsprites.h"
#include "miscgfx.h"
#include "globals.h"

#define BOMB_DURATION 45
#define BOMB_ANIM_SPEED 5
#define POWERBOMB_DAMAGE 15
#define BOMB_DAMAGE 4

char bombs_placed;

// m68k 'swap': exchange the high and low 16-bit halves of a 32-bit word.
#define ASM_SWAP(val) (((unsigned long)(val) << 16) | ((unsigned long)(val) >> 16))

void DrawBombStrip(short x1, short x2, short y) {
    static const unsigned short table1[16] = {0xffff,0x7fff,0x3fff,0x1fff,
                                              0x0fff,0x07ff,0x03ff,0x01ff,
                                              0x00ff,0x007f,0x003f,0x001f,
                                              0x000f,0x0007,0x0003,0x0001};
    static const unsigned short table2[16] = {0x0000,0x8000,0xc000,0xe000,
                                              0xf000,0xf800,0xfc00,0xfe00,
                                              0xff00,0xff80,0xffc0,0xffe0,
                                              0xfff0,0xfff8,0xfffc,0xfffe};
    short           dx;
    short           sx;
    unsigned short* p1;
    unsigned short* p2;

    if(y < 0 || y >= glbs->display_hieght) return;

    //if (x1>x2) {
    //    short tmp = x1;
    //    x1 = x2;
    //    x2 = tmp;
    //}

    if(x1 < 0) x1 = 0;
    else if(x1 >= glbs->display_width) x1 = glbs->display_width - 1;

    if(x2 < 0) x2 = 0;
    else if(x2 >= glbs->display_width) x2 = glbs->display_width - 1;

    p1  = (unsigned short*)(glbs->light_buffer+(y<<5)-(y<<1)+((x1>>3)&0x1e));
    p2  = (unsigned short*)(glbs->dark_buffer+(y<<5)-(y<<1)+((x1>>3)&0x1e));

    dx = x2-x1+1;
    if(dx < 1) return;
    sx = x1 & 0x000f;

    // The framebuffer is big-endian words on m68k; route the word writes through
    // LD/ST helpers (raw *(long*) is an 8-byte LE write on the host).
    if (dx<16) {
        unsigned long val = (ASM_SWAP(table2[dx])) >> sx;
        ST32(p1, LD32(p1) ^ val);
        ST32(p2, LD32(p2) & ~val);

        return;
    }

    if (sx) {
        ST16(p1, LD16(p1) ^ table1[sx]);  p1++;
        ST16(p2, LD16(p2) & ~table1[sx]); p2++;
        dx -= (16-sx);
    }
    while (dx >= 16) {
        ST16(p1, LD16(p1) ^ 0xffff);  p1++;
        ST16(p2, LD16(p2) & ~0xffff); p2++;
        dx-=16;
    }
    if (dx) {
    	ST16(p1, LD16(p1) ^ table2[dx]);
    	ST16(p2, LD16(p2) & ~table2[dx]);
    }
}

void DrawClippedEllipse(short xc, short yc, short r)
{
	register short x = 0;
	register short y = r;
	register short p = 3 - 2 * r;

  DrawBombStrip(xc - y * 2, xc + y * 2, yc + x);

	while (x <= y) {
		DrawBombStrip(xc - y * 2, xc + y * 2, yc + x);
		DrawBombStrip(xc - y * 2, xc + y * 2, yc - x);

		if (p < 0) {
			p += 4 * x++ + 6;

		} else {

			if(x != y) {
				DrawBombStrip(xc - x * 2, xc + x * 2, yc + y);
				DrawBombStrip(xc - x * 2, xc + x * 2, yc - y);
			}

			p += 4 * (x++ - y--) + 10;
			}
		}
}

void check_strip(short x0, short x1, short y, short damage_type)
{
	short x;
	if(y < 0 || y >= glbs->current_map.hieght) return;

	for(x = x0 ; x <= x1 ; x++) {
		if(x < 0 || x >= glbs->current_map.width) continue;
		unsigned char data = get_prop_data(x, y);
		if(data >= BREAK_STAND_REFORM && data <= BREAK_SHOOT_BOMB_SWITCH) hole_create(x, y, damage_type);
	}
}

void check_map(short xc, short yc, short xdim, short ydim, short damage_type)
{
	short x = 0;
	short y = ydim;
	short p = 3 - 2 * ydim;
	short xmul = xdim / ydim;

	while (x <= y)
	{
		check_strip(xc - x * xmul, xc + x * xmul, yc + y, damage_type);
		check_strip(xc - x * xmul, xc + x * xmul, yc - y, damage_type);
		check_strip(xc - y * xmul, xc + y * xmul, yc + x, damage_type);
		check_strip(xc - y * xmul, xc + y * xmul, yc - x, damage_type);

		if (p < 0)
			p += 4 * x++ + 6;
		else
			p += 4 * (x++ - y--) + 10;
	}

}


char rect_circle_overlap(short x0, short y0, short xdim0, short ydim0, short x1, short y1, short r)
{
	//short x, y, h, v, a, b, c;

	return rect_overlap(x0, y0, xdim0, ydim0, x1 - r, y1 - r, r << 1, r << 1);

	/*h = xdim0 >> 1;
	v = ydim0 >> 1;
	x = x0 + h;
	y = y0 + v;
	c = r * r;

	a = x + h - x1; b = y + v - y1;
	if(a * a + b * b < c) return TRUE;

	a = x + h - x1; b = y - v - y1;
	if(a * a + b * b < c) return TRUE;

	a = x - h - x1; b = y + v - y1;
	if(a * a + b * b < c) return TRUE;

	a = x - h - x1; b = y - v - y1;
	if(a * a + b * b < c) return TRUE;

	return FALSE;*/
}

void bomb_reset()
{
	short i;

	memset(glbs->bombs, 0, sizeof(BOMB) * BOMB_MAX);

	for(i = 0 ; i < BOMB_MAX ; i++)
		glbs->bombs[i].frame = -1;

	bombs_placed = 0;
}

void bomb_create(short x, short y, char type)
{
	register short i = 0;

	while(glbs->bombs[i].frame >= 0) {
		i++;
		if(i == BOMB_MAX) return;
	}

	bombs_placed++;
	memset(glbs->bombs + i, 0, sizeof(BOMB));
	glbs->bombs[i].x = x - 3;
	glbs->bombs[i].y = y - 3;
	glbs->bombs[i].type = type;
	glbs->bombs[i].frame_counter = BOMB_ANIM_SPEED;
	glbs->bombs[i].duration = BOMB_DURATION;
}

void bomb_explode(short i)
{
	ENTITY *e;
	short a;
	short tile_x = glbs->bombs[i].x / 12;
	short tile_y = glbs->bombs[i].y / 12;
	unsigned char tile = get_data(tile_x, tile_y);
	short px0 = glbs->player.e.x + glbs->player.e.x_off;
	short py0 = glbs->player.e.y + glbs->player.e.y_off;
	short px1 = px0 + glbs->player.e.width;
	short py1 = py0 + glbs->player.e.hieght;

	animation_create(glbs->bombs[i].x + 3, glbs->bombs[i].y + 3, BOMB_EXPLODE, NONE);
	glbs->bombs[i].frame = -1;
	bombs_placed--;

	for(a = 0 ; a < active_enemy_number ; a++) {
		e = &get_enemy(a)->e;

		if(rect_circle_overlap(e->x + e->x_off, e->y + e->y_off, e->width, e->hieght,
			glbs->bombs[i].x + 3, glbs->bombs[i].y + 3, 12))
				enemy_hit(active_enemies[a], BOMB_DAMAGE, FALSE, DAMAGE_BOMB, get_combo_id());
	}

	for(a = 0 ; a < door_number ; a++)
		if(doors[a].status == DOOR_CLOSED && rect_circle_overlap(
			doors[a].x, doors[a].y,
			doors[a].width, doors[a].hieght,
			glbs->bombs[i].x + 3, glbs->bombs[i].y + 3, 12)) door_open(doors + a, DAMAGE_BOMB);

	for(a = 0 ; a < glbs->pipe_number ; a++) {
		if(rect_circle_overlap(glbs->pipes[a].switch_x, glbs->pipes[a].switch_y, 12, 12,
			glbs->bombs[i].x + 3, glbs->bombs[i].y + 3, 12))
				pipe_open(glbs->pipes + a, DAMAGE_BOMB);
		else if(glbs->pipes[a].activation == PIPE_BOMB &&
			rect_circle_overlap(glbs->pipes[a].tile_x * 12, glbs->pipes[a].tile_y * 12, 12, 12,
			glbs->bombs[i].x + 3, glbs->bombs[i].y + 3, 12))
				pipe_open(glbs->pipes + a, DAMAGE_BOMB);
	}

	check_map(tile_x, tile_y, 1, 1, DAMAGE_BOMB);

	if((glbs->player.action_type & TYPE_BALL) && tile == MAX_TILES + 51 &&
		px0 >= tile_x * 12 && py0 >= tile_y * 12 && px1 <= tile_x * 12 + 12 && py1 <= tile_y * 12 + 12)
			door_bomb_switch_check();
}

void powerbomb_explode(short i)
{
	glbs->bombs[i].type = POWER_BOMB_EXPLODE;
	glbs->bombs[i].frame = 3;
}

void powerbomb_check_hit(short i)
{
	register short a;
	register short xdis, ydis, hdis;
	ENTITY *e;

	for(a = 0 ; a < active_enemy_number ; a++) {
		if(glbs->bombs[i].enemy_hit & (1 << active_enemies[a])) continue;
		e = &get_enemy(a)->e;
		xdis = (e->x - glbs->bombs[i].x);
		ydis = e->y - glbs->bombs[i].y;
		hdis = glbs->bombs[i].frame;

		if(xdis < 0) xdis *= -1;
		if(ydis < 0) ydis *= -1;

		if(e->x < glbs->bombs[i].x) xdis -= (e->width + e->x_off);
		if(e->y < glbs->bombs[i].y) ydis -= (e->hieght + e->y_off);

		if(xdis < 0) xdis = 0;
		if(ydis < 0) ydis = 0;

		xdis = xdis / 2;

		if(xdis * xdis + ydis * ydis < hdis * hdis) {
			enemy_hit(active_enemies[a], POWERBOMB_DAMAGE, FALSE, DAMAGE_POWERBOMB, get_combo_id());
			glbs->bombs[i].enemy_hit |= (1 << active_enemies[a]);
		}
	}

	a = glbs->bombs[i].frame / 12;
	if(a < 1) a = 1;
	check_map(glbs->bombs[i].x / 12, glbs->bombs[i].y / 12, a * 2, a, DAMAGE_POWERBOMB);
}



void bomb_process()
{
	register short i;

	for(i = 0 ; i < BOMB_MAX ; i++) {
		if(glbs->bombs[i].frame < 0) continue;

		if(glbs->bombs[i].type == POWER_BOMB_EXPLODE) {
			glbs->bombs[i].frame++;
			powerbomb_check_hit(i);

			if(glbs->bombs[i].frame == 100) {
				glbs->bombs[i].frame = -1;
				bombs_placed--;
			}
			continue;
		}

		glbs->bombs[i].frame_counter--;
		if(!glbs->bombs[i].frame_counter) {
			glbs->bombs[i].frame_counter = BOMB_ANIM_SPEED;
			glbs->bombs[i].frame++;
			if(glbs->bombs[i].frame == 4) glbs->bombs[i].frame = 0;
		}
		glbs->bombs[i].duration--;
		if(!glbs->bombs[i].duration) {
			if(glbs->bombs[i].type == NORMAL_BOMB) bomb_explode(i);
			else powerbomb_explode(i);

			if(!glbs->player.in_water && rect_overlap(
				glbs->player.e.x + glbs->player.e.x_off,
				glbs->player.e.y + glbs->player.e.y_off,
				glbs->player.e.width, glbs->player.e.hieght,
				glbs->bombs[i].x - 2, glbs->bombs[i].y - 2, 10, 10)) {
					if(glbs->bombs[i].x + 3 < glbs->player.e.x + glbs->player.e.x_off + glbs->player.e.width / 2)
						player_bounce(24, UP | RIGHT);
					else if(glbs->bombs[i].x + 3 > glbs->player.e.x + glbs->player.e.x_off + glbs->player.e.width / 2)
						player_bounce(24, UP | LEFT);
					else
						player_bounce(24, UP);
			}
		}
	}
}

void bomb_draw()
{
	register short i;

	for(i = 0 ; i < BOMB_MAX ; i++)
		if(glbs->bombs[i].frame >= 0) {
			if(glbs->bombs[i].type == POWER_BOMB_EXPLODE)
				DrawClippedEllipse(glbs->bombs[i].x - glbs->camera.x, glbs->bombs[i].y - glbs->camera.y, glbs->bombs[i].frame);
			else
				GraySpriteClip8_MASK(glbs->bombs[i].x - glbs->camera.x, glbs->bombs[i].y - glbs->camera.y, 8,
					glbs->bomb_gfx + glbs->bombs[i].frame * 24 + 96 * glbs->bombs[i].type,
					glbs->light_buffer, glbs->dark_buffer);
		}
}
