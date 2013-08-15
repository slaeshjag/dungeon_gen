#include "util.h"
#include "random.h"
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


void util_blt_trans(unsigned int *dst, int dw, int dh, int dx, int dy, unsigned int *src, int sw, int sh, unsigned int trans) {
	int i, j, dsti;
	
	dsti = dx + dy * dw;

	for (i = 0; i < sh && i + dy < dh; i++)
		for (j = 0; j < sw; j++)
			if (src[i * sw + j] != trans)
				dst[dsti + i * dw + j] = src[i * sw + j];
	return;
}


void util_order_randomize(int *order, int count) {
	int sw, i, t;
	
	for (i = 0; i < count; i++)
		order[i] = i;

	for (i = 0; i < count - 1; i++) {
		sw = random_get() % (count - i) + i;
		t = order[i];
		order[i] = order[sw];
		order[sw] = t;
	}

	return;
}


int util_local_to_global_coord(int w, int room_w, int room_h, int room, int tile) {
	int blah, foo, fred;

	blah = room % w;
	fred = room / w;
	foo = blah * room_w + (tile % room_w);
	foo += (fred * w * room_w * room_h + (tile / room_w * w) * room_w);
	return foo;
}
