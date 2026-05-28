// C Source File
// Created 11/14/2002; 5:05:00 PM

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>
#include "dllsystem.h"
#include "dllutility.h"
#include "globals.h"

//==================================================
//Constants for low-level keyboard access
//==================================================
#define ARROWS_ROW 0xfffe
#define LEFT_KEY 2
#define RIGHT_KEY 8
#define UP_KEY 1
#define DOWN_KEY 4
#define SEL_KEY 16
#define SHIFT_KEY 32
#define DMND_KEY 64
#define ESC_ROW 0xffbf
#define ESC_KEY 1
#define APPS_ROW 0xffdf
#define APPS_KEY 1
#define BSPACE_ROW 0xfffb
#define CLEAR_ROW 0xfffd
#define BSPACE_KEY 64
#define CLEAR_KEY 64

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
		else if(c > 0) w += font_width[c - 1] + 1;
		else break;
	}

	return w - 1;
}

void draw_vline(short x, short y1, short y2, short color)
{
	if(color == COLOR_LIGHTGRAY || color == COLOR_BLACK) FastDrawLine(glbs->light_buffer, x, y1, x, y2, A_NORMAL);
	if(color == COLOR_DARKGRAY || color == COLOR_BLACK) FastDrawLine(glbs->dark_buffer, x, y1, x, y2, A_NORMAL);
	if(color == COLOR_LIGHTGRAY || color == COLOR_WHITE) FastDrawLine(glbs->dark_buffer, x, y1, x, y2, A_REVERSE);
	if(color == COLOR_DARKGRAY || color == COLOR_WHITE) FastDrawLine(glbs->light_buffer, x, y1, x, y2, A_REVERSE);
}

void draw_hline(short x1, short x2, short y, short color)
{
	if(color == COLOR_LIGHTGRAY || color == COLOR_BLACK) FastDrawHLine(glbs->light_buffer, x1, x2, y, A_NORMAL);
	if(color == COLOR_DARKGRAY || color == COLOR_BLACK) FastDrawHLine(glbs->dark_buffer, x1, x2, y, A_NORMAL);
	if(color == COLOR_LIGHTGRAY || color == COLOR_WHITE) FastDrawHLine(glbs->dark_buffer, x1, x2, y, A_REVERSE);
	if(color == COLOR_DARKGRAY || color == COLOR_WHITE) FastDrawHLine(glbs->light_buffer, x1, x2, y, A_REVERSE);
}

void draw_string_ex(short x, short y, const char *txt, char *font, short mode, void *light, void *dark)
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
				GraySprite8_OR(x_pos, y, 6, font + c * 6 - 6, font + c * 6 - 6, light, dark);
			else if(mode == A_XOR)
				GraySprite8_XOR(x_pos, y, 6, font + c * 6 - 6, font + c * 6 - 6, light, dark);
			x_pos += font_width[c - 1] + 1;
		}
	}
}

void draw_string(short x, short y, const char *txt, char *font, short mode)
{
	draw_string_ex(x, y, txt, font, mode, glbs->light_buffer, glbs->dark_buffer);
}

void message(char *txt, char *font, short hieght)
{
	short text_width = string_width(txt);
	short x, y;
	short i;
	char buffer[30 * hieght * 2];
	short line, line_counter;
	short dy;
	short h = hieght / 2;

	x = 80 - text_width / 2;

	memset(buffer, 0xff, 30 * hieght * 2);
	draw_string_ex(x, 3, txt, font, A_XOR, buffer, buffer + 30 * hieght);

	flipping_off();

	for(i = 0 ; i <= h ; i++) {
		if(i == 0) dy = h * 100;
		else dy = (h * 100) / i;
		line_counter = 0;

		line = h;
		y = 46;

		while(y > 46 - i) {
			line_counter += dy;
			while(line_counter >= 100) {
				line_counter -= 100;
				line--;
			}
			y--;
		}
		line_counter = 0;

		while(y <= 46 + i) {
			memcpy(glbs->light_buffer + 30 * y, buffer + 30 * line, 30);
			memcpy(glbs->dark_buffer + 30 * y, buffer + 30 * hieght + 30 * line, 30);
			y++;
			line_counter += dy;
			while(line_counter >= 100) {
				line_counter -= 100;
				line++;
			}
		}
		update_screen();
		delay(15);
	}

	flipping_on();
	glbs->game_counter = 0;
}

void draw_border(short x, short y, short w, short h, char erase)
{
	//short lable_width;
	//short lable_position;
	short i;

	if(erase) {
		for(i = y + 3 ; i < y + h - 3 ; i++)
			draw_hline(x + 3, x + w - 4, i, COLOR_WHITE);
	}

	GraySprite8_MASK(x, y, 4, box_corner, box_corner + 4, box_corner + 8, box_corner + 8,
		glbs->light_buffer, glbs->dark_buffer);
	GraySprite8_MASK(x + w - 4, y, 4, box_corner + 12, box_corner + 16, box_corner + 20, box_corner + 20,
		glbs->light_buffer, glbs->dark_buffer);
	GraySprite8_MASK(x, y + h - 4, 4, box_corner + 24, box_corner + 28, box_corner + 32, box_corner + 32,
		glbs->light_buffer, glbs->dark_buffer);
	GraySprite8_MASK(x + w - 4, y + h - 4, 4, box_corner + 36, box_corner + 40,
		box_corner + 44, box_corner + 44, glbs->light_buffer, glbs->dark_buffer);

	draw_vline(x, y + 4, y + h - 5, COLOR_DARKGRAY);
	draw_vline(x + 1, y + 4, y + h - 5, COLOR_LIGHTGRAY);
	draw_vline(x + 2, y + 4, y + h - 5, COLOR_DARKGRAY);

	draw_vline(x + w - 1, y + 4, y + h - 5, COLOR_DARKGRAY);
	draw_vline(x + w - 2, y + 4, y + h - 5, COLOR_LIGHTGRAY);
	draw_vline(x + w - 3, y + 4, y + h - 5, COLOR_DARKGRAY);

	draw_hline(x + 4, x + w - 5, y + h - 1, COLOR_DARKGRAY);
	draw_hline(x + 4, x + w - 5, y + h - 2, COLOR_LIGHTGRAY);
	draw_hline(x + 4, x + w - 5, y + h - 3, COLOR_DARKGRAY);

	//if(lable == NULL) {
		draw_hline(x + 4, x + w - 5, y, COLOR_DARKGRAY);
		draw_hline(x + 4, x + w - 5, y + 1, COLOR_LIGHTGRAY);
		draw_hline(x + 4, x + w - 5, y + 2, COLOR_DARKGRAY);
	/*} else {
		lable_width = string_width(lable);
		lable_position = x + (w >> 1) - (lable_width >> 1);
		draw_string(lable_position, y - 2, lable, A_NORMAL);

		draw_hline(x + 4, lable_position - 4, y, COLOR_DARKGRAY);
		draw_hline(x + 4, lable_position - 4, y + 1, COLOR_LIGHTGRAY);
		draw_hline(x + 4, lable_position - 4, y + 2, COLOR_DARKGRAY);
		EXT_SETPIX(glbs->dark_buffer, lable_position - 3, y + 1);

		lable_position += lable_width;
		draw_hline(lable_position + 3, x + w - 5, y, COLOR_DARKGRAY);
		draw_hline(lable_position + 3, x + w - 5, y + 1, COLOR_LIGHTGRAY);
		draw_hline(lable_position + 3, x + w - 5, y + 2, COLOR_DARKGRAY);
		EXT_SETPIX(glbs->dark_buffer, lable_position + 2, y + 1);
	}*/

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
		button_gfx + s->state * 8, button_gfx + s->state * 8 + 4, 0b00000111, glbs->light_buffer, glbs->dark_buffer);
}

void draw_bar(MENU *m)
{
	MENU_SELECTION *s = &m->data[m->selected];
	short x = s->x_pos - 1;
	short y = s->y_pos - 1;
	short w = string_width(s->text) + 1;
	short i;

	if(s->state != BUTTON_DISABLE) x += 6;

	for(i = 0 ; i < 7 ; i++) {
		FastDrawHLine(glbs->light_buffer, x, x + w, y + i, A_XOR);
		FastDrawHLine(glbs->dark_buffer, x, x + w, y + i, A_XOR);
	}
}

long menu_process(MENU *m, char *font)
{
	MENU_SELECTION *data = m->data;
	short i, j, dis, best_dis;
	unsigned short k;
	char repeat = 0;
	char sel = 1;
	char pressed;
	char done = FALSE;
	long r = 0;

	glbs->timer = 0;

	for(i = 0 ; i < m->number ; i++) {
		if(data[i].state != BUTTON_DISABLE) {
			draw_button(data + i);
			draw_string(data[i].x_pos + 6, data[i].y_pos, data[i].text, font, A_XOR);
		} else
			draw_string(data[i].x_pos, data[i].y_pos, data[i].text, font, A_XOR);
	}

	draw_bar(m);

	flipping_off();

	while(!done) {
		k = _rowread(ARROWS_ROW);
		if(k == 0) {
			glbs->timer = 0;
			repeat = 0;
		}

		draw_bar(m);

		if(glbs->timer == 0) {
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
					glbs->timer = 200;
					repeat = 1;
				} else glbs->timer = 30;
			}

			if(m->selected < 0) m->selected = m->number - 1;
			if(m->selected == m->number) m->selected = 0;

		}

		if((k & SEL_KEY) && sel == 0) {
			if(data[m->selected].state == BUTTON_DISABLE) done = 1;
			else {
				data[m->selected].state = !data[m->selected].state;
				draw_button(data + m->selected);
				sel = 1;
			}
		} else if(k & DMND_KEY) done = 1;

		if(!(k & SEL_KEY) && sel == 1) sel = 0;

		draw_bar(m);
		update_screen();
	}

	for(i = 0 ; i < m->number ; i++) {
		if(data[i].state == BUTTON_ON) r |= (1 << i);
	}

	while(_rowread(0));
	flipping_on();

	return r;
}
