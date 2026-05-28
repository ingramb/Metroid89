// C Source File
// Created 10/21/2002; 7:24:18 PM

//#define OPTIMIZE_ROM_CALLS    // Use ROM Call Optimization

#include <tigcclib.h>         // Include All Header Files
#include "block.h"
#include "clipsprites.h"

#define BLOCK_MAX 15

BLOCK *blocks = NULL;
short first_block;
short first_empty_block;

char block_setup()
{
	blocks = malloc(sizeof(BLOCK) * BLOCK_MAX);
	if(blocks == NULL) return FALSE;
	
	block_reset();
	return TRUE;
}

void block_reset()
{
	short i;
	
	memset(blocks, 0, sizeof(BLOCK) * BLOCK_MAX);
	for(i = 0 ; i < BLOCK_MAX - 1 ; i++) blocks[i].next = i + 1;
	blocks[BLOCK_MAX - 1].next = -1;
	first_block = -1;
	first_empty_block = 0;
}

void block_cleanup()
{
	if(blocks != NULL) {
		free(blocks);
		blocks = NULL;
	}
}

short block_create(short x, short y, short width, short hieght)
{
	short i = first_empty_block;
	
	if(i < 0) return -1;
	
	first_empty_block  = blocks[i].next;
	
	blocks[i].next = first_block;
	first_block = i;
	
	blocks[i].x = x;
	blocks[i].y = y;
	blocks[i].width = width;
	blocks[i].hieght = hieght;
	
	return i;
}

void block_delete(short a)
{
	short i;
	
	if(a < 0) return;
	
	if(a == first_block)
		first_block = blocks[a].next;
	else
		for(i = first_block ; i >= 0 ; i = blocks[i].next)
			if(blocks[i].next == a) {
				blocks[i].next = blocks[a].next;
				break;
			}
	
	blocks[a].next = first_empty_block;
	first_empty_block = a;
}

char block_test(short x, short y, short width, short hieght)
{
	short i;
	
	for(i = first_block ; i >= 0 ; i = blocks[i].next) {
		if(rect_overlap(x, y, width, hieght, blocks[i].x, blocks[i].y, blocks[i].width, blocks[i].hieght))
			return TRUE;
	}
	
	return FALSE;
}