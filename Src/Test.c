// C Source File
// Created 3/30/02; 11:55:32 AM

#define USE_TI89              // Produce .89z File

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files
#include <extgraph.h>

/*char rect_overlap(short x0, short y0, short xdim0, short ydim0, short x1, short y1, short xdim1, short ydim1)
{
	return (-xdim1 < x1 - x0 && x1 - x0 < xdim0 && -ydim1 < y1 - y0 && y1 - y0 < ydim0);
}

char rect_rotated_overlap(short x0, short y0, short xdim0, short ydim0, short x1, short y1, short d1, short h1, char direction)
{
	char x_overlap, y_overlap;
	
	if(!rect_overlap(x0, y0, xdim0, ydim0, x1, y1, d1, d1)) return FALSE;
	
	if(direction == 0) {
		x_overlap = x0 - x1;
		y_overlap = y0 - y1;
		if(x_overlap + y_overlap > d1 - 1 + h1) return FALSE;
		x_overlap = x0 + xdim0 - x1 - 1;
		y_overlap = y0 + ydim0 - y1 - 1;
		if(x_overlap + y_overlap < d1 - 1 - h1) return FALSE;
		return TRUE;
	} else {
		x_overlap = x1 + d1 - x0 - xdim0;
		y_overlap = y0 - y1;
		if(x_overlap + y_overlap > d1 - 1 + h1) return FALSE;
		x_overlap = x1 + d1 - x0 - 1;
		y_overlap = y0 + ydim0 - y1 - 1;
		if(x_overlap + y_overlap < d1 - 1 - h1) return FALSE;
		return TRUE;
	}
}

#define EXT_XCHG(a,b)    asm volatile ("exg %0,%1" : "=d" (a), "=d" (b) : "0" (a), "1" (b) : "cc")

short TestCollideX8(short x0, short y0, short w0, short h0, short x1, short y1, short w1, short h1,
	unsigned char* data0, unsigned char* data1)
{
	short dx, dy, x, y, width, byte_width, byte_dx, hieght;
	unsigned char *data0_start, *data1_start;
	
	if(!rect_overlap(x0, y0, w0 << 3, h0, x1, y1, w1 << 3, h1)) return 0;
	
	// sprite 0 is left of sprite 1 ...
	if (x0 < x1) {
		dx = x1 - x0;
		width = x0 + (w0 << 3) - x1;
	} else {
		// if sprite 0 is right of sprite 1 just exchange
		// all necessary variables so we can still assume
		// sprite 0 is left of sprite 1 ...
		dx = x0 - x1;
		width = x1 + (w1 << 3) - x0;
		EXT_XCHG(data0, data1);
		EXT_XCHG(y0, y1);
		EXT_XCHG(h0, h1);
		EXT_XCHG(w0, w1);
	}
	
	if(width > (w0 << 3)) width = w0 << 3;
	if(width > (w1 << 3)) width = w1 << 3;
	
	byte_width = width >> 3;
	if((byte_width << 3) != width) byte_width++;
	
	byte_dx = dx >> 3;
	dx = dx - (byte_dx << 3);

	if (y0 < y1) {
		dy = y1 - y0;
		data0 += dy * w0;
		hieght = y0 + h0 - y1;
	} else {
		dy = y0 - y1;
		data1 += dy * w1;
		hieght = y1 + h1 - y0;
	}
	
	if(hieght > h0) hieght = h0;
	if(hieght > h1) hieght = h1;
	
	data0_start = data0;
	data1_start = data1;

	for (y = 0 ; y < hieght ; y++) {
		data0 = data0_start + byte_dx;
		data1 = data1_start;
		data0_start += w0;
		data1_start += w1;
		for(x = 0 ; x < byte_width ; x++) {
			if((*data1 >> dx) & *data0) return 1;
			data0++;
			data1++;
		}
	}

	return 0;
}

unsigned short sprite0[] = {0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0,0xfff0};
unsigned short sprite1[] = {
	0b1000000000000001,
	0b0100000000000010,
	0b0010000000000100,
	0b0001000000001000,
	0b0000100000010000,
	0b0000010000100000,
	0b0000001001000000,
	0b0000000110000000,
	0b0000000110000000,
	0b0000001001000000,
	0b0000010000100000,
	0b0000100000010000,
	0b0001000000001000,
	0b0010000000000100,
	0b0100000000000010,
	0b1000000000000001,
};

extern void draw_sprite(long x asm("d0"), long y asm("d1"), unsigned short *s asm("a0"));

volatile unsigned long timer;
DEFINE_INT_HANDLER(time)
{
	timer++;
}*/

#define STUFF (1 << 18)

void _main(void)
{
	/*short x = 0, y = 0, k;
	
	while(1) {
		ClrScr();
		Sprite16_OR(x, y, 16, sprite0, LCD_MEM);
		Sprite16_OR(30, 30, 16, sprite1, LCD_MEM);
		printf_xy(0, 0, "%d", rect_rotated_overlap(x, y, 12, 16, 30, 30, 16, 1, 1));
		k = ngetchx();
		if(k == KEY_LEFT) x--;
		if(k == KEY_RIGHT) x++;
		if(k == KEY_UP) y--;
		if(k == KEY_DOWN) y++;
	}*/

	unsigned long a = (1 << 18);
}
