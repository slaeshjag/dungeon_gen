#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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


static int index_conv(int i, int w, int h, int dx, int dy) {
	if ((i + dx) / w != i / w || (i % w) + dx < 0 || (i % w) + dx >= w)
		return i;
	if ((i / w + dy) < 0 || (i / w + dy) >= h)
		return i;
	return (i + dx + dy * w);
}


static int dir_conv(int i, int dir, int w, int h) {
	int x, y;
	x = y = 0;

	x = ((dir & 2) ? 1 : -1) * !(dir & 1);
	y = ((dir & 2) ? 1 : -1) * (dir & 1);;

	return index_conv(i, w, h, x, y);
}


static int generate_layout(int *n, int i, int *boss, unsigned int *data, int w, int h, int mi, int mx) {
	int branch, order[4], l, dir;

	if (data[i])
		return 0;
	
	data[i] = 1;
	branch = (rand() & 0xF) + 1;
	(*n)++;

	if (branch < 12 && mx > *n)
		branch = 1;
	else if (branch < 13 && mx > *n)
		branch = 2;
	else if (*n > mi) {
		if (*boss)
			return 1;
		else
			return ((*boss = data[i] = 3) > 0);
	} else
		branch = 1;
	
	util_order_randomize(order, 4);

	for (l = 0; l < 4 && branch; l++) {
		dir = dir_conv(i, order[l], w, h);
		branch -= generate_layout(n, dir, boss, data, w, h, mi, mx);
	}
	
	return 1;
}


static void generate_dungeon_layout_adjust(unsigned int *data, int w, int h) {
	int i, j, xmin, ymin = h;

	for (i = w - 1; i >= 0; i--)
		for (j = h - 1; j >= 0; j--)
			if (data[j * w + i]) {
				xmin = i;
				if (ymin > j)
					ymin = j;
			}

	for (i = ymin; i < h; i++) {
		memmove(&data[(i - ymin) * w], &data[i * w + xmin], (w - xmin) * sizeof(int));
		memset(&data[i * w + w - xmin], 0, xmin * sizeof(int));
		if (ymin)
			memset(&data[i * w], 0, w * sizeof(int));
	}
	
	return;
}


void dungeon_layout_new(unsigned int *data, int w, int h, int max_room, int min_room, int boss_s) {
	int n, boss;

	do {
		n = 0;
		boss = !boss_s;
		memset(data, 0, w * h * 4);
		generate_layout(&n, 72, &boss, data, w, h, min_room, max_room);
	} while (n < min_room || !boss);
	
	data[72] = 2;

	generate_dungeon_layout_adjust(data, w, h);

	return;
}
