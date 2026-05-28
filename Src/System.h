// C Header File
// Created 10/5/2002; 2:22:00 PM

#ifndef __SYSTEM__
#define __SYSTEM__

#include "dllexports.h"

typedef struct {
	short x_pos;
	short y_pos;
	char text[20];
	char state;
} MENU_SELECTION;

typedef struct {
	short number;
	MENU_SELECTION *data;
	short selected;
	short i;
} MENU;

#define BUTTON_OFF 0
#define BUTTON_ON 1
#define BUTTON_DISABLE 2

short string_width(char *);
void draw_string(short, short, const char *, unsigned char *, short);
void draw_string_ex(short, short, const char *, unsigned char *, short, void *, void *);
void message(char *, unsigned char *, short);
void draw_border(short, short, short, short, char);

MENU * menu_create(short);
void menu_add_choice(MENU *, short, short, char, char *);
long menu_process(MENU *, unsigned char *);

char title_screen(char *, void *, void *);




#endif
