// C Source File
// Created 8/28/02; 4:52:05 PM

#include <tigcclib.h>         // Include All Header Files
#include "dllutility.h"
#include "dllmap.h"
#include "dllenvironment.h"
#include "dllentity.h"
#include "player.h"
#include "globals.h"
#include "bitops.h"
#include <stdint.h>

#define WATER_ANIM_DELAY 9

const unsigned char snow_gfx[256] = {
	//sprite 0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x0f,0x80,0x00,0x00,0x0f,0x00,0x01,0xc0,0x07,0x00,0x03,0xc0,0x00,0x00,0x03,0xe0,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x03,0xc0,0x00,0x00,0x03,0xc0,0x00,0x00,0x01,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0xe0,0x07,0x80,0x00,0xf0,0x07,0xc0,0x00,0xf0,0x03,0x80,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xc0,0x00,0x00,0x03,0xe0,0x00,0x00,0x01,0xe0,0x00,0x00,0x01,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	//sprite 1
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x08,0x80,0x00,0x00,0x0d,0x00,0x01,0xc0,0x07,0x00,0x02,0x40,0x00,0x00,0x03,0x20,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x02,0x40,0x00,0x00,0x03,0x40,0x00,0x00,0x01,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0xe0,0x04,0x80,0x00,0x90,0x06,0x40,0x00,0x90,0x03,0x80,0x00,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xc0,0x00,0x00,0x02,0x20,0x00,0x00,0x01,0x60,0x00,0x00,0x01,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

char snow_y_off;
char snow_x_off;

// m68k 'swap': exchange the high and low 16-bit halves of a 32-bit word.
#define ASM_SWAP(val) (((unsigned long)(val) << 16) | ((unsigned long)(val) >> 16))


void draw_snow()
{
	// snow_gfx is big-endian 32-bit; gfx0/gfx1 hold host-native computed words.
	// Each row uses one 32x32 snow pattern word tiled across 5 plane words; the
	// planes are 32-bit big-endian (not 'long' = 8 bytes on arm64).
	unsigned char *light = glbs->light_buffer;
	unsigned char *dark = glbs->dark_buffer;
	uint32_t *snow0 = (uint32_t *)snow_gfx;
	uint32_t *snow1 = (uint32_t *)(snow_gfx + 128);
	uint32_t gfx0[32];
	uint32_t gfx1[32];
	short i, j, idx;
	short cnt2 = 31 - snow_x_off;

	for(i = 0 ; i < 32 ; i++) {
		uint32_t s0 = LD32(&snow0[i]), s1 = LD32(&snow1[i]);
		gfx0[i] = ~((s0 >> snow_x_off) | (s0 << cnt2));
		gfx1[i] = (s1 >> snow_x_off) | (s1 << cnt2);
	}

	idx = 31 - snow_y_off;

	for(i = 0 ; i < 100 ; i++) {
		uint32_t g0 = gfx0[idx], g1 = gfx1[idx];
		for(j = 0 ; j < 5 ; j++) {
			ST32(light + j * 4, LD32(light + j * 4) & g0);
			ST32(dark  + j * 4, (LD32(dark + j * 4) & g0) | g1);
		}
		light += 30;
		dark += 30;
		idx++;
		if(idx == 32) idx = 0;
	}
}

/*===========================================================================*/
/* draws a horizontal line from (x1,y) to (x2,y) in given plane              */
/*===========================================================================*/
void draw_spotlight_strip(short x1, short x2, short y) {
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

    if(x1 < 0) x1 = 0;
    else if(x1 >= glbs->display_width) return;

    if(x2 < 0) return;
    else if(x2 >= glbs->display_width) x2 = glbs->display_width - 1;

    p1  = (unsigned short*)(glbs->light_buffer+(y<<5)-(y<<1)+((x1>>3)&0x1e));
    p2  = (unsigned short*)(glbs->dark_buffer+(y<<5)-(y<<1)+((x1>>3)&0x1e));

    dx = x2-x1+1;
    if(dx < 1) return;
    sx = x1 & 0x000f;

    // Plane words are big-endian (MSB = leftmost pixel); access via LD/ST so the
    // masks line up and the 32-bit case writes 4 bytes (not 8 as 'long' on arm64).
    if (dx<16) {
        unsigned long val = (ASM_SWAP(table2[dx])) >> sx;
        ST32(p1, (LD32(p1) & ~val) | (~LD32(p2) & val));
        ST32(p2, LD32(p2) | val);

        return;
    }

    if (sx) {
        ST16(p1, (LD16(p1) & ~table1[sx]) | (~LD16(p2) & table1[sx])); p1++;
        ST16(p2, LD16(p2) | table1[sx]); p2++;
        dx -= (16-sx);
    }
    while (dx >= 16) {
        ST16(p1, ~LD16(p2)); p1++;
        ST16(p2, 0xffff); p2++;
        dx-=16;
    }
    if (dx) {
    	ST16(p1, (LD16(p1) & ~table2[dx]) | (~LD16(p2) & table2[dx]));
    	ST16(p2, LD16(p2) | table2[dx]);
    }
}

void draw_spotlight(short xc, short yc, short r)
{
	register short x;
	register short y = r;
	register short p = 3 - 2 * r;
	register uint32_t *addr1 = glbs->light_buffer;
	register uint32_t *addr2 = glbs->dark_buffer;

  for(x = 0 ; x < yc - r ; x++) {
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
  	*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;

  	addr1 = (void *)((char *)addr1 + (6));
  	addr2 = (void *)((char *)addr2 + (6));
  }

  x = 0;

	while (x <= y)
	{
		draw_spotlight_strip(0, xc - y, yc + x); draw_spotlight_strip(xc + y, glbs->display_width - 1, yc + x);
		if(x != 0) {
			draw_spotlight_strip(0, xc - y, yc - x);
			draw_spotlight_strip(xc + y, glbs->display_width - 1, yc - x);
		}

		if (p < 0) {
			p += 4 * x++ + 6;

		} else {

			if(x != y) {
				draw_spotlight_strip(0, xc - x, yc + y); draw_spotlight_strip(xc + x, glbs->display_width - 1, yc + y);
				draw_spotlight_strip(0, xc - x, yc - y); draw_spotlight_strip(xc + x, glbs->display_width - 1, yc - y);
			}

			p += 4 * (x++ - y--) + 10;
		}
	}

	x = yc + r + 1;

	addr1 = glbs->light_buffer + 30 * x;
	addr2 = glbs->dark_buffer + 30 * x;

	for( ; x < glbs->display_hieght ; x++) {
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;
		*addr1++ = ~*addr2; *addr2++ |= 0xffffffff;

  	addr1 = (void *)((char *)addr1 + (6));
  	addr2 = (void *)((char *)addr2 + (6));
  }
}

void draw_water(unsigned char *water_gfx)
{
	long addr1, addr2;
	long sprite;
	short x, h, i;
	short y_pos;
	short cnt2 = 31 - glbs->water.cnt;
	short top = glbs->water.top;
	uint32_t *frame = (uint32_t *)(water_gfx + glbs->water.frame * 128);
	uint32_t gfx[64];

	if(glbs->water.type == WATER_LAVA) {
		frame += 8 * 32;
		top -= 8;
	} else if(glbs->water.type == WATER_ACID)
		frame += 8 * 32 + 10 * 32;

	// water_gfx is big-endian 32-bit; read via LD32 so the wave rotate is correct.
	for(i = 0 ; i < 32 ; i++)
		{ uint32_t f = LD32(&frame[i]); gfx[i] = (f << glbs->water.cnt) | (f >> cnt2); }

	if(glbs->water.type == WATER_NORMAL) frame += 4 * 32;
	else frame += 5 * 32;

	for(i = 0 ; i < 32 ; i++)
		{ uint32_t f = LD32(&frame[i]); gfx[32 + i] = (f << glbs->water.cnt) | (f >> cnt2); }

	for(x = 0 ; x < 5 ; x++) {
		y_pos = top - glbs->camera.y;
		if(y_pos < 0) y_pos = -((-y_pos) & 31);
		addr1 = (long)(glbs->light_buffer + 4 * x);
		addr2 = (long)(glbs->dark_buffer + 4 * x);
		if(y_pos > 0) {addr1 += y_pos * 30;addr2 += y_pos * 30;}

		for(;y_pos < 100 ; y_pos += 32) {
			sprite = (long)gfx;
			if(y_pos != top - glbs->camera.y) sprite += 32 * 4;

			if(y_pos < 0) {
				sprite -= y_pos * 4;
				h = 32 + y_pos;
			} else if(y_pos + 32 >= 100) h = 100 - y_pos;
			else h = 32;

			// gfx[] holds host-native computed wave words; the planes are
			// big-endian (LD32/ST32) and 32-bit (not 'long' = 8 bytes on arm64).
			if(glbs->water.type == WATER_NORMAL) {
				for(;h;h--,sprite+=4,addr1+=30,addr2+=30) {
					ST32((void *)addr1, LD32((void *)addr1) ^ (*(uint32_t *)sprite));
				}
			} else {
				for(;h;h--,sprite+=4,addr1+=30,addr2+=30) {
					ST32((void *)addr1, LD32((void *)addr1) & ~(*(uint32_t *)sprite));
					ST32((void *)addr2, LD32((void *)addr2) | (*(uint32_t *)sprite));
				}
			}

		}
	}
}

void environment_setup(char type, char water_type, short start, short end, short speed)
{
	glbs->environment_type = type;
	glbs->water.type = water_type;
	glbs->water.frame = 0;
	glbs->water.anim_counter = 0;
	glbs->water.top = glbs->water.start = start;// - 12;
	glbs->water.target = glbs->water.end = end;// + 12;
	glbs->water.counter = 0;
	glbs->water.speed = speed / 2;
	glbs->water.speed_max = speed;
	glbs->water.cnt = 0;
	snow_y_off = 0;
}

void environment_process()
{
	short mid;
	short delta = abs(glbs->water.start - glbs->water.end) / 2;
	short accel;
	char water_walking;
	char frame_max[] = {0, 3, 4, 4};

	snow_x_off = (snow_x_off + SNOW_Y_SPEED) & 31;
	snow_y_off = (snow_y_off + SNOW_Y_SPEED) & 31;

	if(glbs->water.type == WATER_NONE) return;

	glbs->water.anim_counter++;
	if(glbs->water.anim_counter == WATER_ANIM_DELAY) {
		glbs->water.frame++;
		if(glbs->water.frame == frame_max[glbs->water.type]) glbs->water.frame = 0;
		glbs->water.anim_counter = 0;
		glbs->water.cnt = (glbs->water.cnt + 1) & 31;
	}

	if(glbs->water.start == glbs->water.end) return;

	if(glbs->water.start < glbs->water.end) mid = glbs->water.start + delta;
	else mid = glbs->water.end + delta;
	accel = glbs->water.speed_max * 2 / delta;
	water_walking = (glbs->player.e.water_walking &&
		(glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght == glbs->water.top));

	glbs->water.counter += glbs->water.speed;
	while(glbs->water.counter >= 100) {
		glbs->water.counter -= 100;

		if((glbs->water.top < glbs->water.target && glbs->water.top < mid) || (glbs->water.top > glbs->water.target && glbs->water.top > mid))
			glbs->water.speed += accel;
		else
			glbs->water.speed -= accel;

		if(glbs->water.top < glbs->water.target) {
			glbs->water.top++;
			if(water_walking) entity_move(&glbs->player.e, DOWN, 100);
		} else if(glbs->water.top > glbs->water.target) {
			glbs->water.top--;
			if(water_walking) entity_move(&glbs->player.e, UP, 100);
		}

		else {
			if(glbs->water.top == glbs->water.start) glbs->water.target = glbs->water.end;
			else glbs->water.target = glbs->water.start;
			glbs->water.speed = glbs->water.speed_max / 2;
			//clrscr();printf("%d", glbs->water.speed);pause();
		}
	}

}

void environment_draw(unsigned char *water_gfx)
{
	if(glbs->environment_type == ENVIRONMENT_SNOW) draw_snow();
	if(glbs->environment_type == ENVIRONMENT_DARK)
		draw_spotlight(glbs->player.e.x + glbs->player.e.size->x_off + glbs->player.e.size->width / 2 - glbs->camera.x,
		glbs->player.e.y + glbs->player.e.size->y_off + glbs->player.e.size->hieght / 2 - glbs->camera.y, 50);

	if(glbs->water.type != WATER_NONE) draw_water(water_gfx);
}

