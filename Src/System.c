// C Source File
// Created 10/4/2002; 7:45:18 PM

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>

#include "utility.h"
#include "system.h"
#include "entity.h"
#include "player.h"
#include "tiles.h"
#include "enemy.h"
#include "door.h"
#include "map.h"
#include "bar.h"
#include "miscgfx.h"

typedef struct {
	short x;
	short y;
} POSITION;

extern void screen_draw();

const char font_width[58] = {
	1, 3, 5, 4, 5, 5, 1, 2, 2, 5, 5, 2, 4, 1, 5,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 4, 3, 4, 5,
	4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 3, 5, 4, 4, 4, 5, 4, 4, 5, 4, 4, 5, 3, 3, 4
};

unsigned char button_gfx[16] = {
	//sprite 0
	0x70,0xb8,0xf8,0x70,
	0x10,0x08,0x08,0x10,
	//sprite 1
	0x60,0xf0,0xf0,0x60,
	0x70,0xb8,0xf8,0x70,
};

unsigned char box_corner[48] = {
	//sprite 0
	0x00,0x10,0x20,0x40,
	0x30,0x60,0xd0,0xb0,
	0xcf,0x8f,0x0f,0x0f,
	//sprite 1
	0x00,0x80,0x40,0x20,
	0xc0,0x60,0xb0,0xd0,
	0x3f,0x1f,0x0f,0x0f,
	//sprite 2
	0x40,0x20,0x10,0x00,
	0xb0,0xd0,0x60,0x30,
	0x0f,0x0f,0x8f,0xcf,
	//sprite 3
	0x20,0x40,0x80,0x00,
	0xd0,0xb0,0x60,0xc0,
	0x0f,0x0f,0x1f,0x3f,
};


short string_width(const char *txt)
{
	short i;
	short len = strlen(txt);
	short w = 0;
	char c;
	
	for(i = 0 ; i < len ; i++) {
		c = txt[i] - 32;
		if(c == 0) w += 3;
		if(c < 0) break;
		if(c > 0) w += font_width[c - 1] + 1;
	}
	
	return w - 1;
}

void draw_vline(short x, short y1, short y2, short color)
{
	if(color == COLOR_LIGHTGRAY || color == COLOR_BLACK) FastDrawLine(light_buffer, x, y1, x, y2, A_NORMAL);
	if(color == COLOR_DARKGRAY || color == COLOR_BLACK) FastDrawLine(dark_buffer, x, y1, x, y2, A_NORMAL);
	if(color == COLOR_LIGHTGRAY || color == COLOR_WHITE) FastDrawLine(dark_buffer, x, y1, x, y2, A_REVERSE);
	if(color == COLOR_DARKGRAY || color == COLOR_WHITE) FastDrawLine(light_buffer, x, y1, x, y2, A_REVERSE);
}

void draw_hline(short x1, short x2, short y, short color)
{
	if(color == COLOR_LIGHTGRAY || color == COLOR_BLACK) FastDrawHLine(light_buffer, x1, x2, y, A_NORMAL);
	if(color == COLOR_DARKGRAY || color == COLOR_BLACK) FastDrawHLine(dark_buffer, x1, x2, y, A_NORMAL);
	if(color == COLOR_LIGHTGRAY || color == COLOR_WHITE) FastDrawHLine(dark_buffer, x1, x2, y, A_REVERSE);
	if(color == COLOR_DARKGRAY || color == COLOR_WHITE) FastDrawHLine(light_buffer, x1, x2, y, A_REVERSE);
}

void draw_string(short x, short y, const char *txt, short mode)
{
	short x_pos = x;
	short i;
	short len = strlen(txt);
	char c;
	
	for(i = 0 ; i < len ; i++) {
		c = txt[i] - 32;
		if(c == 0) x_pos += 3;
		if(c < 0) return;
		if(c > 0) {
			if(mode == A_NORMAL)
				GraySprite8_OR(x_pos, y, 6, font + c * 6 - 6, font + c * 6 - 6, light_buffer, dark_buffer);
			else if(mode == A_XOR)
				GraySprite8_XOR(x_pos, y, 6, font + c * 6 - 6, font + c * 6 - 6, light_buffer, dark_buffer);
			x_pos += font_width[c - 1] + 1;
		}
	}
}

void draw_number(short x, short y, unsigned char number, char digits, void *light, void *dark)
{
	short i;
	short d = 1;
	short x_pos = x;
	char n;
	
	if(digits == 2) d = 10;
	if(digits == 3) d = 100;
	
	for(i = 0 ; i < digits ; i++) {
		n = number / d;
	
		GraySprite8_OR(x_pos, y, 6,
			font + n * 6 + 15 * 6, font + n * 6 + 15 * 6, light, dark);
		
		x_pos += 4;
		
		//number %= d;
		//d /= 10;
		
		if(d == 100) {
			number -= n * 100;
			d = 10;
		} else if(d == 10) {
			number -= n * 10;
			d = 1;
		} else if(d == 1) {
			number -= n;
			d = 0;
		}
	}
}

void message(char *txt)
{
	short text_width = 0, w;
	short len = strlen(txt);
	short box_width;
	short box_hieght;
	short x, y;
	short i;
	short lines = 0;
	short offset[10];
	
	offset[0] = 0;
	for(i = 0 ; i < len + 1 ; i++)
		if(txt[i] == 1 || txt[i] == 0) {
	 		w = string_width(txt + offset[lines]);
	 		if(w > text_width) text_width = w;
	 		lines++;
	 		offset[lines] = i + 1;
	 	}
	 
	
	
	box_width = text_width + (text_width >> 1);
	box_hieght = lines * 7 + 1;
	box_hieght += (box_hieght >> 1);
	x = (160 - box_width) >> 1;
	y = (100 - box_hieght) >> 1;
	
	for(i = y ; i < y + box_hieght ; i++) draw_hline(x, x + box_width, i, COLOR_BLACK);
	
	x = (160 - text_width) >> 1;
	y = (99 - lines * 7) >> 1;
	
	for(i = 0 ; i < lines ; i++)
		draw_string(x, y + 7 * i, txt + offset[i], A_XOR);
	
	update_screen();
	pause();
	game_counter = 0;
}

void draw_border(short x, short y, short w, short h, const char *lable)
{
	short lable_width;
	short lable_position;
	short i;
	
	for(i = y + 3 ; i < y + h - 3 ; i++)
		draw_hline(x + 3, x + w - 4, i, COLOR_WHITE);
	
	GraySprite8_MASK(x, y, 4, box_corner, box_corner + 4, box_corner + 8, box_corner + 8,
		light_buffer, dark_buffer);
	GraySprite8_MASK(x + w - 4, y, 4, box_corner + 12, box_corner + 16, box_corner + 20, box_corner + 20,
		light_buffer, dark_buffer);
	GraySprite8_MASK(x, y + h - 4, 4, box_corner + 24, box_corner + 28, box_corner + 32, box_corner + 32,
		light_buffer, dark_buffer);
	GraySprite8_MASK(x + w - 4, y + h - 4, 4, box_corner + 36, box_corner + 40,
		box_corner + 44, box_corner + 44, light_buffer, dark_buffer);
	
	draw_vline(x, y + 4, y + h - 5, COLOR_DARKGRAY);
	draw_vline(x + 1, y + 4, y + h - 5, COLOR_LIGHTGRAY);
	draw_vline(x + 2, y + 4, y + h - 5, COLOR_DARKGRAY);
	
	draw_vline(x + w - 1, y + 4, y + h - 5, COLOR_DARKGRAY);
	draw_vline(x + w - 2, y + 4, y + h - 5, COLOR_LIGHTGRAY);
	draw_vline(x + w - 3, y + 4, y + h - 5, COLOR_DARKGRAY);
	
	draw_hline(x + 4, x + w - 5, y + h - 1, COLOR_DARKGRAY);
	draw_hline(x + 4, x + w - 5, y + h - 2, COLOR_LIGHTGRAY);
	draw_hline(x + 4, x + w - 5, y + h - 3, COLOR_DARKGRAY);
	
	if(lable == NULL) {
		draw_hline(x + 4, x + w - 5, y, COLOR_DARKGRAY);
		draw_hline(x + 4, x + w - 5, y + 1, COLOR_LIGHTGRAY);
		draw_hline(x + 4, x + w - 5, y + 2, COLOR_DARKGRAY);
	} else {
		lable_width = string_width(lable);
		lable_position = x + (w >> 1) - (lable_width >> 1);
		draw_string(lable_position, y - 2, lable, A_NORMAL);
		
		draw_hline(x + 4, lable_position - 4, y, COLOR_DARKGRAY);
		draw_hline(x + 4, lable_position - 4, y + 1, COLOR_LIGHTGRAY);
		draw_hline(x + 4, lable_position - 4, y + 2, COLOR_DARKGRAY);
		EXT_SETPIX(dark_buffer, lable_position - 3, y + 1);
		
		lable_position += lable_width;
		draw_hline(lable_position + 3, x + w - 5, y, COLOR_DARKGRAY);
		draw_hline(lable_position + 3, x + w - 5, y + 1, COLOR_LIGHTGRAY);
		draw_hline(lable_position + 3, x + w - 5, y + 2, COLOR_DARKGRAY);
		EXT_SETPIX(dark_buffer, lable_position + 2, y + 1);
	}
		
}

MENU *menu_create(short number)
{
	MENU *m;
	
	m = malloc(sizeof(MENU) + sizeof(MENU_SELECTION) * number);
	memset(m, 0, sizeof(MENU) + sizeof(MENU_SELECTION) * number);
	m->number = number;
	m->i = 0;
	m->selected = 0;
	m->data = (MENU_SELECTION *)((void *)m + sizeof(MENU));
	
	return m;
}

void menu_add_choice(MENU *m, short x, short y, char state, char *text)
{
	short i = m->i;
	
	if(i == m->number) return;
	
	m->data[i].x_pos = x;
	m->data[i].y_pos = y;
	m->data[i].state = state;
	strncpy(m->data[i].text, text, 20);
	m->data[i].text[19] = 0;
	m->i++;
}

void draw_button(MENU_SELECTION *s)
{
	GraySprite8_BLIT(s->x_pos, s->y_pos + 1, 4,
		button_gfx + s->state * 8, button_gfx + s->state * 8 + 4, 0b00000111, light_buffer, dark_buffer);
}

void draw_bar(MENU *m)
{
	MENU_SELECTION *s = &m->data[m->selected];
	short x = s->x_pos + 5;
	short y = s->y_pos - 1;
	short w = string_width(s->text) + 1;
	short i;
	
	for(i = 0 ; i < 7 ; i++) {
		FastDrawHLine(light_buffer, x, x + w, y + i, A_XOR);
		FastDrawHLine(dark_buffer, x, x + w, y + i, A_XOR);
	}
}

long menu_process(MENU *m)
{
	MENU_SELECTION *data = m->data;
	short i, j, dis, best_dis;
	unsigned short k;
	char repeat = 0;
	char sel = 0;
	char pressed;
	long r = 0;
	
	timer = 0;
	
	for(i = 0 ; i < m->number ; i++) {
		draw_button(data + i);
		draw_string(data[i].x_pos + 6, data[i].y_pos, data[i].text, A_NORMAL);
	}
	
	draw_bar(m);
	
	flipping_off();
	
	do {
		k = _rowread(ARROWS_ROW);
		if(k == 0) {
			timer = 0;
			repeat = 0;
		}
		
		draw_bar(m);
		
		if(timer == 0) {
			pressed = FALSE;
			
			if(k & UP_KEY) {
				m->selected--;
				pressed = TRUE;
			}
			
			if(k & DOWN_KEY) {
				m->selected++;
				pressed = TRUE;
			}
			
			if(k & RIGHT_KEY) {
				j = m->selected + 1;
				best_dis = 1000;
				for(i = 0 ; i < m->number ; i++) {
					if(data[i].x_pos <= data[m->selected].x_pos) continue;
					dis = data[i].y_pos - data[m->selected].y_pos;
					if(dis < 0) dis = -dis;
					if(dis < best_dis) {
						best_dis = dis;
						j = i;
					}
				}
				m->selected = j;
				pressed = TRUE;
			}
			
			if(k & LEFT_KEY) {
				j = m->selected - 1;
				best_dis = 1000;
				for(i = 0 ; i < m->number ; i++) {
					if(data[i].x_pos >= data[m->selected].x_pos) continue;
					dis = data[i].y_pos - data[m->selected].y_pos;
					if(dis < 0) dis = -dis;
					if(dis < best_dis) {
						best_dis = dis;
						j = i;
					}
				}
				m->selected = j;
				pressed = TRUE;
			}
			
			if(pressed) {
				if(repeat == 0) {
					timer = 200;
					repeat = 1;
				} else timer = 30;
			}
			
			if(m->selected < 0) m->selected = m->number - 1;
			if(m->selected == m->number) m->selected = 0;
					
		}
		
		if((k & SEL_KEY) && sel == 0) {
			data[m->selected].state = !data[m->selected].state;
			draw_button(data + m->selected);
			sel = 1;
		}
		
		if(!(k & SEL_KEY) && sel == 1) sel = 0;
		
		draw_bar(m);
		update_screen();
	} while(!(k & DMND_KEY));
	
	for(i = 0 ; i < m->number ; i++)
		if(data[i].state) r |= (1 << i);

	free(m);
	
	flipping_on();
	
	return r;
}

void item_menu()
{
	MENU *m;
	long r, new_items;
	short number = 0;
	short i, j;
	char s = 0;
	
	POSITION p[] = {
		{4, 62},
		{4, 69},
		{4, 76},
		{4, 83},
		{4, 90},
		
		{87, 19},
		{87, 26},
		
		{87, 42},
		{87, 49},
		{87, 56},
		{87, 63},
		{87, 70},
		{87, 77},
		{87, 83},
		{87, 90}
	};
	
	big_map_draw();
		
	for(i = 0 ; i < 32 ; i++)
		if(player.items_found & (1 << i)) number++;
	
	if(number == 0) return;
	
	m = menu_create(number);
	
	for(i = 0 ; i < 32 ; i++) {
		if(player.items_equiped & (1 << i))
			s = TRUE;
		else if(player.items_found & (1 << i))
			s = FALSE;
		else continue;
		
		menu_add_choice(m, p[i].x, p[i].y, s, item_names[i]);
	}
	
	memset(light_buffer, 0xff, 3000);
	memset(dark_buffer, 0xff, 3000);
	bar_active();
	bar_draw();
	draw_border(0, 57, 68, 43, NULL);
	draw_border(83, 14, 77, 22, NULL);
	draw_border(83, 37, 77, 63, NULL);
	
	r = menu_process(m);
	
	new_items = 0;
	j = 0;
	for(i = 0 ; i < 32 ; i++) {
		if(!(player.items_found & (1 << i))) continue;
		if(r & (1 << j)) new_items |= (1 << i);
		j++;
	}
	player.items_equiped = new_items;
	while(_rowread(0));
	game_counter = 0;
}
	
