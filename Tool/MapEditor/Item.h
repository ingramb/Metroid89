#ifndef __ITEM__
#define __ITEM__

typedef struct {
	short x;
	short y;
	unsigned short index;
	unsigned char type;
	unsigned char unused;
} ITEM;

class item {
public:
	ITEM *data;
	short number;
	item();
	~item();
	bool new_item(short x, short y, unsigned char type);
	bool delete_item(short x, short y);
	unsigned char get_item(short x, short y);
	void draw(BITMAP *buffer, short camera_x, short camera_y, tileset *pics);
	void save_data(FILE *outfile);
	void load_data(FILE *infile);
};

#endif