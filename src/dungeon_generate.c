#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dungeon_generate.h"
#include "util.h"

static int generate_layout(int *n, int i, int *boss, unsigned int *data, int w, int h, int mi, int mx) {
	int branch, order[4], l, dir;

	if (data[i])
		return 0;
	
	data[i] = MAP_ROOM_TYPE_ROOM;
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
			return ((*boss = data[i] = MAP_ROOM_TYPE_BOSS_ROOM) > 0);
	} else
		branch = 1;
	
	util_order_randomize(order, 4);

	for (l = 0; l < 4 && branch; l++) {
		dir = util_dir_conv(i, order[l], w, h);
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


static void dungeon_map_size(unsigned int *data, int w, int h, int *r_w, int *r_h) {
	int i, j, max_x, max_y;

	max_x = max_y = 0;
	for (i = 0; i < w; i++)
		for (j = 0; j < h; j++)
			if (data[i + j * w] & 0xFF) {
				if (max_x < i)
					max_x = i;
				if (max_y < j)
					max_y = j;
			}
	
	*r_w = max_x + 1;
	*r_h = max_y + 1;

	return;
}


struct dungeon *dungeon_layout_new(int w, int h, int max_room, int min_room, int boss_s) {
	int n, boss, middle, i;
	unsigned int *tmp_data;
	struct dungeon *dungeon;

	if (!(dungeon = malloc(sizeof(*dungeon))))
		return NULL;
	tmp_data = malloc(sizeof(unsigned int) * w * h);
	middle = w / 2 + h / 2 * w;

	do {
		n = 0;
		boss = !boss_s;
		memset(tmp_data, 0, w * h * 4);
		generate_layout(&n, middle, &boss, tmp_data, w, h, min_room, max_room);
	} while (n < min_room || !boss);
	
	tmp_data[middle] = MAP_ROOM_TYPE_ENTRANCE;
	generate_dungeon_layout_adjust(tmp_data, w, h);
	dungeon_map_size(tmp_data, w, h, &dungeon->w, &dungeon->h);

	dungeon->data = malloc(sizeof(unsigned int) * dungeon->w * dungeon->h);
	dungeon->room_map = malloc(sizeof(void *) * dungeon->w * dungeon->h);
	memset(dungeon->room_map, 0, dungeon->w * dungeon->h * sizeof(void *));

	for (i = 0; i < dungeon->h; i++)
		memcpy(&dungeon->data[i * dungeon->w], &tmp_data[i * w], dungeon->w * 4);
	free(tmp_data);

	return dungeon;
}


static void layout_spawn_keylock(unsigned int *data, int w, int h, int i, int *keys, int kcnt, int *step, int *key) {
	int order[4], dir, l;

	if (i < 0 || i >= w * h)
		return;
	if (!(data[i] & 0xFF))
		return;
	if (data[i] & MAP_ROOM_TMP_VISIT)
		return;
	if ((data[i] & 0xFF) == MAP_ROOM_TYPE_BOSS_ROOM)
		return;
	data[i] |= MAP_ROOM_TMP_VISIT;

	(*step)++;

	if ((*step / kcnt != (*step - 1) / kcnt || *step == 1) && *key) {
		(*keys)++;
		(*key)--;
		data[i] |= MAP_ROOM_HAS_KEY;
	}

	if ((data[i] & 0xFF) == MAP_ROOM_TYPE_ROOM) {
		if (*keys == 2) {
			data[i] |= MAP_ROOM_HAS_LOCK;
			(*keys)--;
		} else if (*keys)
			if (!(rand() % kcnt)) {
				data[i] |= MAP_ROOM_HAS_LOCK;
				(*keys)--;
			}
	} 
	
	util_order_randomize(order, 4);

	for (l = 0; l < 4; l++) {
		dir = util_dir_conv(i, order[l], w, h);
		layout_spawn_keylock(data, w, h, dir, keys, kcnt, step, key);
	}

	return;
}


void dungeon_layout_spawn_keylocks(struct dungeon *dungeon, int keylocks, int boss) {
	int start, keys, step, rooms, boss_loc, i, key;

	if (!keylocks)
		return;
	for (i = rooms = 0; i < dungeon->w * dungeon->h; i++)
		if (dungeon->data[i] & 0xFF)
			rooms++;

	for (i = 0; i < dungeon->w * dungeon->h; i++)
		if ((dungeon->data[i] & 0xFF) == MAP_ROOM_TYPE_ENTRANCE) {
			start = i;
			break;
		}
	
	step = keys = 0;
	key = keylocks;
	layout_spawn_keylock(dungeon->data, dungeon->w, dungeon->h, start, &keys, rooms / keylocks, &step, &key);

	for (i = 0; i < dungeon->w * dungeon->h; i++)
		dungeon->data[i] = (dungeon->data[i] & (~0 ^ MAP_ROOM_TMP_VISIT));
	
	if (boss) {
		boss_loc = (rand() % (rooms - 1));
		for (i = 0; i < dungeon->w * dungeon->h; i++) {
			if (!(dungeon->data[i] & 0xFF))
				continue;
			if ((dungeon->data[i] & 0xFF) == MAP_ROOM_TYPE_BOSS_ROOM)
				continue;
			if (!boss_loc) {
				dungeon->data[i] |= MAP_ROOM_HAS_BOSSKEY;
				break;
			}

			boss_loc--;
		}
	}
	
	return;
}


static unsigned int *dungeon_generate_room_template(int w, int h, unsigned int type) {
	unsigned int *data;
	int i, j;

	if (!(data = malloc(sizeof(unsigned int) * w * h)))
		return data;
	
	for (i = 0; i < w; i++)
		data[i] = data[h * w - w + i] = ROOM_TILE_WALL;
	for (i = 0; i < h; i++)
		data[i * w] = data[i * w + w - 1] = ROOM_TILE_WALL;
	for (i = 1; i < w - 1; i++)
		for (j = 1; j < h - 1; j++)
			data[i + j * w] = ROOM_TILE_WALL;
	
	return data;
}


void dungeon_init_floor(struct dungeon *dungeon, int room_w, int room_h) {
	int i;
	
	dungeon->room_w = room_w;
	dungeon->room_h = room_h;

	for (i = 0; i < dungeon->w * dungeon->h; i++) {
		if (!(dungeon->data[i] & 0xFF))
			continue;
		dungeon->room_map[i] = dungeon_generate_room_template(dungeon->room_w, dungeon->room_h, dungeon->data[i]);
	}

	return;
}
