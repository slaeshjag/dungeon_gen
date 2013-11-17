#ifndef __ITEM_H__
#define __ITEM_H__

#include "aicomm.h"
#include "item_reply.h"

struct item_type {
	char				*name;
	char				*description;
	/* TODO: Insert function pointer for handler */
	struct item_reply		(*handler)(int dataval, struct character_entry *src, struct character_entry *dst);
	int				datavalue;
	int				max_stack;
};


struct item {
	struct item_type		*type;
	int				types;
};


struct inventory_entry {
	/* -1 == unused */
	int				type;
	int				amount;
};


struct inventory {
	struct inventory_entry		*entry;
	int				entries;
};


void item_init(const char *item_table);
void item_destroy();
struct inventory *inventory_new(int size);
struct inventory *inventory_destroy(struct inventory *inv);
void item_use(struct inventory *inv, int item, int char_src, int char_dst);


#endif
