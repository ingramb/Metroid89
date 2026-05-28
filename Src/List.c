// C Source File
// Created 9/11/2002; 7:23:24 PM

struct list_block {
	unsigned char number;
	struct list_block *next;
	char data[0];
};
typedef struct list_block LIST_BLOCK;

struct list_node {
	LIST_BLOCK *block;
	struct list_node *prev;
	struct list_node *next;
	char data[0];
};
typedef struct list_node LIST_NODE;

typedef struct {
	LIST_NODE *header;
	LIST_NODE *tail;
	LIST_NODE *pos;
	LIST_BLOCK *header_block;
	LIST_BLOCK *current_block;
	size_t node_size;
	unsigned char block_size;
} LIST;

char list_add_block(LIST *list)
{
	LIST_BLOCK *new_block;
	size_t size = list->size * list->block_size + sizeof(LIST_BLOCK);
	
	new_block = malloc(size);
	if(new_block == NULL) return FALSE;
	new_block->number = 0;
	new_block->next = NULL;
	
	if(list->header_block == NULL)
		list->header_block = new_block;
	else
		list->current_block->next = new_block;
		
	list->current_block = new_block;
	
	return TRUE;
}

char list_add(LIST *list, void *data)
{
	LIST_NODE *node;
	
	if(list->current_block->number == list->block_size) {
		if(!list_add_block(list)) return FALSE;
	}
	
	node = (LIST_NODE *)(list->current_block->data + list->size * list->current_block->number);
	
	node->next = NULL;
	node->block = list->current_block;
	
	if(list->header == NULL) {
		list->header = node;
		list->tail = node;
		node->prev = NULL;
	} else {
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
	}
	
	memcpy(node->data, data, list->size - sizeof(LIST_NODE));
	
	return TRUE;
}

char list_remove(LIST *list, LIST_NODE *node)
{
	node->block->number--;
	node->prev->next = node->next;
	node->next->prev = node->prev;
	
	if(list->tail == node) list->tail = node->prev;
	if(list->header = node) list->header = node->next;
}

LIST *list_create(size_t size, unsigned char block_size)
{	
	LIST *list = malloc(sizeof(LIST));
	
	size += sizeof(LIST_NODE);
	
	list->size = size;
	list->block_size = block_size;
	
	list->header_block = NULL;
	list->current_block = NULL;
	list->header = NULL;
	list->tail = NULL;
	list->pos = NULL;
	
	if(!list_add_block(list)) return NULL;
	
	return list;
}

