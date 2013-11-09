#include "savefile.h"
#include "item.h"
#include <stdlib.h>
#include <darnit/darnit.h>


struct item *item_init(const char *item_table) {
	struct item *item;
	DARNIT_FILE *f;
	char buff[512], namebuff[64], descbuff[384], handler[32];
	int dataval, stack;

	if (!(f = d_file_open(item_table, "rb")))
		return NULL;
	item = malloc(sizeof(*item));

	/* TODO: Read item table */
	while (!d_file_eof(f)) {
		d_file_gets(buff, 512, f);
		*namebuff = *descbuff = *handler = 0;
		sscanf(buff, "%[^\t] %[^\t] %s %i %i", namebuff, descbuff, handler, &dataval, &stack);
		if (!*handler)
			continue;
		fprintf(stderr, "%s: <%s> by %s;; %i %i\n", namebuff, descbuff, handler, dataval, stack);
	}
	
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
