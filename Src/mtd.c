// C Source File
// Created 1/8/02; 8:39:41 PM

#define USE_TI89              // Produce .89z File

#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#define SAVE_SCREEN           // Save/Restore LCD Contents

#include <tigcclib.h>         // Include All Header Files

#define NONE -1

enum {
	EMPTY,
	BACKGROUND,
	WAYPOINT,
	SOLID,
	BREAK_STAND,
	BREAK_SHOOT,
	SLOPE_RIGHT_UP,
	SLOPE_LEFT_UP,
	SLOPE_RIGHT_DOWN,
	SLOPE_LEFT_DOWN
};

typedef struct {
	unsigned char hit;
	unsigned char animate;
	unsigned char delay;
} TILE_DATA;

TILE_DATA data[] = {
	EMPTY, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SLOPE_RIGHT_UP, 0, 0,
	SLOPE_LEFT_UP, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SLOPE_RIGHT_DOWN, 0, 0,
	SLOPE_LEFT_DOWN, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SLOPE_RIGHT_UP, 0, 0,
	SLOPE_LEFT_UP, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	EMPTY, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	SLOPE_RIGHT_DOWN, 0, 0,
	SLOPE_LEFT_DOWN, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	SOLID, 0, 0,
	SOLID, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
	BACKGROUND, 0, 0,
};

extern unsigned char tiles[];

void _main(void)
{
	int number = 119;
	FILE *outfile;
	
	outfile = fopen("tiles", "wb");
	fwrite(&number, 1, 2, outfile);
	fwrite(data, number + 1, sizeof(TILE_DATA), outfile);
	fwrite(tiles, number, 72, outfile);
	
	fputc (0, outfile);
	fputs ("MTD", outfile);
	fputc (0, outfile);
	fputc (OTH_TAG, outfile);
	
	fclose(outfile);
}
