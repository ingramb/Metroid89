// C Header File
// Created 2/22/02; 4:05:21 PM

typedef struct {
	int x;
	int y;
	int x_space;
	int y_space;
	int max;
	
	int select;
	int hand_x;
	int hand_y;
	int hand_x_counter;
	int hand_y_counter;
	int hand_direction;
	int hand_distance;
} MENU;

void menu_init(MENU *menu, int x, int y, int x_space, int y_space, int max);
void menu_process(MENU *menu);
void menu_draw(MENU *menu);
		