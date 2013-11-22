#include "item_reply.h"
#include "aicomm.h"
#include <string.h>


int item_get_acc_effects(const char *stat, struct character_entry *chr) {
	int i, acc;

	for (i = acc = 0; i < chr->item_replies; i++)
		if (!strcmp(chr->item_reply[i].stat, stat))
			acc += chr->item_reply[i].diff;

	return acc;
}


int item_find_stat(const char *stat, struct character_entry *chr) {
	int i;

	for (i = 0; i < (signed) chr->stats; i++)
		if (!strcmp(chr->stat[i].name, stat))
			return i;
	return -1;
}


struct item_reply morot(int dataval, struct character_entry *src, struct character_entry *dst) {
	struct item_reply ir;
	int diff;

	ir.src = malloc(sizeof(*ir.src) * 2);
	ir.dst = malloc(sizeof(*ir.dst) * 2);

	ir.srcs = 1;
	ir.dsts = 1;
	ir.srcp = ITEM_SPREAD_TYPE_NONE;
	ir.dstp = ITEM_SPREAD_TYPE_NONE;
	ir.srcn = 1;
	ir.dstn = 1;

	diff = item_find_stat("hp", src);
	diff = (src->stat[diff].max) - item_get_acc_effects("hp", src);
	diff /= 10;
	strcpy(ir.src[0].stat, "hp");
	ir.src[0].diff = dataval * diff;
	ir.src[0].turns = -1;
	strcpy(ir.dst[0].stat, "hp");
	ir.dst[0].diff = dataval * -50;
	ir.dst[0].turns = -1;

	return ir;
}
