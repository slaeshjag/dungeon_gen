#include "savefile.h"
#include "item.h"
#include <stdlib.h>
#include <darnit/darnit.h>


struct item *item_init(const char *item_table) {
	struct item *item;
	DARNIT_FILE *f;

	if (!(f = d_file_open(item_table, "rb")))
		return NULL;
	item = malloc(sizeof(*item));

	/* TODO: Read item table */
	item->type = NULL;
	item->types = 0;

	d_file_close(f);

	return item;
}


struct item *item_destroy(struct item *item) {
	int i;

	for (i = 0; i < item->types; i++)
		free(item->type[i].description), free(item->type[i].name);
	free(item->type);
	free(item);

	return NULL;
}


struct inventory *inventory_new(int size) {
	struct inventory *inv;
	int i;

	inv = malloc(sizeof(*inv));
	inv->entry = malloc(sizeof(*inv->entry) * size);
	for (i = 0; i < size; i++)
		inv->entry[i].type = inv->entry[i].amount = -1;
	return inv;
}


struct inventory *inventory_destroy(struct inventory *inv) {
	free(inv->entry);
	free(inv);

	return NULL;
}
