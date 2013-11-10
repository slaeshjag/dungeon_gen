#ifndef __ITEM_H__
#define __ITEM_H__

#include "aicomm.h"

struct item_type {
	char				*name;
	char				*description;
	/* TODO: Insert function pointer for handler */
	int				(*handler)(int dataval, struct character_entry *src, struct character_entry *dst);
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


struct item *item_init(const char *item_table);
struct item *item_destroy(struct item *item);
struct inventory *inventory_new(int size);
struct inventory *inventory_destroy(struct inventory *inv);


#endif
