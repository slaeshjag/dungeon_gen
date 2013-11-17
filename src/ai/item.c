#include "item_reply.h"
#include "aicomm.h"
#include <string.h>

struct item_reply morot(int dataval, struct character_entry *src, struct character_entry *dst) {
	struct item_reply ir;

	ir.src = malloc(sizeof(*ir.src) * 2);
	ir.dst = malloc(sizeof(*ir.dst) * 2);

	ir.srcs = 2;
	ir.dsts = 2;
	ir.srcp = ITEM_SPREAD_TYPE_NONE;
	ir.dstp = ITEM_SPREAD_TYPE_NONE;
	ir.srcn = 1;
	ir.dstn = 1;

	strcpy(ir.src[0].stat, "hp");
	ir.src[0].diff = dataval * 10;
	ir.src[0].turns = -1;
	strcpy(ir.dst[0].stat, "hp");
	ir.dst[0].diff = dataval * -50;
	ir.dst[0].turns = -1;
	strcpy(ir.src[1].stat, "def");
	ir.src[1].diff = dataval * 15;
	ir.src[1].turns = -1;
	strcpy(ir.dst[1].stat, "def");
	ir.dst[1].diff = dataval * -75;
	ir.dst[1].turns = -1;

	return ir;
}
