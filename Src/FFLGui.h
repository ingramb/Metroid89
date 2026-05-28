// C Header File
// Created 3/27/02; 10:02:08 PM

typedef struct {
	short	x;
	short	y;
	short	width;
	short	hieght;
	short	byte_width;
	short	text_hieght;
	void	*light;
	void	*dark;
	void	*light_save;
	void	*dark_save;
} BOX;

typedef struct {
	BOX		*box;
	char	*data;
	short	i;
	char	repeat;
	short	light_save[15];
	short	dark_save[15];
	short	number;
	short	select;
	short	top;
} MENU;

#define MENU_NOTHING 0
#define MENU_SELECTED 1
#define MENU_BACK 2

BOX *box_create(short x, short y, short width, short hieght, void *light, void *dark);
inline BOX *box_create_text(short x, short y, short width, short hieght, void *light, void *dark);
void box_kill(BOX *box);

inline void draw_char(short x, short y, char c, void *light, void *dark);
void draw_str(short x, short y, char *s, void *light, void *dark);
void draw_box_num(BOX *box, short x, short y, short number);
inline void draw_box_str(BOX *box, short x, short y, char *s);

MENU *menu_create(BOX *box, short number, short select);
void menu_add_choice(MENU *menu, char *txt);
void menu_draw(MENU *menu);
short menu_process(MENU *menu);
void menu_kill(MENU *menu);
