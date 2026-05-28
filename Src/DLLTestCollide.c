// C Source File
// Created 11/19/2002; 12:57:48 PM

#define __IN_DLL__
#include <tigcclib.h>         // Include All Header Files

char rect_overlap(short x0, short y0, short xdim0, short ydim0, short x1, short y1, short xdim1, short ydim1)
{
	return (-xdim1 < x1 - x0 && x1 - x0 < xdim0 && -ydim1 < y1 - y0 && y1 - y0 < ydim0);
}
/*
char rect_diag_overlap(short x0, short y0, short xdim0, short ydim0, short x1, short y1, short d1, short h1, char direction)
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
*/
#define EXT_XCHG(a,b)    asm volatile ("exg %0,%1" : "=d" (a), "=d" (b) : "0" (a), "1" (b) : "cc")

char TestCollideX8(short x0, short y0, short w0, short h0, short x1, short y1, short w1, short h1,
	unsigned char* light0, unsigned char* light1)
{
	short dx, dy, x, y, width, byte_width, byte_dx, hieght;
	unsigned char *data0_start, *data1_start;
	unsigned char *dark0, *dark1;

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
		EXT_XCHG(light0, light1);
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
		light0 += dy * w0;
		hieght = y0 + h0 - y1;
	} else {
		dy = y0 - y1;
		light1 += dy * w1;
		hieght = y1 + h1 - y0;
	}

	if(hieght > h0) hieght = h0;
	if(hieght > h1) hieght = h1;

	data0_start = light0;
	data1_start = light1;

	//printf_xy(0, 30, "dx:%d", dx);
	//printf_xy(0, 38, "byte_dx:%d", byte_dx);
	//printf_xy(0, 46, "hieght:%d", hieght);
	//printf_xy(0, 54, "width:%d", width);
	//printf_xy(0, 62, "byte_width:%d", byte_width);

	for(y = 0 ; y < hieght ; y++) {
		light0 = data0_start + byte_dx;
		light1 = data1_start;
		dark0 = light0 + w0 * h0;
		dark1 = light1 + w1 * h1;
		data0_start += w0;
		data1_start += w1;
		for(x = 0 ; x < byte_width ; x++) {
			if((unsigned short)(*light1++ | *dark1++) &
				((unsigned short)(*light0++ | *dark0++) << dx)) return 1;
		}
	}

	return 0;
}

/*#define EXT_XCHG(a,b)    asm volatile ("exg %0,%1" : "=d" (a), "=d" (b) : "0" (a), "1" (b) : "cc")

char TestCollideX8(short x0, short y0, short w0, short h0, short x1, short y1, short w1, short h1,
	unsigned char* data0, char wmask0, unsigned char* data1, char wmask1)
{
	short dx, dy, x, y, width, byte_width, byte_dx, hieght;
	unsigned char *data0_start, *data1_start;
	unsigned char *data0n, *data1n;

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
		EXT_XCHG(wmask0, wmask1);
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

	//printf_xy(0, 30, "dx:%d", dx);
	//printf_xy(0, 38, "byte_dx:%d", byte_dx);
	//printf_xy(0, 46, "hieght:%d", hieght);
	//printf_xy(0, 54, "width:%d", width);
	//printf_xy(0, 62, "byte_width:%d", byte_width);

	if(wmask0) {
		for (y = 0 ; y < hieght ; y++) {
			data0 = data0_start + byte_dx;
			data1 = data1_start;
			data0n = data0 + w0 * h0;
			data0_start += w0;
			data1_start += w1;
			for(x = 0 ; x < byte_width ; x++) {
				if(~((unsigned short)*data1 | 0xff00) & (((unsigned short)(*data0 | *data0n)) << dx))
					return 1;
				data0++;
				data0n++;
				data1++;
			}
		}
	}else if(wmask1) {
		for (y = 0 ; y < hieght ; y++) {
			data0 = data0_start + byte_dx;
			data1 = data1_start;
			data1n = data1 + w1 * h1;
			data0_start += w0;
			data1_start += w1;
			for(x = 0 ; x < byte_width ; x++) {
			if(((unsigned short)(*data1 | *data1n)) & (~((unsigned short)*data0 | 0xff00) << dx))
					return 1;
				data0++;
				data1++;
				data1n++;
			}
		}
	} else {
		for (y = 0 ; y < hieght ; y++) {
			data0 = data0_start + byte_dx;
			data1 = data1_start;
			data0_start += w0;
			data1_start += w1;
			for(x = 0 ; x < byte_width ; x++) {
				if(~((unsigned short)*data1 | 0xff00) & (~((unsigned short)*data0 | 0xff00) << dx)) return 1;
				data0++;
				data1++;
			}
		}
	}

	return 0;
}*/