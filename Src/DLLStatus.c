// C Source File
// Created 5/15/2004; 3:46:22 PM

#define __IN_DLL__
#include <tigcclib.h>
#include "dllsystem.h"
#include "dllutility.h"
#include "globals.h"

unsigned char arrow_gfx[336] = {
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
		if((glbs->player.items_found >> i) & 1L) number++;

	if(number == 0) return;

	m = menu_create(number);

	for(i = 0 ; i < 15 ; i++) {
		if((glbs->player.items_equiped >> i) & 1L) s = TRUE;
		else if((glbs->player.items_found >> i) & 1L) s = FALSE;
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
		if((glbs->player.items_found >> i) & 1L)
			draw_string(4, 13 + 5 + 7 * i - 105, item_names[i], glbs->font, A_NORMAL);
	}

	r = menu_process(m, glbs->font);
	free(m);

	new_items = 0;
	j = 0;
	for(i = 0 ; i < HEAT_SEEKERS ; i++) {
		if(!((glbs->player.items_found >> i) & 1L)) continue;
		if(r & (1L << j)) new_items |= (1L << i);
		j++;
	}
	glbs->player.items_equiped = new_items;
	/*if(glbs->player.items_equiped & GRAVITY_SUIT) glbs->player.in_water = FALSE;
	else if(glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght > water.top && !glbs->player.in_water) {
		glbs->player.in_water = TRUE;
		glbs->player.walk_speed = (WALK_SPEED_START >> 1);
		glbs->player.sprint_state = 0;
		glbs->player.jump_speed = 75;
	}*/
}

void status_screen()
{
	void *light, *dark;
	short x, y, tile_x, tile_y, k, i, j;
	short camera_x, camera_y;
	short len;
	unsigned char tile;
	static unsigned char dot_black[] = {0b11100000, 0b11100000, 0b11100000};
	short blink_counter = 0;
	short blink = 0;
	unsigned long save_seconds = seconds;
	short zone = glbs->current_map.zone;

	flipping_off();

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
			*(short *)light &= 0b1111100000000000; light += 2;
			*(short *)dark &= 0b1111100000000000; dark += 2;
			for(j = 0 ; j < 4 ; j++) {
				*(long *)light = 0; light += 4;
				*(long *)dark = 0; dark += 4;
			}
			*(short *)light &= 0b0000000000011111; light += 12;
			*(short *)dark &=0b0000000000011111; dark += 12;
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
	flipping_on();
	seconds = save_seconds;
	glbs->game_counter = 0;
}


