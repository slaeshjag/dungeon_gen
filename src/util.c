#include "util.h"
#include <stdlib.h>
#include <string.h>

static int util_index_conv(int i, int w, int h, int dx, int dy) {
	if ((i + dx) / w != i / w || (i % w) + dx < 0 || (i % w) + dx >= w)
		return i;
	if ((i / w + dy) < 0 || (i / w + dy) >= h)
		return i;
	return (i + dx + dy * w);
}


int util_dir_conv(int i, int dir, int w, int h) {
	int x, y;
	x = y = 0;

	x = ((dir & 2) ? 1 : -1) * !(dir & 1);
	y = ((dir & 2) ? 1 : -1) * (dir & 1);;

	return util_index_conv(i, w, h, x, y);
}



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

