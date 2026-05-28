// C Header File
// Created 11/18/01; 12:32:24 PM

typedef struct {
	short width;
	short hieght;
	short x;
	short y;
	char byte_width;
	short size;
	char y_offset;
	void *light_data;
	void *dark_data;
} BOX;

BOX *create_box(int x, int y, int width, int hieght, char *title);
void draw_box(BOX *box);
void kill_box(BOX *box);
void clear_box(BOX *box);
void box_text(BOX *box, short x, short y, char *text, short mode);
void draw_box_title(BOX *box, char *text);
