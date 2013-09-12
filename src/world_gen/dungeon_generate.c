#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dungeon_generate.h"
#include "dungeon_generate_structure.h"
#include "util.h"
#include "random.h"

static unsigned int structure[256];
#define	CLEAR_STRUCTURE()		(memset(structure, 0, sizeof(int) * 256));

static void diamond_square(unsigned int *map, int side_len) {
	#define INDEX(x, y) ((y)*side_len+(x))
	
	int x, y, len=side_len, half, r=5000, tmp;
	
	map[0]=map[INDEX(len-1, 0)]=map[INDEX(0, len-1)]=map[INDEX(len-1, len-1)]=r/2;
	
	for(len=side_len-1; len>=2; len/=2, r/=2) {
		half=len/2;
		for(y=0; y<side_len-1; y+=len)
			for(x=0; x<side_len-1; x+=len)
				map[INDEX(x+half, y+half)]=(map[INDEX(x, y)]+map[INDEX(x+len, y)]+map[INDEX(x, y+len)]+map[INDEX(x+len, y+len)])/4+rand()%r-r/2;
		
		for(y=0; y<side_len-1; y+=half)
			for(x=(y+half)%len; x<side_len-1; x+=len) {
				tmp=(map[INDEX((x-half+len)%side_len, y)]+map[INDEX((x+half)%side_len, y)]+map[INDEX(x, (y-half+len)%side_len)]+map[INDEX(x, (y+half)%side_len)])/4+rand()%r-r/2;
				map[INDEX(x, y)]=tmp;
				if(!x)
					map[INDEX(side_len-1, y)]=tmp;
				if(!y)
					map[INDEX(x, side_len-1)]=tmp;
			}
	}
	
	#undef INDEX
}

struct dungeon_use *dungeon_generate_diamond_square(int size) {
	struct dungeon_use *du;
	void *tmp_data;
	int i;

	du = malloc(sizeof(*du));
	du->floor_info = malloc(sizeof(*du->floor_info));
	du->floor_info->tile_data = malloc(sizeof(*du->floor_info->tile_data) * size * size);
	du->floor_info->overlay_data = calloc(sizeof(*du->floor_info->overlay_data), size * size);
	du->floor_info->stair_up = -1;
	du->floor_info->stair_down = -1;
	du->floor_info->w = size;
	du->floor_info->h = size;
	du->floors = 1;
	du->object = NULL;
	du->objects = 0;
	du->puzzle = NULL;
	du->puzzles = 0;
	du->entrance = 4 * size;
	du->entrance_floor = 0;
	tmp_data = malloc((size + 1) * (size + 1) * sizeof(int));
	diamond_square(tmp_data, size + 1);
	util_blt(du->floor_info[0].tile_data, size, size, 0, 0, tmp_data, size + 1, size + 1, 1, 1);

	for (i = 0; i < size * size; i++) {
		du->floor_info[0].tile_data[i] = du->floor_info[0].tile_data[i] > 2500 ? 0 : 1;
		du->floor_info[0].tile_data[i] = (du->floor_info[0].tile_data[i]) ? (du->entrance = i, ROOM_TILE_FLOOR) : 0xF0000;
	}
	free(tmp_data);
	
	return du;
}

#if 0

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
			

static int puzzle_find_index(struct dungeon *dungeon, int layer, int index) {
	int i;

	for (i = 0; i < dungeon->puzzles; i++) {
		if (dungeon->puzzle[i].layer != layer)
			continue;
		if (dungeon->puzzle[i].room_link == index)
			return i;
	}

	return -1;
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
	int i, j, xmin = w, ymin = h;

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


static void spawn_puzzle_part(struct dungeon *dungeon, int room, int floor) {
	int complexity, spawn, i, w, h, j, p;

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

	for (j = 0; j < w * h; j++) {
		if (!(structure[j] & 0xE0000000))
			continue;
		p = spawn + (j % w) + (j / w) * dungeon->room_w;
		i = puzzle_struct_add(dungeon);
		dungeon->puzzle[i].room_link = util_local_to_global_coord(dungeon->w[floor], dungeon->room_w, dungeon->room_h, room, p);
		dungeon->puzzle[i].layer = floor;
		dungeon->puzzle[i].group = dungeon->group_cnt;
		dungeon->puzzle[i].depend = (structure[j] & ROOM_TILE_PUZZLE_DEPEND) ? 1 : -1;
		if (dungeon->puzzle[i].depend < 0)
			dungeon->puzzle[i].depend = (structure[j] & ROOM_TILE_PUZZLE_COULD_DEPEND) ? 0 : -1;
		dungeon->puzzle[i].provide = (structure[j] & ROOM_TILE_PUZZLE_PROVIDE) ? 1 : 0;
	}

	dungeon->group_cnt++;

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


void dungeon_puzzle_link(struct dungeon *dungeon) {
	int i, j, k, *dep_buff, *mdep_buff, dep, mdep, prov, *prov_buff, glob_prov, lim, *group_buff, *rand_buff;

	dep_buff = malloc(sizeof(*dep_buff) * dungeon->puzzles);
	prov_buff = malloc(sizeof(*prov_buff) * dungeon->puzzles);
	mdep_buff = malloc(sizeof(*mdep_buff) * dungeon->puzzles);
	rand_buff = malloc(sizeof(*rand_buff) * dungeon->puzzles);
	group_buff = calloc(sizeof(*group_buff), dungeon->puzzles);
	i = dungeon->entrance_floor;
	lim = dungeon->floors;
	glob_prov = 0;
	
	run_floors:

	for (; i < lim; i++) {
		dep = mdep = prov = k = 0;
		for (j = 0; j < dungeon->puzzles; j++) {
			if (dungeon->puzzle[j].layer != i)
				continue;
			if (dungeon->puzzle[j].depend == 1)
				dep_buff[dep++] = j;
			if (dungeon->puzzle[j].depend == 0)
				mdep_buff[mdep++] = j;
			if (dungeon->puzzle[j].provide)
				prov_buff[prov++] = j;
		}

		util_order_randomize(rand_buff, prov);
		for (j = 0; j < prov; j++) {
			if (!group_buff[dungeon->puzzle[prov_buff[rand_buff[j]]].group])
				group_buff[dungeon->puzzle[rand_buff[j]].group] = glob_prov + k + 1;
			k++;
		}

		for (j = 0; j < dep; j++) {
			if (!group_buff[dungeon->puzzle[dep_buff[j]].group])
				group_buff[dungeon->puzzle[dep_buff[j]].group] = glob_prov + (random_get() % k) + 1;
			dungeon->puzzle[dep_buff[j]].depend = group_buff[dungeon->puzzle[dep_buff[j]].group] - 1;
		}
		
		for (j = 0; j < mdep; j++) {
			if (!group_buff[dungeon->puzzle[mdep_buff[j]].group])
				group_buff[dungeon->puzzle[mdep_buff[j]].group] = glob_prov + (random_get() % k) + 1;
			dungeon->puzzle[mdep_buff[j]].depend = !group_buff[dungeon->puzzle[mdep_buff[j]].group] - 1;
		}
	
		glob_prov += prov;
	}

	if (dungeon->entrance_floor != 0 && lim != dungeon->entrance_floor) {
		lim = dungeon->entrance_floor;
		i = 0;
		goto run_floors;
	}

	free(dep_buff);
	free(prov_buff);
	free(mdep_buff);
	free(rand_buff);
	free(group_buff);
	
	return;
}


void dungeon_init_floor(struct dungeon *dungeon, int room_w, int room_h, int max_enemy, int entrance_floor) {
	int i, j, f, g, last_room, last_tile, rooms, spawn, boss_room;
	
	dungeon->room_w = room_w;
	dungeon->room_h = room_h;
	dungeon->room_scratchpad = malloc(sizeof(unsigned int) * room_w * room_h);
	dungeon->entrance_floor = entrance_floor;
	last_room = last_tile = -1;
	dungeon->group_cnt = 0;

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
			spawn_puzzle_part(dungeon, dungeon->layout_scratchpad[i], f);
	
			for (j = 0; (unsigned int) j < random_get() % ((unsigned int) max_enemy); j++)
				spawn_tile(dungeon, f, dungeon->layout_scratchpad[i], ROOM_TILE_ENEMY0 + random_get() % 8);
		}

		if (f == entrance_floor) {
			dungeon->entrance = dungeon->layout_scratchpad[random_get() % rooms];
			/* TODO: Make sure the entrance is actually reachable */
			dungeon->entrance_tile = spawn_tile(dungeon, f, dungeon->entrance, ROOM_TILE_ENTRANCE);
		}
		
		if (f > 0) {
			spawn = dungeon->layout_scratchpad[random_get() % rooms];
			dungeon->room_map[f-1][last_room][last_tile] |= (spawn << 16);
			dungeon->info[f].stair_down = util_local_to_global_coord(dungeon->w[f], dungeon->room_w, dungeon->room_h, last_room, last_tile);
			g = spawn_tile(dungeon, f, spawn, ROOM_TILE_WAY_DOWN | (last_room << 16));
			dungeon->info[f-1].stair_up = util_local_to_global_coord(dungeon->w[f], dungeon->room_w, dungeon->room_h, spawn, g);
		} else
			dungeon->info[f].stair_down = -1;
		
		if (f + 1 < dungeon->floors) {
			spawn = dungeon->layout_scratchpad[random_get() % rooms];
			last_room = spawn;
			last_tile = spawn_tile(dungeon, f, spawn, ROOM_TILE_WAY_UP);

		} else
			dungeon->info[f].stair_up = -1;
		dungeon_puzzle_link(dungeon);
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


struct dungeon_use *dungeon_make_usable(struct dungeon *dungeon) {
	struct dungeon_use *dngu;
	int i, j, x, y;

	dngu = malloc(sizeof(*dngu));
	dngu->object = NULL;
	dngu->objects = 0;
	dngu->w = malloc(sizeof(*(dngu->w)) * dungeon->floors);
	dngu->h = malloc(sizeof(*(dngu->h)) * dungeon->floors);
	dngu->floors = dungeon->floors;
	dngu->puzzles = dungeon->puzzles;
	for (i = 0; i < dungeon->floors; i++) {
		dngu->w[i] = dungeon->w[i] * dungeon->room_w;
		dngu->h[i] = dungeon->h[i] * dungeon->room_h;
	}

	dngu->tile_data = malloc(sizeof((*(dngu->tile_data))) * dungeon->floors);
	dngu->overlay_data = malloc(sizeof((*(dngu->overlay_data))) * dungeon->floors);
	dngu->floor_info = malloc(sizeof(*dngu->floor_info) * dungeon->floors);
	dngu->puzzle = malloc(sizeof(*dngu->puzzle) * dungeon->puzzles);
	dngu->entrance_floor = dungeon->entrance_floor;
	dngu->entrance = util_local_to_global_coord(dungeon->w[dungeon->entrance_floor], dungeon->room_w, dungeon->room_h, dungeon->entrance, dungeon->entrance_tile);
	memcpy(dngu->floor_info, dungeon->info, sizeof(*dngu->floor_info) * dungeon->floors);
	memcpy(dngu->puzzle, dungeon->puzzle, sizeof(*dngu->puzzle) * dungeon->puzzles);
	
	for (i = 0; i < dungeon->floors; i++) {
		dngu->tile_data[i] = calloc(dngu->w[i] * dngu->h[i], sizeof(**(dngu->tile_data)));
		dngu->overlay_data[i] = calloc(dngu->w[i] * dngu->h[i], sizeof(**(dngu->overlay_data)));
	}

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
			dngu->object[x].saveslot = -1;
			if ((dngu->tile_data[i][j] & 0xFF) >= 48) {
				dngu->object[x].type = ROOM_OBJECT_TYPE_PUZZLE_EL;
				dngu->object[x].link = puzzle_find_index(dungeon, i, j);
				dngu->object[x].subtype = (dngu->tile_data[i][j] & 0xFF) - 48;
			} else {
				dngu->object[x].type = ROOM_OBJECT_TYPE_NPC;
				dngu->object[x].link = -1;
				dngu->object[x].subtype = (dngu->tile_data[i][j] & 0xFF) - 32;
			}
			dngu->tile_data[i][j] = ROOM_TILE_FLOOR_KEEP;
		}

	return dngu;
}

#endif

void *dungeon_free_usable(struct dungeon_use *dngu) {
	int i;

	for (i = 0; i < dngu->floors; i++) {
		free(dngu->floor_info[i].overlay_data);
		free(dngu->floor_info[i].tile_data);
	}

	free(dngu->floor_info);
	free(dngu->object);
	free(dngu->puzzle);
	free(dngu);

	return NULL;
}
