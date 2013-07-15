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


struct dungeon *dungeon_layout_new(int w, int h, int max_room, int min_room, int boss_s, int floors) {
	int n, boss, middle, i, f;
	unsigned int *tmp_data;
	struct dungeon *dungeon;

	if (!(dungeon = malloc(sizeof(*dungeon))))
		return NULL;
	tmp_data = malloc(sizeof(unsigned int) * w * h);
	middle = w / 2 + h / 2 * w;
	dungeon->data = malloc(sizeof(int *) * floors);
	dungeon->room_map = malloc(sizeof(int **) * floors);
	dungeon->w = malloc(sizeof(int) * floors);
	dungeon->h = malloc(sizeof(int) * floors);
	dungeon->floors = floors;

	for (f = 0; f < floors; f++) {
		do {
			n = 0;
			boss = !boss_s;
			memset(tmp_data, 0, w * h * 4);
			generate_layout(&n, middle, &boss, tmp_data, w, h, min_room, max_room);
		} while (n < min_room || !boss);
	
		generate_dungeon_layout_adjust(tmp_data, w, h);
		dungeon_map_size(tmp_data, w, h, &dungeon->w[f], &dungeon->h[f]);
	
		dungeon->data[f] = malloc(sizeof(unsigned int) * dungeon->w[f] * dungeon->h[f]);
		dungeon->room_map[f] = malloc(sizeof(void *) * dungeon->w[f] * dungeon->h[f]);
		memset(dungeon->room_map[f], 0, dungeon->w[f] * dungeon->h[f] * sizeof(void *));
		
		for (i = 0; i < dungeon->h[f]; i++)
			memcpy(&dungeon->data[f][i * dungeon->w[f]], &tmp_data[i * w], dungeon->w[f] * 4);
	}

	dungeon->layout_scratchpad = tmp_data;

	return dungeon;
}


static unsigned int *dungeon_generate_room_template(const int w, const int h, unsigned int type) {
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
			data[i + j * w] = ROOM_TILE_FLOOR;
	
	return data;
}


static void spawn_doors(struct dungeon *dungeon, int i, int floor) {
	int l, door, t, x, y;

	for (l = 00; l < 04; l++) {
		if ((t = util_dir_conv(i, l, dungeon->w[floor], dungeon->h[floor])) == i)
			continue;
		if (!(dungeon->data[floor][t]))
			continue;
		switch (dungeon->data[floor][t] & 0377) {
			case MAP_ROOM_TYPE_ROOM:
			case MAP_ROOM_TYPE_ENTRANCE:
				door = dungeon->data[floor][t] = ROOM_TILE_DOOR;
				break;
			case MAP_ROOM_TYPE_BOSS_ROOM:
				door = ROOM_TILE_DOOR_BOSS;
				break;
			default:
				door = 00;
				fprintf(stderr, "Rurgh...\n");
				break;
		}

		if (l & 01) {
			y = (l & 02) ? dungeon->room_h - 01 : 00;
			x = dungeon->room_w >> 01;
		} else {
			y = dungeon->room_h >> 01;
			x = (l & 02) ? dungeon->room_w - 01 : 00;
		}

		dungeon->room_map[floor][i][x + y * dungeon->room_w] = door;
	}

	return;
}


static int spawn_tile(struct dungeon *dungeon, int floor, int room, int tile) {
	int i, j, spawn;

	for (i = j = 00; i < dungeon->room_w * dungeon->room_h; i++)
		if ((dungeon->room_map[floor][room][i] & 0377) == ROOM_TILE_FLOOR)
			dungeon->room_scratchpad[j++] = i;
	if (!j)
		return -1;
	
	spawn = dungeon->room_scratchpad[rand() % j];
	dungeon->room_map[floor][room][spawn] = tile;

	return spawn;
}


static void spawn_puzzle(struct dungeon *dungeon, int room, int floor) {
	int complexity;

	complexity = rand() % 2;
	
	switch (complexity) {
		default:
			break;
	}

	return;
}


void dungeon_init_floor(struct dungeon *dungeon, int room_w, int room_h, int max_enemy, int entrance_floor) {
	int i, j, f, g, last_room, last_tile, rooms, spawn;
	
	dungeon->room_w = room_w;
	dungeon->room_h = room_h;
	dungeon->room_scratchpad = malloc(sizeof(unsigned int) * room_w * room_h);
	dungeon->entrance_floor = entrance_floor;
	last_room = last_tile = -1;

	for (f = 0; f < dungeon->floors; f++) {
		for (i = g = 0; i < dungeon->w[f] * dungeon->h[f]; i++) {
			if (!(dungeon->data[f][i] & 0xFF))
				continue;
			dungeon->room_map[f][i] = dungeon_generate_room_template(dungeon->room_w, dungeon->room_h, dungeon->data[f][i]);
			if ((dungeon->data[f][i] & 0xFF) == MAP_ROOM_TYPE_ROOM)
				dungeon->layout_scratchpad[g++] = i;
		}

		rooms = g;
		
		for (i = 0; i < rooms; i++) {
			spawn_doors(dungeon, dungeon->layout_scratchpad[i], f);
	
			for (j = 0; j < rand() % max_enemy; j++)
				spawn_tile(dungeon, f, dungeon->layout_scratchpad[i], ROOM_TILE_ENEMY0 + rand() % 8);
		}

		if (f == entrance_floor)
			dungeon->entrance = dungeon->layout_scratchpad[rand() % rooms];
		
		if (f > 0) {
			spawn = dungeon->layout_scratchpad[rand() % rooms];
			dungeon->room_map[f-1][last_room][last_tile] |= (spawn << 16);
			g = spawn;
			spawn_tile(dungeon, f, spawn, ROOM_TILE_WAY_DOWN | (last_room << 16));
		}
		
		if (f + 1 < dungeon->floors) {
			spawn = dungeon->layout_scratchpad[rand() % rooms];
			last_room = spawn;
			last_tile = spawn_tile(dungeon, f, spawn, ROOM_TILE_WAY_UP);
		}
	}


	return;
}
