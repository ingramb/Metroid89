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

#define string_width _DLL_call(short, (char *), STRING_WIDTH)
#define draw_string _DLL_call(void, (short, short, const char *, unsigned char *, short), DRAW_STRING)
#define draw_string_ex _DLL_call(void, (short, short, const char *, unsigned char *, short, void *, void *), DRAW_STRING_EX)
#define message _DLL_call(void, (char *, unsigned char *, short), MESSAGE)
#define draw_border _DLL_call(void, (short, short, short, short, char), DRAW_BORDER)

#define menu_create _DLL_call(MENU *, (short), MENU_CREATE)
#define menu_add_choice _DLL_call(void, (MENU *, short, short, char, char *), MENU_ADD_CHOICE)
#define menu_process _DLL_call(long, (MENU *, unsigned char *), MENU_PROCESS)

#define title_screen _DLL_call(char, (char *, void *, void *), TITLE_SCREEN)




#endif
