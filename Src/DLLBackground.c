// C Source File
// Created 2/11/2004; 8:32:50 PM

#define __IN_DLL__
#include <tigcclib.h>         // Include All Header Files
#include "stdlib.h"
#include "dllmap.h"
#include "dllutility.h"
#include "dlltiles.h"
#include "dllenvironment.h"
#include "globals.h"

const char sine_wave[] =	{2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 1, 0, 0, 0, 1, 1};

const char ripple_table[16 * 8] = {
	0, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, /*0, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2,*/
	0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, /*0, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1,*/
	1, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, /*1, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1,*/
	0, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, /*0, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0,*/
	0, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, /*0, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0,*/
	0, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2, /*0, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, 2,*/
	0, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, /*0, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1,*/
	1, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1, /*1, 1, 0, 1, 0, 1, 2, 1, 2, 1, 0, 1, 0, 1, 2, 1,*/
};

void scroll_left(unsigned short* buffer,unsigned short lines) {
    register short* tmpbuffer = buffer;
    register short  tmplines  = lines;

    tmpbuffer += (tmplines<<3) + (tmplines<<2);
    tmplines--;

    asm volatile ("0:\n"
        "lsl.w  -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0)\n"
        "roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0);roxl.w -(%0)\n"
        "dbf %1,0b"
        : "=a" (tmpbuffer), "=d" (tmplines)
        : "0"  (tmpbuffer), "1"  (tmplines));
}

void scroll_right(unsigned short* buffer,unsigned short lines) {
    register short* tmpbuffer = buffer;
    register short  tmplines  = lines;

    tmplines--;

    asm volatile ("0:\n"
        "lsr.w  (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+\n"
        "roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+;roxr.w (%0)+\n"
        "dbf %1,0b"
        : "=a" (tmpbuffer), "=d" (tmplines)
        : "0"  (tmpbuffer), "1"  (tmplines));
}

void sprite_fill(short col)
{
	short *light, *dark, *sprite0, *sprite1;
	short i, y;
	short map_pos = glbs->camera.bg_tile_x + col - 1;
	char *bg_map = glbs->metpack_base + glbs->bg_list[glbs->current_map.background].data;
	short bg_width = glbs->bg_list[glbs->current_map.background].width;

	while(map_pos >= bg_width) map_pos -= bg_width;
	if(map_pos < 0) map_pos += bg_width;

	light = (short *)glbs->bg_light + col;
	dark = (short *)glbs->bg_dark + col;

	for(y = 0 ; y < glbs->bg_list[glbs->current_map.background].hieght ; y++, map_pos += bg_width) {
		sprite0 = (short *)(glbs->bg_tile + 64 * bg_map[map_pos]);
		sprite1 = sprite0 + 16;
		for(i = 0 ; i < 16 ; i++) {
			*light = *sprite0++;
			*dark = *sprite1++;
			(char *)light += 24; (char *)dark += 24;
		}
	}
}

void draw_footer()
{
	BACKGROUND_HEADER *bg = &glbs->bg_list[glbs->current_map.background];
	char *footer_map = glbs->metpack_base + bg->footer;
	short map_pos;
	short x, y, i;
	short *light, *dark, *sprite0, *sprite1;

	for(x = 0 ; x < 10 ; x++) {
		light = (short *)glbs->footer_light + x;
		dark = (short *)glbs->footer_dark + x;
		map_pos = x;
		for(y = 0 ; y < bg->footer_hieght ; y++, map_pos += 10) {
			sprite0 = (short *)(glbs->bg_tile + 64 * footer_map[map_pos]);
			sprite1 = sprite0 + 16;
			for(i = 0 ; i < 16 ; i++) {
				*light = *sprite0++;
				*dark = *sprite1++;
				(char *)light += 20; (char *)dark += 20;
			}
		}
	}
}

char bg_setup()
{
	BACKGROUND_HEADER *bg = &glbs->bg_list[glbs->current_map.background];
	short x;
	short size = bg->hieght * 16 * 24 * 2;

	if(bg->footer != 0) size += bg->footer_hieght * 16 * 20 * 2;

	glbs->bg_light = malloc(size);
	if(glbs->bg_light == NULL) return FALSE;
	glbs->bg_dark = glbs->bg_light + bg->hieght * 16 * 24;
	if(bg->footer != 0) {
		glbs->footer_light = glbs->bg_dark + bg->hieght * 16 * 24;
		glbs->footer_dark = glbs->footer_light + bg->footer_hieght * 16 * 20;
		draw_footer();
	} else {
		glbs->footer_light = glbs->footer_dark = NULL;
	}

	for(x = 0 ; x < 12 ; x++) sprite_fill(x);
	for(x = 0 ; x < glbs->camera.bg_x_off ; x++) {
		scroll_left((short *)glbs->bg_light, glbs->bg_list[glbs->current_map.background].hieght * 16);
		scroll_left((short *)glbs->bg_dark, glbs->bg_list[glbs->current_map.background].hieght * 16);
	}

	glbs->camera.old_bg_x = glbs->camera.bg_x_off;
	glbs->camera.bg_x_moved = 0;

	return TRUE;
}

void bg_reset()
{
	if(glbs->bg_light != NULL) {
		free(glbs->bg_light);
		glbs->bg_light = NULL;
	}
}

void bg_cleanup()
{
	bg_reset();
}

void bg_draw()
{
	BACKGROUND_HEADER *bg = &glbs->bg_list[glbs->current_map.background];
	short i = 0, y = 0, h;
	unsigned long *light_src, *dark_src, *light, *dark;
	short footer_level;
	short water_level = glbs->water.top - glbs->camera.y;
	short hieght;
	char *table_ptr;

	if(bg->footer == 0) footer_level = 100;
	else {
		footer_level = (100 - bg->footer_hieght * 16) +
		((long)(glbs->current_map.hieght * 12 - 100 - glbs->camera.y) * bg->scroll_y) / 100;
		if(footer_level > 100) footer_level = 100;
		else if(footer_level < 0) footer_level = 0;
	}

	if(water_level < 0) water_level = 0;
	else if(water_level > 100) water_level = 100;

	if(water_level < footer_level) hieght = water_level;
	else hieght = footer_level;

	if(glbs->camera.old_bg_x != glbs->camera.bg_x_off) {

		if(glbs->camera.bg_x_moved > 0) {
			for(i = 0 ; i < glbs->camera.bg_x_moved ; i++) {
				scroll_left((short *)glbs->bg_light, bg->hieght * 16);
				scroll_left((short *)glbs->bg_dark, bg->hieght * 16);
				glbs->camera.old_bg_x++; if(glbs->camera.old_bg_x > 15) glbs->camera.old_bg_x = 0;
				if(glbs->camera.old_bg_x == 0) sprite_fill(11);
			}
		} else {
			for(i = 0 ; i < -glbs->camera.bg_x_moved ; i++) {
				scroll_right((short *)glbs->bg_light, bg->hieght * 16);
				scroll_right((short *)glbs->bg_dark, bg->hieght * 16);
				glbs->camera.old_bg_x--; if(glbs->camera.old_bg_x < 0) glbs->camera.old_bg_x = 15;
				if(glbs->camera.old_bg_x == 0) sprite_fill(0);
			}
		}
		glbs->camera.bg_x_moved = 0;
		glbs->camera.old_bg_x = glbs->camera.bg_x_off;
	}

	i = 16 * glbs->camera.bg_tile_y + glbs->camera.bg_y_off;
	light = glbs->light_buffer; dark = glbs->dark_buffer;
	light_src = (long *)(glbs->bg_light + i * 24 + 2);
	dark_src = (long *)(glbs->bg_dark + i * 24 + 2);
	/*if(1) {
		table_ptr = (char *)ripple_table + glbs->camera.bg_ripple * 16; h = 0;
		while(y < hieght) {
			while(y < hieght) {
				(char *)light_src += 24 * table_ptr[h]; (char *)dark_src += 24 * table_ptr[h];
				i += table_ptr[h];  h = (h + 1) & 15;
				if(i >= bg->hieght * 16) break;
				
				*light++ = *light_src; *dark++ = *dark_src;
				*light++ = *(light_src+1); *dark++ = *(dark_src+1);
				*light++ = *(light_src+2); *dark++ = *(dark_src+2);
				*light++ = *(light_src+3); *dark++ = *(dark_src+3);
				*light = *(light_src+4); *dark = *(dark_src+4);
				(char *)light += 14; (char *)dark += 14;
				y++;
			}
			if(y < hieght) {
				i = 0; light_src = (long *)(glbs->bg_light + 2); dark_src = (long *)(glbs->bg_dark + 2);
			}
		}
		return;
	}*/
		
	
	
	while(y < hieght) {
		while(i < bg->hieght * 16 && y < hieght) {
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light = *light_src; *dark = *dark_src;
			(char *)light += 14; (char *)dark += 14;
			(char *)light_src += 8; (char *)dark_src += 8;
			i++; y++;
		}
		if(y < hieght) {
			i = 0; light_src = (long *)(glbs->bg_light + 2); dark_src = (long *)(glbs->bg_dark + 2);
		}
	}
	if(footer_level < 100) {
		light_src = (long *)(glbs->footer_light);
		dark_src = (long *)(glbs->footer_dark);
		while(y < 100) {
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light++ = *light_src++; *dark++ = *dark_src++;
			*light = *light_src++; *dark = *dark_src++;
			(char *)light += 14; (char *)dark += 14;
			y++;
		}
	} else if(water_level < 100) {
		short s = (glbs->camera.bg_wave + water_level) & 15;
		short cnt0, cnt1;
		while(y < 100) {
			while(i < bg->hieght * 16 && y < 100) {
				cnt0 = sine_wave[s]; cnt1 = 16 - cnt0;
				*light = (*light_src++) << cnt0; *((unsigned short *)light + 1) |= *(unsigned short *)light_src >> cnt1;
				*++light = (*light_src++) << cnt0; *((unsigned short *)light + 1) |= *(unsigned short *)light_src >> cnt1;
				*++light = (*light_src++) << cnt0; *((unsigned short *)light + 1) |= *(unsigned short *)light_src >> cnt1;
				*++light = (*light_src++) << cnt0; *((unsigned short *)light + 1) |= *(unsigned short *)light_src >> cnt1;
				*++light = (*light_src++) << cnt0; *((unsigned short *)light + 1) |= *(unsigned short *)light_src >> cnt1;

				*dark = (*dark_src++) << cnt0; *((unsigned short *)dark + 1) |= *(unsigned short *)dark_src >> cnt1;
				*++dark = (*dark_src++) << cnt0; *((unsigned short *)dark + 1) |= *(unsigned short *)dark_src >> cnt1;
				*++dark = (*dark_src++) << cnt0; *((unsigned short *)dark + 1) |= *(unsigned short *)dark_src >> cnt1;
				*++dark = (*dark_src++) << cnt0; *((unsigned short *)dark + 1) |= *(unsigned short *)dark_src >> cnt1;
				*++dark = (*dark_src++) << cnt0; *((unsigned short *)dark + 1) |= *(unsigned short *)dark_src >> cnt1;

				(char *)light += 14; (char *)dark += 14;
				(char *)light_src += 4; (char *)dark_src += 4;
				i++; y++; s = (s + 1) & 15;
			}
			i = 0; light_src = (long *)(glbs->bg_light + 2); dark_src = (long *)(glbs->bg_dark + 2);
		}
	}
}


