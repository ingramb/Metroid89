// C Source File
// Created 2/27/02; 7:18:15 PM

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>
#include "extgraph2.h"
#include "cards.h"
#include "utility.h"
#include "select.h"
#include "textbox.h"
#include "request.h"
#include "font.h"

void request_init(REQUEST *r, BOX *b, int x, int y, char *text, int number)
{
	int i, a = 0;
	int width = 0;
	int hieght = 0;
	
	memset(r, 0, sizeof(REQUEST));
	r->box = b;
	r->x = x;
	r->y = y;
	r->text = text;
	r->number = number;
	
	for(i = 0 ; i < number ; i++){
		a = text_length(text + 15 * i);
		if(a > width) width = a;
		a = text_hieght(text + 15 * i);
		if(a > hieght) hieght = a;
	}
	
	r->width = width;
	r->hieght = hieght;
	
	request_draw(r);
}

void request_process(REQUEST *r)
{
	int k = _rowread(ARROWS_ROW);
	
	if(k == -256) r->delay = 0;
	
	if(r->delay) {
		r->delay--;
		return;
	}
	
	if(k & LEFT_KEY) {
		r->select--;
		r->delay = 5;
		if(r->select < 0) r->select = r->number - 1;
		request_draw(r);
	}
	
	if(k & RIGHT_KEY) {
		r->select++;
		r->delay = 5;
		if(r->select == r->number) r->select = 0;
		request_draw(r);
	}
}

void request_draw(REQUEST *r)
{
	SCR_RECT srect = {{r->x + 6, r->y + 6, r->x + 7 + r->width, r->y + 7 + r->hieght}};
	
	PortSet(r->box->light_data, r->box->byte_width * 8 - 1, r->box->hieght);
  ScrRectFill(&srect, &(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
  DrawClipRect(MakeWinRect(r->x + 5, r->y + 5, r->x + 5 + r->width + 3, r->y + 8 + r->hieght),
  	&(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
 	
 	PortSet(r->box->dark_data, r->box->byte_width * 8 - 1, r->box->hieght);
  ScrRectFill(&srect, &(SCR_RECT){{0, 0, 159, 99}}, A_REVERSE);
  //DrawClipRect(MakeWinRect(r->x + 5, r->y + 5, r->x + 5 + r->width + 3, r->y + 8 + r->hieght),
  //	&(SCR_RECT){{0, 0, 159, 99}}, A_NORMAL);
  
  //FastDrawLine2(r->box->light_data, r->box->width, r->x + 6, r->y + 5,
  //	r->x + 7 + r->width, r->y + 5, A_NORMAL);
  //FastDrawLine2(r->box->light_data, r->box->width, r->x + 6, r->y + 8 + r->hieght,
  //	r->x + 7 + r->width, r->y + 8 + r->hieght, A_NORMAL);
  //FastDrawLine2(r->box->light_data, r->box->width, r->x + 5, r->y + 6,
  //	r->x + 5, r->y + 7 + r->hieght, A_NORMAL);
  //FastDrawLine2(r->box->light_data, r->box->width, r->x + 8 + r->width, r->y + 6,
  //	r->x + 6 + r->width, r->y + 7 + r->hieght, A_NORMAL);
	
	box_text(r->box, r->x + 2, r->y + 2, (r->text + r->select * 15), A_REVERSE);
}
	
  