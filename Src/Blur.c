// C Source File
// Created 7/26/02; 12:15:15 AM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
//#include "extgraph.h"
#include "utility.h"
#include "entity.h"
#include "map.h"
#include "clipsprites.h"
#include "blur.h"
#include "globals.h"

//Must be power of 2
#define BLUR_MAX 16

BLUR *blur = NULL;

short blur_front;
short blur_back;

char blur_setup()
{
	blur = malloc(sizeof(BLUR) * BLUR_MAX);
	if(blur == NULL) return FALSE;

	blur_reset();
	return TRUE;
}

void blur_reset()
{
	blur_front = blur_back = 0;
}

void blur_cleanup()
{
	if(blur != NULL) {
		free(blur);
		blur = NULL;
	}
}

void blur_create(short x, short y, SPRITE_HEADER *header, unsigned char *gfx)
{
	if(((blur_back + 1) & (BLUR_MAX - 1)) == blur_front) return;

	blur_back = (blur_back + 1) & (BLUR_MAX - 1);

	blur[blur_back].x = x;
	blur[blur_back].y = y;
	blur[blur_back].header = header;
	blur[blur_back].gfx = gfx;
	blur[blur_back].counter = 20;
	blur[blur_back].flash = 0;
}

void blur_process()
{
	short i;

	for(i = (blur_front + 1) & (BLUR_MAX - 1) ;
		i != ((blur_back + 1) & (BLUR_MAX - 1)) ;
		i = (i + 1) & (BLUR_MAX - 1)) blur[i].counter--;
	while(blur[(blur_front + 1) & (BLUR_MAX - 1)].counter == 0 && blur_front != blur_back)
		blur_front = (blur_front + 1) & (BLUR_MAX - 1);
}

void blur_draw()
{
	SPRITE_HEADER *header;
	short i;
	short x;
	short y;
	short frame_size;

	for(i = (blur_front + 1) & (BLUR_MAX - 1) ;
		i != ((blur_back + 1) & (BLUR_MAX - 1)) ;
		i = (i + 1) & (BLUR_MAX - 1)) {
			header = blur[i].header;
			x = blur[i].x + header->x_offset - glbs->camera.x;
			y = blur[i].y + header->y_offset - glbs->camera.y;
			frame_size = header->hieght * header->width;

			GraySpriteClipX8_MASK(x, y, header->hieght,
				blur[i].gfx + header->offset, header->width, blur[i].flash);
			blur[i].flash = !blur[i].flash;
	}
}

