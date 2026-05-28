// C Source File
// Created 11/20/01; 11:48:30 AM

#define OPTIMIZE_ROM_CALLS // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "menubox.h"
#include "menu.h"
#include "extgraph.h"
#include "utility.h"

volatile int menu_delay = 0;

unsigned char up_arrow[] = {
	0x30,0x58,0xac,0xfc,
	0x30,0x68,0xc4,0xfc,
	0xcf,0x87,0x03,0x03
};

unsigned char down_arrow[] = {
	0xfc,0xac,0x58,0x30,
	0xfc,0xc4,0x68,0x30,
	0x03,0x03,0x87,0xcf
};

MENU *create_menu(BOX *b, char length, char rows, char colums)
{
	MENU *menu;
	
	menu = malloc(sizeof(MENU));
	menu->box = b;
	menu->length = length;
	menu->rows = rows;
	menu->colums = colums;
	menu->selection = 0;
	menu->old_selection = -1;
	menu->top = 0;
	menu->pos = 0;
	
	menu->data = malloc(13 * length);
	
	return menu;
}

char add_text(MENU *menu, char *text)
{
	if(menu->pos > menu->length) return FALSE;
	strcpy(menu->data + 13 * menu->pos, text);
	menu->pos++;
	return TRUE;
}

void update_menu(MENU *menu)
{
	register char i, j;
	char *text;
	char selection_x = 0;
	char selection_y = 0;
	
	if(menu->selection == menu->old_selection) return;
	
	clear_box(menu->box);
	
	for(i = 0 ; i < menu->rows ; i++)
		for(j = 0 ; j < menu->colums ; j++)
			if((i + menu->top) * menu->colums + j < menu->length)
				if(((i + menu->top) * menu->colums + j) != menu->selection){
					text = menu->data + ((i + menu->top) * menu->colums + j) * 13;
					box_text(menu->box, j*75, i*12, text, A_NORMAL);
				} else {
					selection_x = j;
					selection_y = i;
				}
	
	text = menu->data + menu->selection * 13;
					
	draw_bar(menu->box, selection_x * 75 - 1, selection_y * 12 - 1,
		selection_x * 75 + text_length(text) + 2, selection_y * 12 + 12 + text_hieght(text));
	
	box_text(menu->box, selection_x*75, selection_y*12, text, A_XOR);
	
	menu->old_selection = menu->selection;
}

void display_menu(MENU *menu)
{
	int arrow_x;
	int arrow_y;
	int total_menu_rows;
	int current_menu_row;
	
	total_menu_rows = (menu->length - menu->rows) / menu->colums;
	if(total_menu_rows * menu->colums != menu->length - menu->rows) total_menu_rows++;
	
	current_menu_row = menu->top;
	
	arrow_x = menu->box->x + menu->box->width - 5;
	arrow_y = menu->box->y + 3 + menu->box->y_offset +
		(menu->box->hieght-14-menu->box->y_offset) / total_menu_rows * current_menu_row;
	
	update_menu(menu);
	draw_box(menu->box);
	if(menu->top > 0)
		GraySprite8_MASK(arrow_x + virtual_x_off, arrow_y + virtual_y_off, 4, up_arrow, up_arrow + 4,
			up_arrow + 8, up_arrow + 8, light_buffer, dark_buffer);
	if((menu->top + menu->rows) * menu->colums < menu->length)
		GraySprite8_MASK(arrow_x + virtual_x_off, arrow_y + virtual_y_off + 6, 4, down_arrow, down_arrow + 4,
			down_arrow + 8, down_arrow + 8, light_buffer, dark_buffer);
}
			
void draw_bar(BOX *box, int x, int y, int x2, int y2)
{
	SCR_RECT srect = {{x + 5, y + 5 + box->y_offset, x2 +3, y2 + 3 + box->y_offset}};
	
	y += box->y_offset;
	y2 += box->y_offset;
	
	PortSet(box->light_data, box->byte_width * 8 - 1, box->hieght);
  ScrRectFill(&srect, &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
  DrawClipRect(MakeWinRect(x + 4, y + 4, x2 + 4, y2+ 4), &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
  PortSet(box->dark_data, box->byte_width * 8 - 1, box->hieght);
  ScrRectFill(&srect, &(SCR_RECT){{0, 0, 159, 99}}, A_REVERSE);
  DrawClipRect(MakeWinRect(x + 4, y + 4, x2 + 4, y2+ 4), &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
}

char process_menu(MENU *menu)
{
	short k = _rowread(ARROWS_ROW);
	
	if(k == -256) menu_delay = 0;
	if(menu_delay) return TRUE;	
	
	if(k&UP_KEY){
		menu->selection -= menu->colums;
		menu_delay = 50;
	}
	if(k&DOWN_KEY){
		menu->selection += menu->colums;
		menu_delay = 50;
	}
	if(k&LEFT_KEY){
		menu->selection--;
		menu_delay = 70;
	}
	if(k&RIGHT_KEY){
		menu->selection++;
		menu_delay = 70;
	}
	
	if(menu->selection >= menu->length){
		menu->selection = 0;
		menu->top = 0;
	}
	if(menu->selection < 0){
		menu->selection = menu->length - 1;
		menu->top = menu->length / menu->colums - menu->rows;
	}
	
	if(menu->selection / menu->colums < menu->top)
		menu->top--;
	if(menu->selection /menu->colums - menu->top > menu->rows - 1)
		menu->top++;
	
	return TRUE;
}
			
			
			
			
			
