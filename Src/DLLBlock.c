// C Source File
// Created 1/20/2003; 9:28:11 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#define __IN_DLL__
#include <tigcclib.h>         // Include All Header Files
#include "dllblock.h"
#include "dlltestcollide.h"
#include "dllentity.h"
#include "globals.h"

/*char block_test_player(short i)
{
	return block_check(i, glbs->player.e.x + glbs->player.e.x_off, glbs->player.e.y + glbs->player.e.y_off,
		glbs->player.e.width, glbs->player.e.hieght);
}*/

char block_setup()
{
	//blocks = malloc(sizeof(BLOCK) * BLOCK_MAX);
	//if(blocks == NULL) return FALSE;

	block_reset();
	return TRUE;
}

void block_reset()
{
	short i;

	memset(glbs->blocks, 0, sizeof(BLOCK) * BLOCK_MAX);
	for(i = 0 ; i < BLOCK_MAX - 1 ; i++) glbs->blocks[i].next = i + 1;
	glbs->blocks[BLOCK_MAX - 1].next = -1;
	glbs->first_block = -1;
	glbs->first_empty_block = 0;
}

void block_cleanup()
{
	//if(blocks != NULL) {
	//	free(blocks);
	//	blocks = NULL;
	//}
}

short block_create(short x, short y, short width, short hieght)
{
	short i = glbs->first_empty_block;

	if(i < 0) return -1;

	glbs->first_empty_block  = glbs->blocks[i].next;

	glbs->blocks[i].next = glbs->first_block;
	glbs->first_block = i;

	glbs->blocks[i].x = x;
	glbs->blocks[i].y = y;
	glbs->blocks[i].width = width;
	glbs->blocks[i].hieght = hieght;

	return i;
}

void block_stretch_vert(short a, short d, short mode)
{
	if(d > 0) glbs->blocks[a].hieght += d * mode;
	else {
		glbs->blocks[a].hieght -= d * mode;
		glbs->blocks[a].y += d * mode;
	}
}

void block_stretch_horiz(short a, short d, short mode)
{
	if(d > 0) glbs->blocks[a].width += d * mode;
	else {
		glbs->blocks[a].width -= d * mode;
		glbs->blocks[a].x += d * mode;
	}
}

void block_move(short a, short direction, short speed)
{
	short px0, px1, py;
	short x_dir = 0, y_dir = 0;
	
	px0 = glbs->player.e.x + glbs->player.e.x_off;
	px1 = px0 + glbs->player.e.width - 1;
	py = glbs->player.e.y + glbs->player.e.y_off + glbs->player.e.hieght;
	
	if(direction & LEFT) x_dir = -1;
	else if(direction & RIGHT) x_dir = 1;
	if(direction & UP) y_dir = -1;
	else if(direction & DOWN) y_dir = 1;
	
	if(py == glbs->blocks[a].y && px1 >= glbs->blocks[a].x &&
	px0 <= glbs->blocks[a].x + glbs->blocks[a].width - 1)
		player_move(direction, speed);
		
	while(speed >= 100) {
		glbs->blocks[a].x += x_dir;
		glbs->blocks[a].y += y_dir;
		speed -= 100;
	}	
}

void block_delete(short a)
{
	short i;

	if(a < 0) return;

	if(a == glbs->first_block)
		glbs->first_block = glbs->blocks[a].next;
	else
		for(i = glbs->first_block ; i >= 0 ; i = glbs->blocks[i].next)
			if(glbs->blocks[i].next == a) {
				glbs->blocks[i].next = glbs->blocks[a].next;
				break;
			}

	glbs->blocks[a].next = glbs->first_empty_block;
	glbs->first_empty_block = a;
}

char block_test(short x, short y, short width, short hieght, short x_dir, short y_dir)
{
	short i;
	short bx, by, bw, bh;

	if(x_dir == 0 && y_dir == 0) {
		for(i = glbs->first_block ; i >= 0 ; i = glbs->blocks[i].next) {
			bx = glbs->blocks[i].x; by = glbs->blocks[i].y;
			bw = glbs->blocks[i].width; bh = glbs->blocks[i].hieght;
			if(rect_overlap(x, y, width, hieght, bx, by, bw, bh)) return TRUE;
		}
	} else {
		for(i = glbs->first_block ; i >= 0 ; i = glbs->blocks[i].next) {
			bx = glbs->blocks[i].x; by = glbs->blocks[i].y;
			bw = glbs->blocks[i].width; bh = glbs->blocks[i].hieght;
			if(rect_overlap(x, y, width, hieght, bx, by, bw, bh) &&
			!rect_overlap(x - x_dir, y - y_dir, width, hieght, bx, by, bw, bh))
				return TRUE;
		}
	}
	return FALSE;
}

char block_check(short i, short x, short y, short width, short hieght)
{
	return
		rect_overlap(x, y, width, hieght, glbs->blocks[i].x, glbs->blocks[i].y,
			glbs->blocks[i].width, glbs->blocks[i].hieght);
}
