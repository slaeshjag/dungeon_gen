#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dungeon_generate.h"
#include "dungeon_generate_structure.h"
#include "util.h"
#include "random.h"

static unsigned int structure[256];
#define	CLEAR_STRUCTURE()		(memset(structure, 0, sizeof(int) * 256));


static void floor_clear_visit(struct dungeon *dungeon, int f) {
	int i;

	for (i = 0; i < dungeon->layout_scratchuse; i++)
		dungeon->data[f][dungeon->layout_scratchpad[i]] = dungeon->data[f][dungeon->layout_scratchpad[i]] & (~MAP_ROOM_TMP_VISIT);
	return;
}


static int spawn_tile(struct dungeon *dungeon, int floor, int room, int tile) {
	int i, j, spawn;

	for (i = j = 00; i < dungeon->room_w * dungeon->room_h; i++)
		if ((dungeon->room_map[floor][room][i] & 0377) == ROOM_TILE_FLOOR)
			dungeon->room_scratchpad[j++] = i;
	if (!j)
		return -1;
	
	spawn = dungeon->room_scratchpad[random_get() % j];
	dungeon->room_map[floor][room][spawn] = tile;

	return spawn;
}


static int spawn_structure(struct dungeon *dungeon, int floor, int room, int struct_w, int struct_h) {
	int i, j, k, l, m, w, spawn;
	w = dungeon->room_w;

	/* får får får? */
	for (i = m = 0; i < dungeon->room_w - struct_w; i++)
		for (j = 0; j < dungeon->room_h - struct_h; j++) {
			for (k = i; k < i + struct_w; k++)
				for (l = j; l < j + struct_h; l++)
					if (dungeon->room_map[floor][room][k + l * w] != ROOM_TILE_FLOOR) {
						goto nospawn;
					}
			
			dungeon->room_scratchpad[m++] = i + j * w;

			nospawn:
				continue;
		}
	
	spawn = dungeon->room_scratchpad[random_get() % m];
	i = spawn % w;
	j = spawn / w;
	util_blt_trans(dungeon->room_map[floor][room], dungeon->room_w, dungeon->room_h, i, j, structure, struct_w, struct_h, 0);

	return spawn;
}
			
	


static int dungeon_room_reachable(struct dungeon *dungeon, int from, int to, int floor) {
	int i, ret, next, op;

	if (from == to)
		return 1;
	if ((dungeon->data[floor][from] & 0377) != MAP_ROOM_TYPE_ROOM)
		return 0;
	if (dungeon->data[floor][from] & MAP_ROOM_TMP_VISIT)
		return 0;
	dungeon->data[floor][from] |= MAP_ROOM_TMP_VISIT;
	for (i = ret = 0; i < 4 && !ret; i++) {
		next = util_dir_conv(from, i, dungeon->w[floor], dungeon->h[floor]);
		if (next == from)
			continue;
		op = ((~(i ^ 1)) & 3);
		if (!((1 << (28 + i)) & dungeon->data[floor][from]) && !((1 << (28 + op)) & dungeon->data[floor][next]))
			ret = dungeon_room_reachable(dungeon, next, to, floor);
	}

	return ret;
}


static int generate_layout(int *n, int i, int *boss, unsigned int *data, int w, int h, int mi, int mx) {
	int branch, order[4], l, dir;

	if (data[i])
		return 0;
	
	data[i] = MAP_ROOM_TYPE_ROOM;
	branch = (random_get() & 0xF) + 1;
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
	int n, boss, middle, i, f, boss_floor;
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
	dungeon->info = malloc(sizeof(*dungeon->info) * floors);
	boss_floor = random_get() % floors;
	dungeon->puzzle = NULL;
	dungeon->puzzles = 0;

	for (f = 0; f < floors; f++) {
		do {
			n = 0;
			boss = (f == boss_floor) ? !boss_s : 1;
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


static unsigned int *dungeon_generate_room_template(const int w, const int h) {
	unsigned int *data;
	int i;

	if (!(data = malloc(sizeof(unsigned int) * w * h)))
		return data;

	for (i = 0; i < w * h; i++) 
		data[i] = ROOM_TILE_FLOOR;

	return data;
}


static int puzzle_struct_add(struct dungeon *dungeon) {
	struct dungeon_puzzle_part *tmp;

	if (!(tmp = realloc(dungeon->puzzle, sizeof(*tmp) * (++dungeon->puzzles))))
		return -1;
	dungeon->puzzle = tmp;
	return dungeon->puzzles - 1;
}


static void spawn_puzzle_part(struct dungeon *dungeon, int room, int floor, int depends) {
	int complexity, spawn, i, w, h;

	CLEAR_STRUCTURE()
	complexity = random_get() % 2;
	
	switch (complexity) {
		case 0:
			dungeon_generate_bslide_puzzle(structure, &w, &h);
			spawn = spawn_structure(dungeon, floor, room, w, h);
			break;
		default:
			return;
			break;
	}

	i = puzzle_struct_add(dungeon);
	dungeon->puzzle[i].room_link = util_local_to_global_coord(dungeon->w[floor], dungeon->room_w, room, spawn);

	return;
}


static void fill_side(struct dungeon *dungeon, int floor, int room, int offset, int dir, int tile) {
	int i;

	if (!dir) {
		offset *= (dungeon->room_w * dungeon->room_h - dungeon->room_w);
		for (i = 0; i < dungeon->room_w; i++)
			dungeon->room_map[floor][room][i + offset] = tile;
	} else {
		offset *= (dungeon->room_w - 1);
		for (i = 0; i < dungeon->room_h; i++)
			dungeon->room_map[floor][room][i * dungeon->room_w + offset] = tile;
	}

	return;
}


static void spawn_walls(struct dungeon *dungeon, int f, int i) {
	int l, r, t, t2;

	if (dungeon->data[f][i] & MAP_ROOM_TMP_VISIT)
		return;
	dungeon->data[f][i] |= MAP_ROOM_TMP_VISIT;

	for (l = 0; l < 4; l++) {
		r = util_dir_conv(i, l, dungeon->w[f], dungeon->h[f]);
		t = dungeon->data[f][r] & 0xFF;
		t2 = dungeon->data[f][i] & 0xFF;

		if (!t || r == i || t == MAP_ROOM_TYPE_BOSS_ROOM || t2 == MAP_ROOM_TYPE_BOSS_ROOM)
			fill_side(dungeon, f, i, (l & 2) >> 1, !(l & 1), ROOM_TILE_WALL);
		else
			spawn_walls(dungeon, f, r);
		if (t == MAP_ROOM_TYPE_BOSS_ROOM)
			spawn_walls(dungeon, f, r);
	}

	return;
}


static void spawn_walls_inside(struct dungeon *dungeon, int f) {
	int i, dir, target;

	for (i = 0; i < dungeon->layout_scratchuse; i++) {
		dir = (random_get() & 03);
		target = util_dir_conv(dungeon->layout_scratchpad[i], dir, dungeon->w[f], dungeon->h[f]);
		if ((unsigned int) target == dungeon->layout_scratchpad[i])
			continue;
		if ((dungeon->data[f][target] & 0377) != MAP_ROOM_TYPE_ROOM)
			continue;
		dungeon->data[f][dungeon->layout_scratchpad[i]] |= (1 << (28 + dir));
		if (!dungeon_room_reachable(dungeon, dungeon->layout_scratchpad[i], target, f)) {
			dungeon->data[f][dungeon->layout_scratchpad[i]] ^= (1 << (28 + dir));
			continue;
		}

		fill_side(dungeon, f, dungeon->layout_scratchpad[i], (dir & 2) >> 1, (dir & 1), ROOM_TILE_WALL);
	}

	return floor_clear_visit(dungeon, f);
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
			default:
				door = ((dungeon->data[floor][i] & 0377) == MAP_ROOM_TYPE_BOSS_ROOM) ? ROOM_TILE_DOOR : 00;
				break;
			case MAP_ROOM_TYPE_BOSS_ROOM:
				door = ROOM_TILE_DOOR_BOSS;
				break;
		}

		if (l & 01) {
			y = (l & 02) ? dungeon->room_h - 01 : 00;
			x = dungeon->room_w >> 01;
		} else {
			y = dungeon->room_h >> 01;
			x = (l & 02) ? dungeon->room_w - 01 : 00;
		}

		if (door != 00)
			dungeon->room_map[floor][i][x + y * dungeon->room_w] = door;
	}

	return;
}


void dungeon_init_floor(struct dungeon *dungeon, int room_w, int room_h, int max_enemy, int entrance_floor) {
	int i, j, f, g, last_room, last_tile, rooms, spawn, boss_room;
	
	dungeon->room_w = room_w;
	dungeon->room_h = room_h;
	dungeon->room_scratchpad = malloc(sizeof(unsigned int) * room_w * room_h);
	dungeon->entrance_floor = entrance_floor;
	last_room = last_tile = -1;

	for (f = 0; f < dungeon->floors; f++) {
		boss_room = -1;
		for (i = g = 0; i < dungeon->w[f] * dungeon->h[f]; i++) {
			if (!(dungeon->data[f][i] & 0xFF))
				continue;
			dungeon->room_map[f][i] = dungeon_generate_room_template(dungeon->room_w, dungeon->room_h);
			if ((dungeon->data[f][i] & 0xFF) == MAP_ROOM_TYPE_ROOM)
				dungeon->layout_scratchpad[g++] = i;
			else if ((dungeon->data[f][i] & 0xFF) == MAP_ROOM_TYPE_BOSS_ROOM)
				boss_room = i;
		}

		rooms = g;
		dungeon->layout_scratchuse = rooms;
		spawn_walls(dungeon, f, dungeon->layout_scratchpad[0]);
		floor_clear_visit(dungeon, f);
		spawn_walls_inside(dungeon, f);
		
		if (boss_room >= 0)
			spawn_doors(dungeon, boss_room, f);
		for (i = 0; i < rooms; i++) {
			spawn_doors(dungeon, dungeon->layout_scratchpad[i], f);
			spawn_puzzle_part(dungeon, dungeon->layout_scratchpad[i], f, 0);
	
			for (j = 0; (unsigned int) j < random_get() % ((unsigned int) max_enemy); j++)
				spawn_tile(dungeon, f, dungeon->layout_scratchpad[i], ROOM_TILE_ENEMY0 + random_get() % 8);
		}

		if (f == entrance_floor)
			dungeon->entrance = dungeon->layout_scratchpad[random_get() % rooms];
		
		if (f > 0) {
			spawn = dungeon->layout_scratchpad[random_get() % rooms];
			dungeon->room_map[f-1][last_room][last_tile] |= (spawn << 16);
			dungeon->info[f].stair_down = util_local_to_global_coord(dungeon->w[f], dungeon->room_w, spawn, last_tile);
			g = spawn_tile(dungeon, f, spawn, ROOM_TILE_WAY_DOWN | (last_room << 16));
			dungeon->info[f-1].stair_down = util_local_to_global_coord(dungeon->w[f], dungeon->room_w, last_room, last_tile);
		}
		
		if (f + 1 < dungeon->floors) {
			spawn = dungeon->layout_scratchpad[random_get() % rooms];
			last_room = spawn;
			last_tile = spawn_tile(dungeon, f, spawn, ROOM_TILE_WAY_UP);
		}
	}


	return;
}


void *dungeon_free_generate_dungeon(struct dungeon *dungeon) {
	int i, j;

	for (i = 0; i < dungeon->floors; i++) {
		for (j = 0; j < dungeon->w[i] * dungeon->h[i]; j++)
			free(dungeon->room_map[i][j]);
		free(dungeon->data[i]);
	}

	free(dungeon->w);
	free(dungeon->h);
	free(dungeon->puzzle);
	free(dungeon->info);
	free(dungeon->room_scratchpad);
	free(dungeon->layout_scratchpad);
	free(dungeon);
	
	return NULL;
}


static int dungeon_add_object(struct dungeon_use *dngu) {
	int id;

	id = dngu->objects++;
	dngu->object = realloc(dngu->object, sizeof(*dngu->object) * dngu->objects);
	return id;
}


struct dungeon_use *dungeon_make_usable(struct dungeon *dungeon, struct autotile *at) {
	struct dungeon_use *dngu;
	int i, j, x, y;

	dngu = malloc(sizeof(*dngu));
	dngu->object = NULL;
	dngu->objects = 0;
	dngu->w = malloc(sizeof(*(dngu->w)) * dungeon->floors);
	dngu->h = malloc(sizeof(*(dngu->h)) * dungeon->floors);
	dngu->floors = dungeon->floors;
	for (i = 0; i < dungeon->floors; i++) {
		dngu->w[i] = dungeon->w[i] * dungeon->room_w;
		dngu->h[i] = dungeon->h[i] * dungeon->room_h;
	}

	dngu->tile_data = malloc(sizeof((*(dngu->tile_data))) * dungeon->floors);
	dngu->floor_info = malloc(sizeof(*dngu->floor_info) * dungeon->floors);
	memcpy(dngu->floor_info, dungeon->info, sizeof(*dngu->floor_info) * dungeon->floors);
	
	for (i = 0; i < dungeon->floors; i++)
		dngu->tile_data[i] = calloc(dngu->w[i] * dngu->h[i], sizeof(**(dngu->tile_data)));

	for (i = 0; i < dungeon->floors; i++)
		for (j = 0; j < dungeon->w[i] * dungeon->h[i]; j++) {
			x = (j % dungeon->w[i]) * dungeon->room_w;
			y = (j / dungeon->w[i]) * dungeon->room_h;
			if (!dungeon->room_map[i][j])
				continue;
			util_blt(dngu->tile_data[i], dngu->w[i], dngu->h[i], x, y, dungeon->room_map[i][j], dungeon->room_w, dungeon->room_h, 0, 0);
		}

	for (i = 0; i < dungeon->floors; i++)
		for (j = 0; j < dngu->w[i] * dngu->h[i]; j++) {
			if ((dngu->tile_data[i][j] & 0xFF) < 32)
				continue;
			x = dungeon_add_object(dngu);
			dngu->object[x].x = j % dngu->w[i];
			dngu->object[x].y = j / dngu->w[i];
			dngu->object[x].l = i;
			dngu->object[x].data = NULL;
			if ((dngu->tile_data[i][j] & 0xFF) >= 48) {
				/* FIXME: Add puzzle object here */
				dngu->object[x].type = ROOM_OBJECT_TYPE_PUZZLE_EL;
			} else
				dngu->object[x].type = ROOM_OBJECT_TYPE_NPC;
			dngu->tile_data[i][j] = ROOM_TILE_FLOOR_KEEP;
		}

	return dngu;
}
