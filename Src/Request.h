// C Header File
// Created 2/27/02; 7:30:18 PM

typedef struct {
	BOX *box;
	char *text;
	int x;
	int y;
	int width;
	int hieght;
	int number;
	int select;
	int delay;
} REQUEST;

void request_init(REQUEST *r, BOX *b, int x, int y, char *text, int number);
void request_process(REQUEST *r);
void request_draw(REQUEST *r);
