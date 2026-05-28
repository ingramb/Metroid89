#include <allegro.h>
#include "scrollbar.h"
#include "string.h"

extern BITMAP *buffer;

scrollbar::scrollbar()
{
	bar_number = 0;
	bars = 0;
}

scrollbar::~scrollbar()
{
	delete bars;
}

void scrollbar::add_bar(int x, int y, int width, int hieght, int color_1, int color_2,
						int *data, int max)
{
	SCROLL_BAR *new_bars;
	int i;

	new_bars = new SCROLL_BAR[bar_number + 1];

	for(i = 0 ; i < bar_number ; i++)
		memcpy(new_bars + i, bars + i, sizeof(SCROLL_BAR));

	memset(new_bars + bar_number, 0, sizeof(SCROLL_BAR));

	(new_bars + bar_number)->x = x;
	(new_bars + bar_number)->y = y;
	(new_bars + bar_number)->width = width;
	(new_bars + bar_number)->hieght = hieght;
	(new_bars + bar_number)->color_1 = color_1;
	(new_bars + bar_number)->color_2 = color_2;
	(new_bars + bar_number)->data = data;
	(new_bars + bar_number)->max = max;

	bar_number++;

	delete bars;
	bars = new_bars;
}

void scrollbar::change_bar_max(int bar, int max)
{
	BAR(bar)->max = max;
}

void scrollbar::process_bars(int x_off, int y_off)
{
	int i;
	int side;
	int x1;
	int y1;
	int a;

	int mousex = mouse_x - x_off;
	int mousey = mouse_y - y_off;

	for(i = 0 ; i < bar_number ; i++){

		BAR(i)->box_1 = BAR(i)->box_2 = false;

		if(BAR(i)->max <= 0) continue;
		
		if(BAR(i)->width > BAR(i)->hieght){
			side = BAR(i)->hieght;
			x1 = BAR(i)->x + BAR(i)->width - BAR(i)->hieght;
			y1 = BAR(i)->y;
			BAR(i)->slider_size = (BAR(i)->width - BAR(i)->hieght * 2) / (BAR(i)->max + 1);
			if(BAR(i)->slider_size < 3) BAR(i)->slider_size = 6;
			BAR(i)->slider_pos = BAR(i)->x + BAR(i)->hieght +
				*BAR(i)->data * (BAR(i)->width - BAR(i)->hieght * 2 - BAR(i)->slider_size) / BAR(i)->max;
		}else{
			side = BAR(i)->width;
			x1 = BAR(i)->x;
			y1 = BAR(i)->y + BAR(i)->hieght - BAR(i)->width;
			BAR(i)->slider_size = (BAR(i)->hieght - BAR(i)->width * 2) / (BAR(i)->max + 1);
			if(BAR(i)->slider_size < 3) BAR(i)->slider_size = 6;
			BAR(i)->slider_pos = BAR(i)->y + BAR(i)->width +
				*BAR(i)->data * (BAR(i)->hieght - BAR(i)->width * 2 - BAR(i)->slider_size) / BAR(i)->max;
		}

		if(mousex > BAR(i)->x && mousey > BAR(i)->y &&
			mousex < BAR(i)->x + side - 1 && mousey < BAR(i)->y + side - 1){
				BAR(i)->box_1 = true;
				if( (mouse_b & 1) && *BAR(i)->data) (*BAR(i)->data)--;
		}
		if(mousex > x1 && mousey > y1 &&
			mousex < x1 + side - 1 && mousey < y1 + side - 1){
				BAR(i)->box_2 = true;
				if( (mouse_b & 1) && *BAR(i)->data != BAR(i)->max) (*BAR(i)->data)++;
		}

		if(mousex > BAR(i)->x && mousex < BAR(i)->x + BAR(i)->width - 1 &&
			mousey > BAR(i)->y && mousey < BAR(i)->y + BAR(i)->hieght - 1 &&
			!BAR(i)->box_1 && !BAR(i)->box_2 && (mouse_b & 1)){
				if(BAR(i)->width > BAR(i)->hieght)
					a = mousex;
				else
					a = mousey;
				if(a < BAR(i)->slider_pos)
					*BAR(i)->data -= BAR(i)->max / 10;
				if(a > BAR(i)->slider_pos + BAR(i)->slider_size - 1)
					*BAR(i)->data += BAR(i)->max / 10;
				if(*BAR(i)->data < 0) *BAR(i)->data = 0;
				if(*BAR(i)->data > BAR(i)->max) *BAR(i)->data = BAR(i)->max;
		}
				
	}
}

void scrollbar::display_bars(BITMAP *buffer)
{
	int i;
	int x1;
	int y1;
	int x2;
	int y2;

	for(i = 0 ; i < bar_number ; i++){

		if(BAR(i)->max <= 0) continue;

		rect(buffer, BAR(i)->x, BAR(i)->y,
			BAR(i)->x + BAR(i)->width - 1, BAR(i)->y + BAR(i)->hieght - 1, BAR(i)->color_1);
		rectfill(buffer, BAR(i)->x + 1, BAR(i)->y + 1,
			BAR(i)->x + BAR(i)->width - 2, BAR(i)->y + BAR(i)->hieght - 2, BAR(i)->color_2);
		
		if(BAR(i)->hieght > BAR(i)->width){
			hline(buffer, BAR(i)->x + 1, BAR(i)->y + BAR(i)->width - 1,
				BAR(i)->x + BAR(i)->width - 2, BAR(i)->color_1);
			hline(buffer, BAR(i)->x + 1, BAR(i)->y + BAR(i)->hieght - BAR(i)->width,
				BAR(i)->x + BAR(i)->width - 2, BAR(i)->color_1);
			if(BAR(i)->box_1)
				rectfill(buffer, BAR(i)->x + 1, BAR(i)->y + 1,
					BAR(i)->x + BAR(i)->width - 2, BAR(i)->y + BAR(i)->width - 2, BAR(i)->color_1);
			if(BAR(i)->box_2)
				rectfill(buffer, BAR(i)->x + 1, BAR(i)->y + BAR(i)->hieght - BAR(i)->width + 1,
					BAR(i)->x + BAR(i)->width - 2, BAR(i)->y + BAR(i)->hieght - 2, BAR(i)->color_1);

			x1 = BAR(i)->x + 1;
			y1 = BAR(i)->slider_pos;
			x2 = x1 + BAR(i)->width - 3;
			y2 = y1 + BAR(i)->slider_size - 1;
		}
		if(BAR(i)->hieght < BAR(i)->width){
			vline(buffer, BAR(i)->x + BAR(i)->hieght - 1, BAR(i)->y + 1,
				BAR(i)->y + BAR(i)->hieght - 2, BAR(i)->color_1);
			vline(buffer, BAR(i)->x + BAR(i)->width - BAR(i)->hieght, BAR(i)->y + 1,
				BAR(i)->y + BAR(i)->hieght - 2, BAR(i)->color_1);
			if(BAR(i)->box_1)
				rectfill(buffer, BAR(i)->x + 1, BAR(i)->y + 1,
					BAR(i)->x + BAR(i)->hieght - 2, BAR(i)->y + BAR(i)->hieght - 2, BAR(i)->color_1);
			if(BAR(i)->box_2)
				rectfill(buffer, BAR(i)->x + BAR(i)->width - BAR(i)->hieght + 1, BAR(i)->y + 1,
					BAR(i)->x + BAR(i)->width - 2, BAR(i)->y + BAR(i)->hieght - 2, BAR(i)->color_1);

			x1 = BAR(i)->slider_pos;
			y1 = BAR(i)->y + 1;
			x2 = x1 + BAR(i)->slider_size - 1;
			y2 = y1 + BAR(i)->hieght - 3;
		}

		rect(buffer, x1, y1, x2, y2, 0);
		rectfill(buffer, x1 + 1, y1 + 1, x2 - 1, y2 - 1, BAR(i)->color_1);
	}
}
