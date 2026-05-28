// C Source File
// Created 2/23/02; 12:43:25 PM

#define OPTIMIZE_ROM_CALLS // Use ROM Call Optimization

#define FULLSCREEN  &(SCR_RECT){{0, 0, 239, 127}}

#include <tigcclib.h>         // Include All Header Files
#include "textbox.h"
#include "extgraph.h"
#include "utility.h"
#include "extgraph2.h"
#include "font.h"

unsigned char box_gfx[192] = {
	//sprite 0
	0x07,0x18,0x20,0x40,0x40,0x80,0x80,0x80,
	0x07,0x18,0x27,0x5f,0x5f,0xbf,0xbf,0xbf,
	0xf8,0xe0,0xc0,0x80,0x80,0x00,0x00,0x00,
	//sprite 1
	0xe0,0x18,0x04,0x02,0x02,0x01,0x01,0x01,
	0xe0,0x18,0xe4,0xfa,0xfa,0xfd,0xfd,0xfd,
	0x1f,0x07,0x03,0x01,0x01,0x00,0x00,0x00,
	//sprite 2
	0x80,0x80,0x80,0x40,0x40,0x20,0x18,0x07,
	0xbf,0xbf,0xbf,0x5f,0x5f,0x27,0x18,0x07,
	0x00,0x00,0x00,0x80,0x80,0xc0,0xe0,0xf8,
	//sprite 3
	0x01,0x01,0x01,0x02,0x02,0x04,0x18,0xe0,
	0xfd,0xfd,0xfd,0xfa,0xfa,0xe4,0x18,0xe0,
	0x00,0x00,0x00,0x01,0x01,0x03,0x07,0x1f,
	//sprite 4
	0x84,0x88,0x90,0xa0,0xc0,0x80,0x00,0x00,
	0xcc,0xd8,0xf0,0xe0,0xc0,0x80,0x00,0x00,
	0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff,0xff,
	//sprite 5
	0x00,0x00,0x80,0xc0,0xa0,0x90,0x88,0x84,
	0x00,0x00,0x80,0xc0,0xe0,0xf0,0xd8,0xcc,
	0xff,0xff,0x7f,0x3f,0x1f,0x0f,0x07,0x03,
	//sprite 6
	0xfc,0x08,0x10,0x20,0x40,0x80,0x00,0x00,
	0xfc,0xf8,0x30,0x60,0xc0,0x80,0x00,0x00,
	0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff,0xff,
	//sprite 7
	0x3f,0x10,0x08,0x04,0x02,0x01,0x00,0x00,
	0x3f,0x1f,0x0c,0x06,0x03,0x01,0x00,0x00,
	0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff,0xff,
};

BOX *create_box(int x, int y, int width, int hieght)
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
	
	box->light_data = malloc(box->size * 3);
	box->dark_data = box->light_data + box->size;
	box->mask_data = box->dark_data + box->size;
	
	memset(box->light_data, 0x0000, box->size * 3);
	
	PortSet(box->dark_data, box->byte_width * 8 - 1, hieght);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
	
	Sprite8_OR2(0, 0, 8, box_gfx, box->light_data, box->byte_width);
	Sprite8_BLIT2(0, 0, 8, box_gfx + 8, 0, box->dark_data, box->byte_width);
	Sprite8_OR2(0, 0, 8, box_gfx + 16, box->mask_data, box->byte_width);
	
	Sprite8_OR2(width - 8, 0, 8, box_gfx + 24, box->light_data, box->byte_width);
	Sprite8_BLIT2(width - 8, 0, 8, box_gfx + 24 + 8, 0, box->dark_data, box->byte_width);
	Sprite8_OR2(width - 8, 0, 8, box_gfx + 24 + 16, box->mask_data, box->byte_width);
	
	Sprite8_OR2(0, hieght - 8, 8, box_gfx + 48, box->light_data, box->byte_width);
	Sprite8_BLIT2(0, hieght - 8, 8, box_gfx + 48 + 8, 0, box->dark_data, box->byte_width);
	Sprite8_OR2(0, hieght - 8, 8, box_gfx + 48 + 16, box->mask_data, box->byte_width);
	
	Sprite8_OR2(width - 8, hieght - 8, 8, box_gfx + 72, box->light_data, box->byte_width);
	Sprite8_BLIT2(width - 8, hieght - 8, 8, box_gfx + 72 + 8, 0, box->dark_data, box->byte_width);
	Sprite8_OR2(width - 8, hieght - 8, 8, box_gfx + 72 + 16, box->mask_data, box->byte_width);
	
	FastDrawLine2(box->light_data, box->byte_width, 8, 0, width - 8, 0, A_NORMAL);
	FastDrawLine2(box->dark_data, box->byte_width, 8, 1, width - 8, 1, A_REVERSE);
	
	FastDrawLine2(box->light_data, box->byte_width, 8, hieght - 1, width - 8, hieght - 1, A_NORMAL);
	FastDrawLine2(box->dark_data, box->byte_width, 8, hieght - 2, width - 8, hieght - 2, A_REVERSE);
	
	FastDrawLine2(box->light_data, box->byte_width, 0, 8, 0, hieght - 8, A_NORMAL);
	FastDrawLine2(box->dark_data, box->byte_width, 1, 8, 1, hieght - 8, A_REVERSE);
	
	FastDrawLine2(box->light_data, box->byte_width, width - 1, 8, width - 1, hieght - 8, A_NORMAL);
	FastDrawLine2(box->dark_data, box->byte_width, width - 2, 8, width - 2, hieght - 8, A_REVERSE);

	return box;
}

void draw_box(BOX *box)
{
	GraySpriteClipX8_MASK(box->x, box->y, box->hieght,
		box->light_data, box->dark_data, box->mask_data, box->byte_width, light_buffer, dark_buffer);
}

void kill_box(BOX *box)
{
	free(box->light_data);
	free(box);
}

void clear_box(BOX *box)
{
	SCR_RECT rect = {{3, 3, box->width - 5, box->hieght - 5}};
	
	PortSet(box->dark_data, box->byte_width * 8 - 1, box->hieght);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
	PortSet(box->light_data, box->byte_width * 8 - 1, box->hieght);
	ScrRectFill(&rect, &(SCR_RECT){{0, 0, 159, 99}}, A_REVERSE);
}

void box_text(BOX *box, short x, short y, char *text, short mode)
{
	register unsigned int i;
	
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

void draw_bar(BOX *box, int x, int y, int x2, int y2)
{
	SCR_RECT srect = {{x + 5, y + 5, x2 +3, y2 + 3}};
	
	PortSet(box->light_data, box->byte_width * 8 - 1, box->hieght);
  ScrRectFill(&srect, &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
  DrawClipRect(MakeWinRect(x + 4, y + 4, x2 + 4, y2+ 4), &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
  PortSet(box->dark_data, box->byte_width * 8 - 1, box->hieght);
  ScrRectFill(&srect, &(SCR_RECT){{0, 0, 159, 99}}, A_REVERSE);
  DrawClipRect(MakeWinRect(x + 4, y + 4, x2 + 4, y2+ 4), &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
}
