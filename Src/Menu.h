// C Header File
// Created 11/20/01; 12:18:56 PM

typedef struct {
	BOX *box;
	char length;
	char rows;
	char colums;
	char selection;
	char old_selection;
	char top;
	char *data;
	
	char pos;
} MENU;

extern volatile int menu_delay;

MENU *create_menu(BOX *b, char length, char rows, char colums);
char add_text(MENU *menu, char *text);
void update_menu(MENU *menu);
void display_menu(MENU *menu);
void draw_bar(BOX *box, int x, int y, int x2, int y2);
char process_menu(MENU *menu);
