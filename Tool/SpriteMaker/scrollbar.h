#define BAR_HORIZ 0
#define BAR_VERT 1

typedef struct {
	int x;
	int y;
	int width;
	int hieght;
	int color_1;
	int color_2;
	int *data;
	int max;
	char box_1;
	char box_2;
	int slider_pos;
	int slider_size;
} SCROLL_BAR;

#define BAR(x) (bars + (x))

class scrollbar {
	SCROLL_BAR *bars;
	int bar_number;

public:

	scrollbar();
	~scrollbar();
	void add_bar(int x, int y, int width, int hieght,
		int color_1, int color_2, int *data, int max);
	void change_bar_max(int bar, int max);
	void process_bars(int x_off, int y_off);
	void display_bars(BITMAP *buffer);
};
