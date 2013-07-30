#include "dungeon_generate_structure.h"
#include "dungeon_generate.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>


static int dungeon_generate_path(unsigned int *buff, int w, int h, int i, int tile, int *cnt, int max) {
	int order[4], ret, l, t;

	if (i < 0 || i > w * h)
		return -1;
	if (buff[i])
		return -1;
	if (*cnt >= max)
		return -1;
	
	buff[i] = tile;
	(*cnt)++;
	util_order_randomize(order, 4);
	ret = -1;

	for (l = 0; l < 4; l++)
		ret = ((t = dungeon_generate_path(buff, w, h, util_dir_conv(i, order[l], w, h), tile, cnt, max)) == -1) ? ret : t;
	return (ret == -1) ? i : ret;
}


void dungeon_generate_bslide_puzzle(unsigned int *buff, int *w, int *h) {
	int i, x, y, order[4], l, max, t, use;

	*w = 3 + (random_get() % 6);
	*h = 3 + (random_get() % 6);
	
	x = random_get() % (*w);
	y = random_get() % (*h);
	buff[x + y * (*w)] = (ROOM_TILE_PUZZLE_BUTTON | ROOM_TILE_PUZZLE_PROVIDE);

	i = 0;

	util_order_randomize(order, 4);
	max = (random_get() % 14) + 4;
	for (l = 0; l < 4; l++)
		use = ((t = dungeon_generate_path(buff, *w, *h, util_dir_conv(x + y * (*w), order[l], (*w), (*h)), ROOM_TILE_FLOOR_KEEP, &i, max)) == -1) ? use : t;
	buff[use] = (ROOM_TILE_PUZZLE_SLIDEBLOCK | ROOM_TILE_PUZZLE_COULD_DEPEND);

	return;
}
