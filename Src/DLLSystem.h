// C Header File
// Created 11/14/2002; 5:06:27 PM

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

short string_width(const char *txt);
void draw_string(short x, short y, const char *txt, char *font, short mode);
void draw_string_ex(short x, short y, const char *txt, char *font, short mode, void *light, void *dark);
void draw_number(short x, short y, unsigned short number, char *font, char digits, void *light, void *dark);
void message(char *txt, char *font, short hieght);
void draw_border(short x, short y, short w, short h, char erase);

MENU *menu_create(short number);
void menu_add_choice(MENU *m, short x, short y, char state, char *text);
long menu_process(MENU *m, char *font);