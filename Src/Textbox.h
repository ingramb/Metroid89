// C Header File
// Created 2/23/02; 12:43:55 PM

typedef struct {
	short width;
	short hieght;
	short x;
	short y;
	char byte_width;
	short size;
	void *light_data;
	void *dark_data;
	void *mask_data;
} BOX;

BOX *create_box(int x, int y, int width, int hieght);
void draw_box(BOX *box);
void kill_box(BOX *box);
void clear_box(BOX *box);
void box_text(BOX *box, short x, short y, char *text, short mode);
void draw_bar(BOX *box, int x, int y, int x2, int y2);
