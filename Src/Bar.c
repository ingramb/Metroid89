// C Source File
// Created 3/12/02; 5:00:45 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "entity.h"
#include "utility.h"
#include "extgraph.h"
#include "tiles.h"
#include "enemy.h"
#include "door.h"
#include "map.h"
#include "player.h"
#include "miscgfx.h"
#include "system.h"
#include "bar.h"
#include "globals.h"
#include "bitops.h"
#include <stdint.h>

void *status_bar_light = NULL;
void *status_bar_dark = NULL;
void *status_bar_mask = NULL;
void *map_light = NULL;
void *map_dark = NULL;
void *zone_name_buffer = NULL;

#define BAR_DURATION 100
#define ZONE_DURATION 150

char update;
char update_map;
short bar_position;
short bar_duration;
short zone_position;
short zone_duration;

const unsigned char icon_gfx[180] = {
	//sprite 0
	0x00,0x00,0x00,0x60,0x01,0xa0,0x3f,0x80,0x65,0x00,0x4a,0xe0,0x3f,0x00,0x01,0xe0,0x00,0x60,0x00,0x00,
	0x00,0x00,0x00,0x60,0x01,0x80,0x3f,0x60,0x50,0x00,0x75,0x00,0x3f,0xe0,0x01,0x80,0x00,0x60,0x00,0x00,
	0xff,0x0f,0xfc,0x0f,0x80,0x0f,0x00,0x0f,0x00,0x0f,0x00,0x0f,0x00,0x0f,0x80,0x0f,0xfc,0x0f,0xff,0x0f,
	//sprite 1
	0x00,0x00,0x10,0x00,0x3a,0x00,0x53,0xc0,0x20,0x00,0x1f,0xe0,0x53,0xc0,0x3e,0x00,0x10,0x00,0x00,0x00,
	0x00,0x00,0x1c,0x00,0x3e,0x00,0x73,0xc0,0x52,0xa0,0x72,0xa0,0x7f,0xc0,0x3e,0x00,0x1c,0x00,0x00,0x00,
	0xc1,0xff,0x80,0xff,0x00,0x1f,0x00,0x0f,0x00,0x0f,0x00,0x0f,0x00,0x0f,0x00,0x1f,0x80,0xff,0xc1,0xff,
	//sprite 2
	0x00,0x00,0x00,0x00,0x10,0x80,0x1d,0x80,0x28,0x40,0x28,0x40,0x12,0x80,0x10,0x80,0x0f,0x00,0x00,0x00,
	0x00,0x00,0x0f,0x00,0x19,0x80,0x32,0xc0,0x51,0xa0,0x57,0xa0,0x39,0xc0,0x1f,0x80,0x0f,0x00,0x00,0x00,
	0xe0,0x7f,0xc0,0x3f,0x80,0x1f,0x00,0x0f,0x00,0x0f,0x00,0x0f,0x00,0x0f,0x80,0x1f,0xc0,0x3f,0xe0,0x7f,
};

const unsigned char arrow_gfx[336] = {
	//sprite 0
	0x03,0xc0,0x04,0x40,0x08,0x40,0x12,0x40,0x26,0x40,0x4e,0x40,0x9e,0x40,0x9e,0x40,0x4e,0x40,0x26,0x40,0x12,0x40,0x08,0x40,0x04,0x40,0x03,0xc0,
	0x03,0xc0,0x04,0x40,0x0b,0x40,0x15,0x40,0x29,0x40,0x51,0x40,0xa1,0x40,0xa1,0x40,0x51,0x40,0x29,0x40,0x15,0x40,0x0b,0x40,0x04,0x40,0x03,0xc0,
	0xfc,0x3f,0xf8,0x3f,0xf0,0x3f,0xe0,0x3f,0xc0,0x3f,0x80,0x3f,0x00,0x3f,0x00,0x3f,0x80,0x3f,0xc0,0x3f,0xe0,0x3f,0xf0,0x3f,0xf8,0x3f,0xfc,0x3f,
	//sprite 1
	0xf0,0x00,0x88,0x00,0x84,0x00,0x92,0x00,0x99,0x00,0x9c,0x80,0x9e,0x40,0x9e,0x40,0x9c,0x80,0x99,0x00,0x92,0x00,0x84,0x00,0x88,0x00,0xf0,0x00,
	0xf0,0x00,0x88,0x00,0xb4,0x00,0xaa,0x00,0xa5,0x00,0xa2,0x80,0xa1,0x40,0xa1,0x40,0xa2,0x80,0xa5,0x00,0xaa,0x00,0xb4,0x00,0x88,0x00,0xf0,0x00,
	0x0f,0xff,0x07,0xff,0x03,0xff,0x01,0xff,0x00,0xff,0x00,0x7f,0x00,0x3f,0x00,0x3f,0x00,0x7f,0x00,0xff,0x01,0xff,0x03,0xff,0x07,0xff,0x0f,0xff,
	//sprite 2
	0x03,0x00,0x04,0x80,0x08,0x40,0x13,0x20,0x27,0x90,0x4f,0xc8,0x9f,0xe4,0x80,0x04,0x80,0x04,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x03,0x00,0x04,0x80,0x0b,0x40,0x14,0xa0,0x28,0x50,0x50,0x28,0xa0,0x14,0xbf,0xf4,0x80,0x04,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xfc,0xff,0xf8,0x7f,0xf0,0x3f,0xe0,0x1f,0xc0,0x0f,0x80,0x07,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	//sprite 3
	0xff,0xfc,0x80,0x04,0x80,0x04,0x9f,0xe4,0x4f,0xc8,0x27,0x90,0x13,0x20,0x08,0x40,0x04,0x80,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xff,0xfc,0x80,0x04,0xbf,0xf4,0xa0,0x14,0x50,0x28,0x28,0x50,0x14,0xa0,0x0b,0x40,0x04,0x80,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x80,0x07,0xc0,0x0f,0xe0,0x1f,0xf0,0x3f,0xf8,0x7f,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};

const unsigned char energy_lable[15] = {
	0xdd,0xb1,0xa8,0x95,0x2a,0x28,0xd5,0xb2,0xa8,0x95,0x2a,0x90,0xd5,0xa9,0x90,
};

const unsigned char full_tank[] = {
	0b11100000, 0b00100000, 0b00100000, 0b00100000, 0b11100000,
	0b00000000, 0b11000000, 0b11000000, 0b11000000, 0b11100000
};

const unsigned char empty_tank[] = {
	0b11100000, 0b11100000, 0b11100000, 0b11100000, 0b11100000
};

const unsigned char hpfont_gfx[] = {
	//sprite 0
	0x78,0xcc,0xcc,0xcc,0x78,
	//sprite 1
	0x0c,0x1c,0x0c,0x0c,0x0c,
	//sprite 2
	0xf8,0x0c,0x78,0xc0,0xfc,
	//sprite 3
	0xf8,0x0c,0x78,0x0c,0xf8,
	//sprite 4
	0x38,0x58,0x98,0xfc,0x18,
	//sprite 5
	0xf8,0xc0,0xf8,0x0c,0xf8,
	//sprite 6
	0x78,0xc0,0xf8,0xcc,0x78,
	//sprite 7
	0xfc,0x0c,0x18,0x30,0x30,
	//sprite 8
	0x78,0xcc,0x78,0xcc,0x78,
	//sprite 9
	0x78,0xcc,0x7c,0x0c,0x78,
};

const unsigned char hpfont_mask[70] = {
	//sprite 0
	0x3c,0x7e,0xff,0xff,0xff,0x7e,0x3c,
	//sprite 1
	0x07,0x0f,0x1f,0x0f,0x0f,0x0f,0x0f,
	//sprite 2
	0xfc,0xfe,0x7f,0x7e,0xfe,0xff,0xfe,
	//sprite 3
	0xfc,0xfe,0xff,0x7e,0xff,0xfe,0xfc,
	//sprite 4
	0x1e,0x3e,0x7e,0xfe,0xff,0xfe,0x1e,
	//sprite 5
	0xfe,0xfe,0xfc,0xfe,0x7f,0xfe,0x7c,
	//sprite 6
	0x3c,0x7e,0xfc,0xfe,0xff,0x7e,0x3c,
	//sprite 7
	0xff,0xff,0xff,0x1e,0x3c,0x3c,0x3c,
	//sprite 8
	0x3c,0x7e,0xff,0x7e,0xff,0x7e,0x3c,
	//sprite 9
	0x3c,0x7e,0xff,0x7f,0x3f,0x7e,0x7c,
};

char bar_setup()
{
	status_bar_light = malloc(/*bar*/16 * 30 * 3 + /*map*/4 * 15 * 2 + /*zone*/9 * 30);
	status_bar_dark = status_bar_light + 16 * 30;
	status_bar_mask = status_bar_dark + 16 * 30;
	map_light = status_bar_mask + 16 * 30;
	map_dark = map_light + 4 * 15;
	zone_name_buffer = map_dark + 4 * 15;

	update = TRUE;
	update_map = TRUE;
	bar_position = 0;
	bar_duration = BAR_DURATION;
	zone_position = 100;
	zone_duration = 0;

	return TRUE;
}

void bar_cleanup()
{
	free(status_bar_light);
}

void bar_draw_number(short x, short y, unsigned char number, char digits,
	void *light, void *dark, void *mask, void *mask2)
{
	register short i;
	register short d = 1;
	short x_pos = x;
	char n;

	if(digits == 2) d = 10;
	if(digits == 3) d = 100;

	for(i = 0 ; i < digits ; i++) {
		n = number / d;

		//Sprite8_XOR(x_pos, y, 7, hpfont_mask + n * 7, mask);
		SpriteX8(x_pos, y, 7, hpfont_mask + n * 7, 1, mask, SPRITE_XOR);
		if(mask2 != NULL) SpriteX8(x_pos, y, 7, hpfont_mask + n * 7, 1, mask2, SPRITE_XOR);
		//Sprite8_XOR(x_pos, y, 7, hpfont_mask + n * 7, mask2);
		//GraySprite8_OR(x_pos + 1, y + 1, 5,
		//	hpfont_gfx + n * 5, hpfont_gfx + n * 5, light, dark);

		GraySpriteX8(x_pos + 1, y + 1, 5,
			hpfont_gfx + n * 5, hpfont_gfx + n * 5, 1, light, dark, SPRITE_OR);

		x_pos += 8;

		number %= d;
		d /= 10;
	}
}

void bar_draw_small_number(short x, short y, unsigned char number, char digits)
{
	draw_number(x, y, number, glbs->font, digits, status_bar_light, status_bar_dark);
}

void draw_map_tile(short x, short y, unsigned char tile, char width, void *light, void *dark)
{
    long           offset = (y * width) + ((x>>3)&0x1e);
    long           addr1  = (long)light+offset;
    long           addr2  = (long)dark+offset;
    unsigned char *sprite1 = glbs->map_tiles + (tile & 0x7f) * 10;
    unsigned char *sprite2 = sprite1 + 5;
    unsigned short cnt  = 24-(x&15);
    short h = 5;

    for (;h;h--,addr1+=width,addr2+=width) {
        ST32((void*)addr1, LD32((void*)addr1) | ((long)(*sprite1++)<<cnt));
        ST32((void*)addr2, LD32((void*)addr2) | ((long)(*sprite2++)<<cnt));
    }

    if(tile & 0x80)
    	ST32((void*)(dark + offset + width * 2), LD32((void*)(dark + offset + width * 2)) & (~((long)0x20 << cnt)));
}

void draw_energy(short x, short y, short hp, short hp_max,
	void *light, void *dark, void *mask, void *mask2)
{
	short tank_number = hp / 100;
	short tank_max = hp_max / 100;
	short current_tank = hp % 100;
	short i, j, x_pos, y_pos;

	bar_draw_number(x + 7, y + 6, current_tank, 2, light, dark, mask, mask2);

	for(i = 0 ; i < 7 ; i++) {
		FastDrawHLine(mask, x, x + 22, y + i, A_REVERSE);
		if(mask2 != NULL) FastDrawHLine(mask2, x, x + 22, y + i, A_REVERSE);
	}
	//GraySpriteX8_OR(x + 1, y + 1, 5, energy_lable, energy_lable, 3, light, dark);
	GraySpriteX8(x + 1, y + 1, 5, energy_lable, energy_lable, 3, light, dark, SPRITE_OR);

	j = tank_max * 4 + 1;
	if(j > 41) j = 41;
	if(j > 1) for(i = 0 ; i < 7 ; i++) {
		FastDrawHLine(mask, x + 23, x + 22 + j, y + i + 6, A_REVERSE);
		if(mask2 != NULL) FastDrawHLine(mask2, x + 23, x + 22 + j, y + i + 6, A_REVERSE);
	}

	j = (tank_max - 10) * 4 + 1;
	if(j > 41) j = 41;
	if(j > 1) for(i = 0 ; i < 6 ; i++) {
		FastDrawHLine(mask, x + 23, x + 22 + j, y + i, A_REVERSE);
		if(mask2 != NULL) FastDrawHLine(mask2, x + 23, x + 22 + j, y + i, A_REVERSE);
	}

	x_pos = 24; y_pos = 7;

	for(i = 0 ; i < tank_max ; i++) {
		if(i < tank_number)
			//GraySprite8_OR(x + x_pos, y + y_pos, 5, full_tank, full_tank + 5, light, dark);
			GraySpriteX8(x + x_pos, y + y_pos, 5, full_tank, full_tank + 5, 1, light, dark, SPRITE_OR);
		else
			//GraySprite8_OR(x + x_pos, y + y_pos, 5, empty_tank, empty_tank, light, dark);
			GraySpriteX8(x + x_pos, y + y_pos, 5, empty_tank, empty_tank, 1, light, dark, SPRITE_OR);
		x_pos += 4;
		if(x_pos > 61) {
			x_pos = 24;
			y_pos = 1;
		}
	}
}

void bar_update_draw()
{
	short i = 0, h;
	short icon_pos[] = {65, 90, 111};
	short box_width[] = {13, 9, 9};

	memset(status_bar_light, 0, 30 * 16 * 2);
	memset(status_bar_mask, 0xff, 30 * 16);

	draw_energy(0, 0, glbs->player.hp, glbs->player.hp_max, status_bar_light, status_bar_dark, status_bar_mask, NULL);

	for(i = 0 ; i < 3 ; i++) {
		if(glbs->player.ammunition_max[i]) {
			//GraySprite16_OR(67 + i * 15, 0, 13, (unsigned short *)icon_gfx + i * 39,
			//	(unsigned short *)icon_gfx + i * 39 + 13, status_bar_light, status_bar_dark);
			GraySpriteX8(icon_pos[i], 0, 10, icon_gfx + i * 60,
				icon_gfx + i * 60 + 20, 2, status_bar_light, status_bar_dark, SPRITE_OR);

			//Sprite16_AND(67 + i * 15, 0, 13, (unsigned short *)icon_gfx + i * 39 + 26, status_bar_mask);
			SpriteX8(icon_pos[i], 0, 10, icon_gfx + i * 60 + 40, 2, status_bar_mask, SPRITE_AND);

			if(glbs->player.weapon_selected == i) {
				for(h = 0 ; h < 7 ; h++) {
					FastDrawHLine(status_bar_light, icon_pos[i] + 11,
						icon_pos[i] + 10 + box_width[i], h, A_NORMAL);
					FastDrawHLine(status_bar_dark, icon_pos[i] + 11,
						icon_pos[i] + 10 + box_width[i], h, A_NORMAL);
				}
			}

			for(h = 0 ; h < 7 ; h++) FastDrawHLine(status_bar_mask, icon_pos[i] + 11,
				icon_pos[i] + 10 + box_width[i], h, A_REVERSE);

			bar_draw_small_number(icon_pos[i] + 12, 1, glbs->player.ammunition[i], i == 0 ? 3 : 2);
		}
	}
}

void map_update_draw()
{
	short x_pos = 7;
	short y_pos = 0;
	short x;
	short y;
	unsigned char tile;

	memset(map_light, 0, 4 * 15 * 2);

	for(x = glbs->player.global_screen_x - 2 ; x <= glbs->player.global_screen_x + 2 ; x++) {
		for(y = glbs->player.global_screen_y - 1 ; y <= glbs->player.global_screen_y + 1 ; y++) {
			if(x < 0 || x >= global_map_width || y < 0 || y >= global_map_hieght) tile = 81;
			else tile = explored_map[glbs->current_map.zone][y * global_map_width + x];
			draw_map_tile(x_pos, y_pos, tile, 4, map_light, map_dark);

			y_pos += 5;
			if(y_pos == 15) {
				y_pos = 0;
				x_pos += 5;
			}
		}
	}

	for(y = 6 ; y < 9 ; y++) {
		ST32((uint32_t*)map_light + y, LD32((uint32_t*)map_light + y) | (~0b11111111111111111100011111111111));
		ST32((uint32_t*)map_dark + y, LD32((uint32_t*)map_dark + y) & (0b11111111111111111100011111111111));
	}
}

void draw_time(short x, short y, unsigned long s)
{
	short hours, minutes;

	minutes = s / 60;
	hours = minutes / 60;
	minutes = minutes % 60;

	draw_number(x, y, hours, glbs->font, 2, glbs->light_buffer, glbs->dark_buffer);
	draw_string(x + 7, y, ":", glbs->font, A_NORMAL);
	draw_number(x + 10, y, minutes, glbs->font, 2, glbs->light_buffer, glbs->dark_buffer);
}

static void draw_rect(short x, short y, short w, short h, char mode)
{
	short i;

	for(i = 0 ; i < h ; i++) {
		FastDrawHLine(glbs->light_buffer, x, x + w - 1, i + y, mode);
		FastDrawHLine(glbs->dark_buffer, x, x + w - 1, i + y, mode);
	}
}

void draw_status_bar()
{
	short i;
	short icon_pos[] = {39, 80, 113};
	short box_pos[] = {50, 91, 124};
	short digits[] = {3, 2, 2};

	draw_rect(0, 0, 39, 15, A_NORMAL);
	draw_rect(1, 1, 37, 13, A_REVERSE);
	//GraySpriteX8_OR(2, 2, 5, energy_lable, energy_lable, 3, glbs->light_buffer, glbs->dark_buffer);
	GraySpriteX8(2, 2, 5, energy_lable, energy_lable, 3, glbs->light_buffer, glbs->dark_buffer, SPRITE_OR);
	draw_number(2, 8, glbs->player.hp, glbs->font, 4, glbs->light_buffer, glbs->dark_buffer);
	draw_number(22, 8, glbs->player.hp_max, glbs->font, 4, glbs->light_buffer, glbs->dark_buffer);
	draw_string(18, 8, "/", glbs->font, A_NORMAL);

	for(i = 0 ; i < 3 ; i++)
		if(glbs->player.ammunition_max[i] != 0) {
			//GraySprite16_XOR(icon_pos[i], 2, 7,
			//	(unsigned short *)(status_icon_gfx + 28 * i),
			//	(unsigned short *)(status_icon_gfx + 28 * i + 14), glbs->light_buffer, glbs->dark_buffer);
			GraySpriteX8_mask(icon_pos[i], 0, 10, icon_gfx + 60 * i, 2,
				glbs->light_buffer, glbs->dark_buffer);
			draw_rect(box_pos[i], 2, 8 * digits[i] + 5, 7, A_REVERSE);
			draw_number(box_pos[i] + 1, 3, glbs->player.ammunition[i], glbs->font, digits[i],
				glbs->light_buffer, glbs->dark_buffer);
			draw_string(box_pos[i] + digits[i] * 4 + 1, 3, "/", glbs->font, A_NORMAL);
			draw_number(box_pos[i] + digits[i] * 4 + 5, 3, glbs->player.ammunition_max[i], glbs->font, digits[i],
				glbs->light_buffer, glbs->dark_buffer);
		}
}

void status_screen()
{
	void *light, *dark;
	short x, y, tile_x, tile_y, k, i, j;
	short camera_x, camera_y;
	short len;
	unsigned char tile;
	static unsigned char dot_black[] = {0b11100000, 0b11100000, 0b11100000};
	char blink_counter = 0;
	char blink = 0;
	unsigned long save_seconds = glbs->seconds;
	unsigned char zone = glbs->current_map.zone;

	flipping_off();

	load_metsys();

	len = string_width(zone_name) + 8;

	memset(glbs->light_buffer, 0xff, LCD_SIZE);
	memset(glbs->dark_buffer, 0xff, LCD_SIZE);
	draw_border(0, 10, 160, 90, FALSE);
	draw_status_bar();

	camera_x = global_map_width / 2 - 15;
	camera_y = global_map_hieght / 2 - 8;

	while(1) {
		light = glbs->light_buffer + 15 * 30;
		dark = glbs->dark_buffer + 15 * 30;
		for(i = 15 ; i <= 94 ; i++) {
			// Clear the map interior per row, preserving the left/right border
			// columns. The masks are big-endian (byte 0 = leftmost pixels), so go
			// through LD16/ST16 -- a raw *(short*) swaps the bytes on a LE host and
			// erases the border instead of the interior.
			ST16(light, LD16(light) & 0xF800); light += 2;
			ST16(dark,  LD16(dark)  & 0xF800); dark += 2;
			for(j = 0 ; j < 4 ; j++) {
				ST32((void*)light, 0); light += 4;
				ST32((void*)dark, 0); dark += 4;
			}
			ST16(light, LD16(light) & 0x001F); light += 12;
			ST16(dark,  LD16(dark)  & 0x001F); dark += 12;
		}

		for(x = 0 ; x < 30 ; x++) {
			for(y = 0 ; y < 16 ; y++) {
				tile_x = x + camera_x;
				tile_y = y + camera_y;
				if(tile_x < 0 || tile_x >= global_map_width || tile_y < 0 || tile_y >= global_map_hieght)
					tile = 81;
				else
					tile = explored_map[zone][tile_y * global_map_width + tile_x];

				draw_map_tile(x * 5 + 5, y * 5 + 15, tile, 30, glbs->light_buffer, glbs->dark_buffer);
			}
		}

		blink_counter++;
		if(blink_counter == 3 + 2 * blink) {
			blink = !blink;
			blink_counter = 0;
		}

		if(blink) {
			tile_x = glbs->player.global_screen_x - camera_x;
			tile_y = glbs->player.global_screen_y - camera_y;
			if(tile_x >= 0 && tile_x < 30 && tile_y >= 0 && tile_y < 16)
				//GraySprite8_XOR(tile_x * 5 + 6, tile_y * 5 + 16, 3, dot_black, dot_black, glbs->light_buffer, glbs->dark_buffer);
				GraySpriteX8(tile_x * 5 + 6, tile_y * 5 + 16, 3, dot_black, dot_black, 1,
					glbs->light_buffer, glbs->dark_buffer, SPRITE_XOR);

			if(camera_x > 0)
				//GraySprite16_MASK(4, 47, 14,
				//	(unsigned short *)(arrow_gfx),
				//	(unsigned short *)(arrow_gfx + 28),
				//	(unsigned short *)(arrow_gfx + 56),
				//	(unsigned short *)(arrow_gfx + 56), glbs->light_buffer, glbs->dark_buffer);
				GraySpriteX8_mask(4, 47, 14, arrow_gfx, 2, glbs->light_buffer, glbs->dark_buffer);

			if(camera_y > 0)
				//GraySprite16_MASK(73, 14, 14,
				//	(unsigned short *)(arrow_gfx + 84 * 2),
				//	(unsigned short *)(arrow_gfx + 84 * 2 + 28),
				//	(unsigned short *)(arrow_gfx + 84 * 2 + 56),
				//	(unsigned short *)(arrow_gfx + 84 * 2 + 56), glbs->light_buffer, glbs->dark_buffer);
				GraySpriteX8_mask(73, 14, 14, arrow_gfx + 84 * 2, 2, glbs->light_buffer, glbs->dark_buffer);

			if(camera_x < global_map_width - 30)
				//GraySprite16_MASK(145, 47, 14,
				//	(unsigned short *)(arrow_gfx + 84) ,
				//	(unsigned short *)(arrow_gfx + 84 + 28),
				//	(unsigned short *)(arrow_gfx + 84 + 56),
				//	(unsigned short *)(arrow_gfx + 84 + 56), glbs->light_buffer, glbs->dark_buffer);
				GraySpriteX8_mask(145, 47, 14, arrow_gfx + 84, 2, glbs->light_buffer, glbs->dark_buffer);

			if(camera_y < global_map_hieght - 16)
				//GraySprite16_MASK(73, 85, 14,
				//	(unsigned short *)(arrow_gfx + 84 * 3),
				//	(unsigned short *)(arrow_gfx + 84 * 3 + 28),
				//	(unsigned short *)(arrow_gfx + 84 * 3 + 56),
				//	(unsigned short *)(arrow_gfx + 84 * 3 + 56), glbs->light_buffer, glbs->dark_buffer);
				GraySpriteX8_mask(73, 85, 14, arrow_gfx + 84 * 3, 2, glbs->light_buffer, glbs->dark_buffer);
		}
		draw_border(0, 87, len, 13, TRUE);
		draw_string(4, 91, zone_name, glbs->font, A_NORMAL);
		draw_border(133, 87, 27, 13, TRUE);
		draw_time(138, 91, save_seconds);
		update_screen();

		k = player_get_input_89();
		if((k & LEFT_PRESSED) && camera_x > -10) camera_x--;
		else if((k & RIGHT_PRESSED) && camera_x < global_map_width - 20) camera_x++;
		if(k & UP_PRESSED && camera_y > -10) camera_y--;
		else if((k & DOWN_PRESSED) && camera_y < global_map_hieght - 6) camera_y++;

		if(k & SHOOT_PRESSED) break;
		if(k & JUMP_PRESSED) goto end;
	}

	item_menu();
end:
	while(_rowread(0));
	bar_update();
	bar_active();
	load_metgame();
	flipping_on();
	glbs->seconds = save_seconds;
	glbs->game_counter = 0;
}

void bar_update()
{
	update = TRUE;
	bar_duration = BAR_DURATION;
}

void map_update()
{
	short i;
	short loc = glbs->player.global_screen_y * global_map_width + glbs->player.global_screen_x;

	for(i = 0 ; i < glbs->current_map.screen_number ; i++) {
		if(glbs->player.screen_x == glbs->current_map.screen_header[i].x_pos &&
			glbs->player.screen_y == glbs->current_map.screen_header[i].y_pos) {
				explored_map[glbs->current_map.zone][loc] = glbs->current_map.screen_header[i].map_tile;
				break;
		}
	}

	//explored_map[glbs->current_map.zone][loc] = global_map[loc];
	update_map = TRUE;
}

void bar_active()
{
	bar_duration = BAR_DURATION;
	bar_position = 0;
}

void zone_name_activate()
{
	short x;

	memset(zone_name_buffer, 0xff, 30 * 9);
	load_metsys();
	x = (160 - string_width(zone_name)) / 2;
	draw_string_ex(x, 2, zone_name, glbs->font, A_XOR, zone_name_buffer, glbs->light_buffer);
	load_metgame();
	zone_position = 99;
	zone_duration = ZONE_DURATION;

	/*memset(glbs->light_buffer, 0, 3000);
	memset(glbs->dark_buffer, 0, 3000);
	memcpy(glbs->light_buffer, zone_name_buffer, 30 * 9);
	memcpy(glbs->dark_buffer, zone_name_buffer, 30 * 9);
	update_screen();
	pause();*/

}

void bar_process()
{
	if(zone_duration) {
		if(zone_position > 91) zone_position--;
		zone_duration--;
	} else if(zone_position < 100) zone_position++;

	if(bar_duration) {
		bar_duration--;
		if(bar_position < 0) bar_position++;
	} else if(bar_position > -13) bar_position--;
}

void bar_draw()
{
	register short i;
	register uint32_t *dest1 = (long *)(glbs->light_buffer);
	register uint32_t *dest2 = (long *)(glbs->dark_buffer);
	register uint32_t *src1 = status_bar_light;
	register uint32_t *src2 = status_bar_dark;
	register uint32_t *mask = status_bar_mask;

	if(update) {
		update = FALSE;
		bar_update_draw();
	}

	if(update_map) {
		update_map = FALSE;
		map_update_draw();
	}

	src1 = (void *)((char *)src1 - bar_position * 30);
	src2 = (void *)((char *)src2 - bar_position * 30);
	mask = (void *)((char *)mask - bar_position * 30);

	for(i = 13 + bar_position; i > 0 ; i--) {
		*dest1 &= *mask; *dest1++ |= *src1++;
		*dest2 &= *mask++; *dest2++ |= *src2++;

		*dest1 &= *mask; *dest1++ |= *src1++;
		*dest2 &= *mask++; *dest2++ |= *src2++;

		*dest1 &= *mask; *dest1++ |= *src1++;
		*dest2 &= *mask++; *dest2++ |= *src2++;

		*dest1 &= *mask; *dest1++ |= *src1++;
		*dest2 &= *mask++; *dest2++ |= *src2++;

		*dest1 &= *mask; *dest1 |= *src1;
		*dest2 &= *mask; *dest2 |= *src2;

		dest1 = (void *)((char *)dest1 + 14); dest2 = (void *)((char *)dest2 + 14);
		src1 = (void *)((char *)src1 + 14); src2 = (void *)((char *)src2 + 14); mask = (void *)((char *)mask + 14);
	}

	if(zone_position < 100) {
		src1 = zone_name_buffer;
		dest1 = glbs->light_buffer + 3000 - (100 - zone_position) * 30;
		dest2 = glbs->dark_buffer + 3000 - (100 - zone_position) * 30;

		for(i = 0 ; i < 100 - zone_position ; i++) {
			*dest1++ = *src1; *dest2++ = *src1++;
			*dest1++ = *src1; *dest2++ = *src1++;
			*dest1++ = *src1; *dest2++ = *src1++;
			*dest1++ = *src1; *dest2++ = *src1++;
			*dest1 = *src1; *dest2 = *src1;
			dest1 = (void *)((char *)dest1 + 14); dest2 = (void *)((char *)dest2 + 14); src1 = (void *)((char *)src1 + 14);
		}
	}

	// Minimap composite. The mask is a literal (not a buffer), so it must be
	// applied big-endian to hit the leftmost pixels of the framebuffer word;
	// map_light/map_dark were produced big-endian (ST32) too.
	dest1 = glbs->light_buffer + 16;
	dest2 = glbs->dark_buffer + 16;
	src1 = map_light;
	src2 = map_dark;
	for(i = 0 ; i < 15 ; i++) {
		ST32(dest1, (LD32(dest1) & 0xFC000000u) | LD32(src1)); src1++;
		ST32(dest2, (LD32(dest2) & 0xFC000000u) | LD32(src2)); src2++;
		dest1 = (void *)((char *)dest1 + 30);
		dest2 = (void *)((char *)dest2 + 30);
	}
	ST32(dest1, LD32(dest1) & 0xFC000000u);
	ST32(dest2, LD32(dest2) & 0xFC000000u);
}
