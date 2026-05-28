// C Source File
// Created 12/28/2002; 10:41:55 PM

#define __IN_DLL__
#include <tigcclib.h>         // Include All Header Files
#include "stdlib.h"
#include "dllmap.h"
#include "dllutility.h"
#include "dlltiles.h"
#include "dllenvironment.h"
#include "globals.h"

//TILE_NODE_CLIPPED *tile_list_clipped = NULL;
//TILE_NODE *tile_list = NULL;

/*const char sine_wave[] =	{2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 1, 0, 0, 0, 1, 1};
												//{4, 6, 7, 8, 8, 8, 7, 6, 4, 2, 1, 0, 0, 0, 1, 2};
const char ripple_table[] = {
	0, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 0, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2,
	0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1,
	1, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 1, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1,
	0, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 0, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0,
	0, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 0, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0,
	0, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 0, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2,
	0, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 0, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1,
	1, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 1, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1,
};

//=================
void draw_bg(unsigned char *bg_tile, BACKGROUND_HEADER *bg, void *metpack_base)
{
	unsigned char *map_data;
	short cnt, cnt2;
	short i, h, s, x_pos, y_pos, x, y, loc, l;
	unsigned char *light, *dark, *light_src, *dark_src;
	unsigned long *sprite;
	unsigned short map[8 * 5];
	short hieght = 32 * bg->hieght;
	short water_level = water.top - glbs->camera.y;
	short footer_level = (100 - bg->footer_hieght * 32) +
		((long)(glbs->current_map.hieght * 12 - 100 - glbs->camera.y) * bg->scroll_y) / 100;

	if(hieght > 100) hieght = 100;

	if(bg->footer == 0 || footer_level > 100) footer_level = 100;
	else if(footer_level < 0) footer_level = 0;

	if(water_level <= 0) water_level = 0;
	else if(water_level >= 100) water_level = 100;
	else hieght = 100;

	if(hieght > footer_level) hieght = footer_level;

	map_data = (unsigned char *)(metpack_base + bg->data);
	y_pos = glbs->camera.bg_tile_y;

	for(y = 0 ; y < 5 ; y++) {
		x_pos = glbs->camera.bg_tile_x - 1;
		if(x_pos < 0) x_pos = bg->width - 1;

		for(x = 0 ; x < 8 ; x++) {
			map[y * 8 + x] = map_data[y_pos * bg->width + x_pos] * 64;
			x_pos++;
			if(x_pos == bg->width) x_pos = 0;
		}
		y_pos++;
		if(y_pos == bg->hieght) y_pos = 0;
	}

	light = glbs->light_buffer;
	dark = glbs->dark_buffer;
	cnt = glbs->camera.bg_x_off;
	cnt2 = 32 - cnt;
	loc = 1;

	if(0) {
		char *table_ptr = (char *)ripple_table + glbs->camera.bg_ripple * 32;

		hieght = 100;
		sprite = (long *)(bg_tile);
		for(i = 0 ; i < glbs->camera.bg_y_off ; i++) sprite += table_ptr[i];

		for(i = 0, h = glbs->camera.bg_y_off ; h < 32 && i < water_level ; i++, h++) {
			sprite += table_ptr[h];

			*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

			light += 30;
			dark += 30;
		}

		while(i < hieght) {
			sprite = (long *)bg_tile;
			loc += 8;

			for(h = 0 ; h < 32 && i < hieght; h++, i++) {
				sprite += table_ptr[h];

				*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
				*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

				*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
				*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

				*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
				*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

				*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
				*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

				*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
				*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

				light += 30;
				dark += 30;
			}
		}

		return;
	}

	sprite = (long *)(bg_tile + glbs->camera.bg_y_off * 4);
	s = (glbs->camera.bg_wave + water_level) & 15;

	for(i = 0, h = glbs->camera.bg_y_off ; h < 32 && i < water_level ; i++, h++) {
		*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
		*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

		*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
		*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

		*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
		*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

		*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
		*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

		*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
		*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

		light += 30;
		dark += 30;
		sprite++;
	}

	while(i < water_level && i < hieght) {
		sprite = (long *)bg_tile;
		loc += 8;

		for(h = 0 ; h < 32 && i < water_level && i < hieght; h++, i++) {
			*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

			light += 30;
			dark += 30;
			sprite++;
		}
	}

	if(h < 32 && i < hieght) {
		for(; h < 32 && i < hieght; h++, i++) {
			cnt = glbs->camera.bg_x_off + sine_wave[s];
			if(cnt > 31) {cnt -= 32; l = loc + 1;}
			else l = loc;
			cnt2 = 32 - cnt;

			*(long *)(light) = (*(sprite + map[l]) << cnt) | (*(sprite + map[l + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[l] + 32) << cnt) | (*(sprite + map[l + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[l + 1]) << cnt) | (*(sprite + map[l + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[l + 1] + 32) << cnt) | (*(sprite + map[l + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[l + 2]) << cnt) | (*(sprite + map[l + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[l + 2] + 32) << cnt) | (*(sprite + map[l + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[l + 3]) << cnt) | (*(sprite + map[l + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[l + 3] + 32) << cnt) | (*(sprite + map[l + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[l + 4]) << cnt) | (*(sprite + map[l + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[l + 4] + 32) << cnt) | (*(sprite + map[l + 5] + 32) >> cnt2);

			light += 30;
			dark += 30;
			sprite++;
			s = (s + 1) & 15;
		}
	}

	while(i < hieght) {
		sprite = (long *)bg_tile;
		loc += 8;

		for(h = 0 ; h < 32 && i < hieght ; h++, i++) {
			cnt = glbs->camera.bg_x_off + sine_wave[s];
			if(cnt > 31) {cnt -= 32; l = loc + 1;}
			else l = loc;
			cnt2 = 32 - cnt;

			*(long *)(light) = (*(sprite + map[l]) << cnt) | (*(sprite + map[l + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[l] + 32) << cnt) | (*(sprite + map[l + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[l + 1]) << cnt) | (*(sprite + map[l + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[l + 1] + 32) << cnt) | (*(sprite + map[l + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[l + 2]) << cnt) | (*(sprite + map[l + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[l + 2] + 32) << cnt) | (*(sprite + map[l + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[l + 3]) << cnt) | (*(sprite + map[l + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[l + 3] + 32) << cnt) | (*(sprite + map[l + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[l + 4]) << cnt) | (*(sprite + map[l + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[l + 4] + 32) << cnt) | (*(sprite + map[l + 5] + 32) >> cnt2);

			light += 30;
			dark += 30;
			sprite++;
			s = (s + 1) & 15;
		}
	}

	light_src = glbs->light_buffer;
	dark_src = glbs->dark_buffer;

	for(; i < footer_level ; i++, light += 30, dark += 30, light_src += 30, dark_src += 30) {
		*(long *)(light) = *(long *)(light_src);
		*(long *)(dark) = *(long *)(dark_src);
		*(long *)(light + 4) = *(long *)(light_src + 4);
		*(long *)(dark + 4) = *(long *)(dark_src + 4);
		*(long *)(light + 8) = *(long *)(light_src + 8);
		*(long *)(dark + 8) = *(long *)(dark_src + 8);
		*(long *)(light + 12) = *(long *)(light_src + 12);
		*(long *)(dark + 12) = *(long *)(dark_src + 12);
		*(long *)(light + 16) = *(long *)(light_src + 16);
		*(long *)(dark + 16) = *(long *)(dark_src + 16);
	}

	if(i >= 100) return;

	map_data = (unsigned char *)(metpack_base + bg->footer);
	for(y = 0 ; y < bg->footer_hieght ; y++, i += 32) {
		for(x = 0 ; x < 5 ; x++) {
			sprite = (unsigned long *)bg_tile + map_data[y * 5 + x] * 64;
			light = glbs->light_buffer + 30 * i + 4 * x;
			dark = glbs->dark_buffer + 30 * i + 4 * x;
			for(h = i ; h < 100 ; h++) {
				*(long *)light = *sprite;
				*(long *)dark = *(sprite + 32);
				sprite++;
				light += 30;
				dark += 30;
			}
		}
	}
}
*/



/*void draw_bg(unsigned char *bg_tile, BACKGROUND_HEADER *bg, void *metpack_base)
{
	unsigned char *map_data;
	short cnt, cnt2;
	short i, h, s, x_pos, y_pos, x, y, loc, l;
	unsigned char *light, *dark, *light_src, *dark_src;
	unsigned long *sprite;
	unsigned short map[8 * 5];
	short hieght = 32 * bg->hieght;
	short water_level = water.top - glbs->camera.y;
	short footer_level = (100 - bg->footer_hieght * 32) +
		((long)(glbs->current_map.hieght * 12 - 100 - glbs->camera.y) * bg->scroll_y) / 100;
	char next_line[101];
	short gap_start = 0;
	short line_skip = -glbs->camera.y_off;
	short first_break = 0xff;
	short map_loc;
	short map_loc_start = glbs->camera.tile_y * glbs->current_map.width + glbs->camera.tile_x;

	//memset(glbs->light_buffer, 0x55, 3000);
	//memset(glbs->dark_buffer, 0xaa, 3000);

	memset(next_line, 1, 101);

	i = -glbs->camera.y_off;
	if(hieght == 32) { //don't skip any of upper 32 rows for map with hieght = 1
		while(i < 32) {
			i += 12;
			map_loc_start += glbs->current_map.width;
		}
		gap_start = i;
	}

	//look for rows full of tiles; don't need to draw bg if it will just be covered up
	for(; i < 100 ; i += 12) {
		map_loc = map_loc_start;
		map_loc_start += glbs->current_map.width;
		for(x = -glbs->camera.x_off ; x < 160 ; x += 12, map_loc++) {
			if(!tile_data[glbs->current_map.data[map_loc]].blit) {
				if(line_skip > 0) {
					next_line[gap_start] = line_skip + 1;
					if(first_break == 0xff) first_break = gap_start; //first pixel row of bg to be skipped
				}
				line_skip = -12;
				gap_start = i + 12;
				//clrscr();
				//printf("%d:%d", i, x);
				//pause();
				break;
			}
		}
		line_skip += 12;
	}
	if(gap_start < 100) next_line[gap_start] = line_skip + 1;

	if(bg->footer == 0 || footer_level > 100) footer_level = 100;
	else if(footer_level < 0) footer_level = 0;

	if(hieght < 100 - first_break) hieght = 100; //can't copy from top of bg if there are breaks =(
	else if(hieght > footer_level) hieght = footer_level;
	else if(hieght > 100) hieght = 100;

	if(water_level <= 0) water_level = 0;
	else if(water_level >= 100) water_level = 100;
	else hieght = 100;

	map_data = (unsigned char *)(metpack_base + bg->data);
	y_pos = glbs->camera.bg_tile_y;

	for(y = 0 ; y < 5 ; y++) {
		x_pos = glbs->camera.bg_tile_x - 1;
		if(x_pos < 0) x_pos = bg->width - 1;

		for(x = 0 ; x < 8 ; x++) {
			map[y * 8 + x] = map_data[y_pos * bg->width + x_pos] * 64;
			x_pos++;
			if(x_pos == bg->width) x_pos = 0;
		}
		y_pos++;
		if(y_pos == bg->hieght) y_pos = 0;
	}

	next_line[0]--;
	light = glbs->light_buffer + next_line[0] * 30;
	dark = glbs->dark_buffer + next_line[0] * 30;
	sprite = (long *)bg_tile + glbs->camera.bg_y_off + next_line[0];
	i = next_line[0] + 1;
	h = glbs->camera.bg_y_off + next_line[0];
	s = (glbs->camera.bg_wave + water_level + next_line[0]) & 15;
	cnt = glbs->camera.bg_x_off;
	cnt2 = 32 - cnt;
	loc = 1;

	if(0) {
		char *table_ptr = (char *)ripple_table + glbs->camera.bg_ripple * 32;

		hieght = 100;
		sprite = (long *)(bg_tile);
		for(i = 0 ; i < glbs->camera.bg_y_off ; i++) sprite += table_ptr[i];

		for(i = 0, h = glbs->camera.bg_y_off ; h < 32 && i < water_level ; i++, h++) {
			sprite += table_ptr[h];

			*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

			light += 30;
			dark += 30;
		}

		while(i < hieght) {
			sprite = (long *)bg_tile;
			loc += 8;

			for(h = 0 ; h < 32 && i < hieght; h++, i++) {
				sprite += table_ptr[h];

				*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
				*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

				*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
				*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

				*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
				*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

				*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
				*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

				*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
				*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

				light += 30;
				dark += 30;
			}
		}

		return;
	}

	for(; h < 32 && i <= water_level;) {
		*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
		*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

		*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
		*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

		*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
		*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

		*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
		*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

		*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
		*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

		if(next_line[i] == 1) {
			i++; h++;
			light += 30; dark += 30; sprite++;
		} else {
			short tmp = next_line[i];
			i += tmp; h += tmp;
			light += 30 * tmp; dark += 30 * tmp; sprite += tmp;
		}
	}

	while(i <= water_level && i <= hieght) {
		sprite -= 32; h -= 32; //assuming there was no 'overshoot', this will reset sprite to begining
		if(h >= 64) { sprite -= 32; h -= 32; } //overshot by more than 31, keep sprite in boudns
		if(h >= 96) { sprite -= 32; h -= 32; } //dito
		loc += 8;

		for(; h < 32 && i <= water_level && i <= hieght;) {
			*(long *)(light) = (*(sprite + map[loc]) << cnt) | (*(sprite + map[loc + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[loc] + 32) << cnt) | (*(sprite + map[loc + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[loc + 1]) << cnt) | (*(sprite + map[loc + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[loc + 1] + 32) << cnt) | (*(sprite + map[loc + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[loc + 2]) << cnt) | (*(sprite + map[loc + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[loc + 2] + 32) << cnt) | (*(sprite + map[loc + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[loc + 3]) << cnt) | (*(sprite + map[loc + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[loc + 3] + 32) << cnt) | (*(sprite + map[loc + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[loc + 4]) << cnt) | (*(sprite + map[loc + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[loc + 4] + 32) << cnt) | (*(sprite + map[loc + 5] + 32) >> cnt2);

			if(next_line[i] == 1) {
				i++; h++;
				light += 30; dark += 30; sprite++;
			} else {
				short tmp = next_line[i];
				i += tmp; h += tmp;
				light += 30 * tmp; dark += 30 * tmp; sprite += tmp;
			}
		}
	}

	if(h < 32 && i <= hieght) {
		for(; h < 32 && i <= hieght;) {
			cnt = glbs->camera.bg_x_off + sine_wave[s];
			if(cnt > 31) {cnt -= 32; l = loc + 1;}
			else l = loc;
			cnt2 = 32 - cnt;

			*(long *)(light) = (*(sprite + map[l]) << cnt) | (*(sprite + map[l + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[l] + 32) << cnt) | (*(sprite + map[l + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[l + 1]) << cnt) | (*(sprite + map[l + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[l + 1] + 32) << cnt) | (*(sprite + map[l + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[l + 2]) << cnt) | (*(sprite + map[l + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[l + 2] + 32) << cnt) | (*(sprite + map[l + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[l + 3]) << cnt) | (*(sprite + map[l + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[l + 3] + 32) << cnt) | (*(sprite + map[l + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[l + 4]) << cnt) | (*(sprite + map[l + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[l + 4] + 32) << cnt) | (*(sprite + map[l + 5] + 32) >> cnt2);

			if(next_line[i] == 1) {
				i++; h++;
				light += 30; dark += 30; sprite++;
				s = (s + 1) & 15;
			} else {
				short tmp = next_line[i];
				i += tmp; h += tmp;
				light += 30 * tmp; dark += 30 * tmp; sprite += tmp;
				s = (s + tmp) & 15;
			}

		}
	}

	while(i <= hieght) {
		sprite -= 32; h -= 32; //assuming there was no 'overshoot', this will reset sprite to begining
		if(h >= 64) { sprite -= 32; h -= 32; } //overshot by more than 31, keep sprite in boudns
		if(h >= 96) { sprite -= 32; h -= 32; } //dito
		loc += 8;

		for(; h < 32 && i <= hieght;) {
			cnt = glbs->camera.bg_x_off + sine_wave[s];
			if(cnt > 31) {cnt -= 32; l = loc + 1;}
			else l = loc;
			cnt2 = 32 - cnt;

			*(long *)(light) = (*(sprite + map[l]) << cnt) | (*(sprite + map[l + 1]) >> cnt2);
			*(long *)(dark) = (*(sprite + map[l] + 32) << cnt) | (*(sprite + map[l + 1] + 32) >> cnt2);

			*(long *)(light + 4) = (*(sprite + map[l + 1]) << cnt) | (*(sprite + map[l + 2]) >> cnt2);
			*(long *)(dark + 4) = (*(sprite + map[l + 1] + 32) << cnt) | (*(sprite + map[l + 2] + 32) >> cnt2);

			*(long *)(light + 8) = (*(sprite + map[l + 2]) << cnt) | (*(sprite + map[l + 3]) >> cnt2);
			*(long *)(dark + 8) = (*(sprite + map[l + 2] + 32) << cnt) | (*(sprite + map[l + 3] + 32) >> cnt2);

			*(long *)(light + 12) = (*(sprite + map[l + 3]) << cnt) | (*(sprite + map[l + 4]) >> cnt2);
			*(long *)(dark + 12) = (*(sprite + map[l + 3] + 32) << cnt) | (*(sprite + map[l + 4] + 32) >> cnt2);

			*(long *)(light + 16) = (*(sprite + map[l + 4]) << cnt) | (*(sprite + map[l + 5]) >> cnt2);
			*(long *)(dark + 16) = (*(sprite + map[l + 4] + 32) << cnt) | (*(sprite + map[l + 5] + 32) >> cnt2);

			if(next_line[i] == 1) {
				i++; h++;
				light += 30; dark += 30; sprite++;
				s = (s + 1) & 15;
			} else {
				short tmp = next_line[i];
				i += tmp; h += tmp;
				light += 30 * tmp; dark += 30 * tmp; sprite += tmp;
				s = (s + tmp) & 15;
			}

		}
	}

	light_src = glbs->light_buffer;
	dark_src = glbs->dark_buffer;

	//clrscr();
	//pause();

	while(i <= footer_level) {
		//clrscr();
		//printf("%d", i);
		//pause();

		*(long *)(light) = *(long *)(light_src);
		*(long *)(dark) = *(long *)(dark_src);
		*(long *)(light + 4) = *(long *)(light_src + 4);
		*(long *)(dark + 4) = *(long *)(dark_src + 4);
		*(long *)(light + 8) = *(long *)(light_src + 8);
		*(long *)(dark + 8) = *(long *)(dark_src + 8);
		*(long *)(light + 12) = *(long *)(light_src + 12);
		*(long *)(dark + 12) = *(long *)(dark_src + 12);
		*(long *)(light + 16) = *(long *)(light_src + 16);
		*(long *)(dark + 16) = *(long *)(dark_src + 16);

		if(next_line[i] == 1) {
			i++;
			light += 30; dark += 30;
			light_src += 30; dark_src += 30;
		} else {
			short tmp = next_line[i];
			i += tmp;
			light += 30 * tmp; dark += 30 * tmp;
			light_src += 30 * tmp; dark_src += 30 * tmp;
		}
	}

	if(footer_level >= 100) return;
	i = footer_level;

	map_data = (unsigned char *)(metpack_base + bg->footer);
	for(y = 0 ; y < bg->footer_hieght ; y++, i += 32) {
		for(x = 0 ; x < 5 ; x++) {
			sprite = (unsigned long *)bg_tile + map_data[y * 5 + x] * 64;
			light = glbs->light_buffer + 30 * i + 4 * x;
			dark = glbs->dark_buffer + 30 * i + 4 * x;
			for(h = i ; h <= 100 ; h++) {
				*(long *)light = *sprite;
				*(long *)dark = *(sprite + 32);
				sprite++;
				light += 30;
				dark += 30;
			}
		}
	}
}*/

void draw_map0()
{
	TILE_NODE_CLIPPED *tile_ptr_clipped = glbs->tile_list_clipped;
	TILE_NODE *tile_ptr = glbs->tile_list;
	unsigned char *data = glbs->current_map.data;
	unsigned char *prop_data = glbs->current_map.prop_data;
	short x_pos;
	short y_pos;
	short x = glbs->camera.tile_x;
	short map_loc;
	short map_loc_start = glbs->camera.tile_y * glbs->current_map.width + glbs->camera.tile_x;
	short tile1;
	//short tile2;
	short h;
	unsigned short* sprite0;
	short cnt;

	long dark;
	long light;
	short offset;

	for(x_pos = -glbs->camera.x_off ; x_pos < glbs->display_width ; x++, x_pos += 12, map_loc_start++) {
		y_pos = -glbs->camera.y_off;
		offset = 0;

		if(x_pos < 0)
			cnt = -x_pos+16;
    else {
    	cnt = 16-(x_pos&15);
    	offset += ((x_pos>>3)&0x1e);
    }

    light = (long)glbs->light_buffer + offset;
    dark = (long)glbs->dark_buffer + offset;

    map_loc = map_loc_start;

    //=======================================================================
    //FIRST BLOCK (Clipping support)
    //=======================================================================

    tile1 = data[map_loc];

		if(tile1 > 0) {
			short blit = glbs->tile_data[tile1].blit;
			sprite0 = glbs->tile_data[tile1].data - y_pos * 2;
			if(!blit) sprite0 -= y_pos;

			h = 12 + y_pos;

			if(!(prop_data[map_loc] & 0x80)) {
				tile_ptr_clipped->tile = sprite0;
				tile_ptr_clipped->offset = offset;
				tile_ptr_clipped->cnt = cnt;
				tile_ptr_clipped->h = h;
				tile_ptr_clipped->blit = blit;
				tile_ptr_clipped++;

				light += 30 * h;
				dark += 30 * h;
				offset += 30 * h;
			} else {
				offset += 30 * h;

				if(blit) {
					long mask = ~(65520 << cnt);
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
					}
				} else {
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
					}
				}
			}

		} else {
			light += 30 * (12 + y_pos);
			dark += 30 * (12 + y_pos);
			offset += 30 * (12 + y_pos);
		}

		y_pos += 12;
		map_loc += glbs->current_map.width;

		//=======================================================================
    //MIDDLE BLOCKS (No clipping)
    //=======================================================================
		for( ; y_pos < glbs->display_hieght - 12 ; y_pos += 12, map_loc += glbs->current_map.width) {

			tile1 = data[map_loc];

			if(tile1 > 0) {

				if(!(prop_data[map_loc] & 0x80)) {
					tile_ptr->tile = tile1;
					tile_ptr->offset = offset;
					tile_ptr->cnt = cnt;
					tile_ptr++;

					light += 30 * 12;
					dark += 30 * 12;
					offset += 30 * 12;
				} else {
					sprite0 = glbs->tile_data[tile1].data;
					offset += 30 * 12;

					if(glbs->tile_data[tile1].blit) {
						long mask = ~(65520 << cnt);
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
					} else {
						//for (h = 12 ; h ; h--, light += 30, dark += 30) {
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
						light += 30; dark += 30;
						//}
					}
				}

			} else {
				light += 30 * 12;
				dark += 30 * 12;
				offset += 30 * 12;
			}
		}

		//=======================================================================
    //LAST BLOCK (Clipping support)
    //=======================================================================

		tile1 = data[map_loc];

		if(tile1 > 0) {
			sprite0 = glbs->tile_data[tile1].data;

			h = (glbs->display_hieght - y_pos);

			if(!(prop_data[map_loc] & 0x80)) {
				tile_ptr_clipped->tile = sprite0;
				tile_ptr_clipped->offset = offset;
				tile_ptr_clipped->cnt = cnt;
				tile_ptr_clipped->h = h;
				tile_ptr_clipped->blit = glbs->tile_data[tile1].blit;
				tile_ptr_clipped++;
			} else {

				if(glbs->tile_data[tile1].blit) {
					long mask = ~(65520 << cnt);
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= mask; *(long*)dark &= mask;
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
					}
				} else {
					for (; h ; h--, light += 30, dark += 30) {
						*(long*)light &= ~((long)(*sprite0)<<cnt); *(long*)dark &= ~((long)(*sprite0++)<<cnt);
						*(long*)light |= (long)(*sprite0++)<<cnt; *(long*)dark |= (long)(*sprite0++)<<cnt;
					}
				}
			}

		}
	}

	tile_ptr_clipped->offset = -1;
	tile_ptr->offset = -1;
}

unsigned char get_data(short x, short y)
{
	return glbs->current_map.data[y * glbs->current_map.width + x];
}

unsigned char get_prop_data(short x, short y)
{
	return glbs->current_map.prop_data[y * glbs->current_map.width + x] & 0x7f;
}

void set_data(short x, short y, unsigned char a)
{
	if(x >= 0 && x < glbs->current_map.width && y >= 0 && y < glbs->current_map.hieght)
		glbs->current_map.data[y * glbs->current_map.width + x] = a;
}

void set_prop_data(short x, short y, unsigned char a)
{
	if(x >= 0 && x < glbs->current_map.width && y >= 0 && y < glbs->current_map.hieght)
		glbs->current_map.prop_data[y * glbs->current_map.width + x] = a;
}


