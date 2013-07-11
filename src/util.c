#include "util.h"
#include <stdlib.h>
#include <string.h>


void util_blt(unsigned int *dest, int dw, int dh, int dx, int dy, unsigned int *src, int sw, int sh, int sx, int sy) {
	int i;

	for (i = 0; i < sh && dy + i < dh; i++)
		memcpy(&dest[dx + (dy + i) * dw], &src[sx + (sy + i) * sw], sw * sizeof(unsigned int));
	return;
}


void util_order_randomize(int *order, int count) {
	int sw, i, t;
	
	for (i = 0; i < count; i++)
		order[i] = i;

	for (i = 0; i < count - 1; i++) {
		sw = rand() % (count - i) + i;
		t = order[i];
		order[i] = order[sw];
		order[sw] = t;
	}

	return;
}


