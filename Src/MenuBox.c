// C Source File
// Created 11/18/01; 1:02:51 PM

#define OPTIMIZE_ROM_CALLS // Use ROM Call Optimization

#define FULLSCREEN  &(SCR_RECT){{0, 0, 239, 127}}

#include <tigcclib.h>         // Include All Header Files
#include "menubox.h"
#include "extgraph.h"
#include "utility.h"
#include "extgraph2.h"
#include "font.h"

BOX *create_box(int x, int y, int width, int hieght, char *title)
{
	BOX *box;
	register char i;
	void *buffer;
	short mode;
	SCR_RECT rect = {{0, 0, width - 1, hieght - 1}};
	
	box = malloc(sizeof(BOX));
	box->x = x;
	box->y = y;
	box->width = width;
	box->hieght = hieght;
	box->byte_width = width / 8;
	if(box->byte_width * 8 != width) box->byte_width++;
	if((box->byte_width / 2) * 2 != box->byte_width) box->byte_width++;
	box->size = box->byte_width * hieght;
	
	box->light_data = malloc(box->size * 2);
	box->dark_data = box->light_data + box->size;
	
	memset(box->light_data, 0x0000, box->size * 2);
	PortSet(box->dark_data, box->byte_width * 8 - 1, hieght);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
	
	FastDrawLine2(box->light_data, box->byte_width,
		1, 0, box->width - 2, 0, A_NORMAL);
	FastDrawLine2(box->light_data, box->byte_width,
		1, box->hieght - 1, box->width - 2, box->hieght - 1, A_NORMAL);
	FastDrawLine2(box->light_data, box->byte_width,
		0, 1, 0, box->hieght - 2, A_NORMAL);
	FastDrawLine2(box->light_data, box->byte_width,
		box->width - 1, 1, box->width - 1, box->hieght - 2, A_NORMAL);
		
	buffer = box->light_data;
	mode = A_NORMAL;
	
	for(i = 0 ; i < 2 ; i++){
		FastDrawLine2(buffer, box->byte_width,
			1, 1, box->width - 2, 1, mode);
		FastDrawLine2(buffer, box->byte_width,
			1, box->hieght - 2, box->width - 2, box->hieght - 2, mode);
		FastDrawLine2(buffer, box->byte_width,
			1, 2, 1, box->hieght - 3, mode);
		FastDrawLine2(buffer, box->byte_width,
			box->width - 2, 2, box->width - 2, box->hieght - 3, mode);
		
		buffer = box->dark_data;
		mode = A_REVERSE;
	}
	
	EXT_CLRPIX2(box->dark_data, box->byte_width, 0, 0);
	EXT_CLRPIX2(box->dark_data, box->byte_width, box->width - 1, 0);
	EXT_CLRPIX2(box->dark_data, box->byte_width, 0, box->hieght - 1);
	EXT_CLRPIX2(box->dark_data, box->byte_width, box->width - 1, box->hieght - 1);
	
	box->y_offset = 0;
	if(title != NULL){
		FastDrawLine2(box->light_data, box->byte_width, x + 2, y + 11, x + width - 3, y + 11, A_NORMAL);
		FastDrawLine2(box->dark_data, box->byte_width, x + 2, y + 11, x + width - 3, y + 11, A_REVERSE);
		
		FastDrawLine2(box->light_data, box->byte_width, x + 2, y + 10, x + width - 3, y + 10, A_NORMAL);
		FastDrawLine2(box->light_data, box->byte_width, x + width - 3, y + 2, x + width - 3, y + 9, A_NORMAL);
		
		draw_box_title(box, title);
		box->y_offset = 10;
	}
	
	FastDrawLine2(box->light_data, box->byte_width,
		2, box->hieght - 3, box->width - 3, box->hieght - 3, A_NORMAL);
	FastDrawLine2(box->light_data, box->byte_width,
		box->width - 3, 2 + box->y_offset, box->width - 3, box->hieght - 4, A_NORMAL);
	
	return box;
}

void draw_box(BOX *box)
{
	SCR_RECT rect = {{box->x + virtual_x_off, box->y + virtual_y_off,
		box->x + box->width +virtual_x_off - 1, box->y + box->hieght +virtual_y_off - 1}};
	
	PortSet(light_buffer, 239, 99);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_REVERSE);
	PortSet(dark_buffer, 239, 99);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_REVERSE);
	
	SpriteClipX8_OR(box->x, box->y, box->hieght,
		box->light_data, box->byte_width, light_buffer);
	SpriteClipX8_OR(box->x, box->y, box->hieght,
		box->dark_data, box->byte_width, dark_buffer);
}

void kill_box(BOX *box)
{
	free(box->light_data);
	free(box);
}

void clear_box(BOX *box)
{
	SCR_RECT rect = {{2, 2 + box->y_offset, box->width - 4, box->hieght - 4}};
	
	PortSet(box->dark_data, box->byte_width * 8 - 1, box->hieght);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
	PortSet(box->light_data, box->byte_width * 8 - 1, box->hieght);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_REVERSE);
}

void box_text(BOX *box, short x, short y, char *text, short mode)
{
	register unsigned int i;
	
	y += box->y_offset;
	
	if(mode == A_NORMAL)
		for(i = 0 ; i < strlen(text) ; i++){
			draw_char(box->light_data, box->byte_width, x + 5, y + 4, *(text + i), A_NORMAL);
			draw_char(box->light_data, box->byte_width, x + 6, y + 5, *(text + i), A_REVERSE);
			draw_char(box->dark_data, box->byte_width, x + 6, y + 5, *(text + i), A_REVERSE);
			x += (font_width[*(text + i) - 28] + 1);
		}
	else
		for(i = 0 ; i < strlen(text) ; i++){
			draw_char(box->light_data, box->byte_width, x + 5, y + 4, *(text + i), A_REVERSE);
			draw_char(box->light_data, box->byte_width, x + 6, y + 5, *(text + i), A_NORMAL);
			draw_char(box->dark_data, box->byte_width, x + 6, y + 5, *(text + i), A_NORMAL);
			x += (font_width[*(text + i) - 28] + 1);
		}
}

void draw_box_title(BOX *box, char *text)
{
	register unsigned int i;
	short x = 3;
	short y = 3;
	
	for(i = 0 ; i < strlen(text) ; i++){
		draw_small_char(box->light_data, box->byte_width, x, y, *(text + i), A_NORMAL);
		draw_small_char(box->light_data, box->byte_width, x + 1, y + 1, *(text + i), A_REVERSE);
		draw_small_char(box->dark_data, box->byte_width, x + 1, y + 1 , *(text + i), A_REVERSE);
		x += (small_font_width[*(text + i)] + 1);
	}
}
