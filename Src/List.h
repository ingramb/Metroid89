// C Header File
// Created 9/11/2002; 6:05:40 PM

#ifndef __LIST__
#define __LIST__

struct list_block {
	unsigned char number;
	void *data;
};
typedef struct list_block LIST_BLOCK;

struct list_node {
	void *data;
	LIST_BLOCK *block;
	struct list_node *prev;
	struct list_node *next;
};
typedef struct list_node LIST_NODE;

typedef struct {
	LIST_NODE *header;
	LIST_NODE *tail;
	LIST_NODE *pos;
	LIST_BLOCK *current_block;
	size_t node_size;
	unsigned char block_size;
} LIST;

LIST *list_create(size_t size, unsigned char block_size);
char list_add(LIST *list, void *data);
char list_remove(LIST *list, LIST_NODE *node);


