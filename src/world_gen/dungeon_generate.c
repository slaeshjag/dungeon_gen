#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dungeon_generate.h"
#include "dungeon_generate_structure.h"
#include "util.h"
#include "random.h"

//static unsigned int structure[256];
#define	CLEAR_STRUCTURE()		(memset(structure, 0, sizeof(int) * 256));

static void diamond_square(int *map, int side_len) {
	#define INDEX(x, y) ((y)*side_len+(x))
	
	int x, y, len=side_len, half, r=5000, tmp;
	
	map[0]=map[INDEX(len-1, 0)]=map[INDEX(0, len-1)]=map[INDEX(len-1, len-1)]=r/2;
	
	for(len=side_len-1; len>=2; len/=2, r/=2) {
		half=len/2;
		for(y=0; y<side_len-1; y+=len)
			for(x=0; x<side_len-1; x+=len) {
				if(map[INDEX(x+half, y+half)])
					continue;
				map[INDEX(x+half, y+half)]=(map[INDEX(x, y)]+map[INDEX(x+len, y)]+map[INDEX(x, y+len)]+map[INDEX(x+len, y+len)])/4+random_get()%r;
				if(r<5000)
					map[INDEX(x+half, y+half)]-=r/2;
				else
					map[INDEX(x+half, y+half)]=0;
			}
		
		for(y=0; y<side_len-1; y+=half)
			for(x=(y+half)%len; x<side_len-1; x+=len) {
				if(map[INDEX(x, y)])
					continue;
				tmp=(map[INDEX((x-half+len)%side_len, y)]+map[INDEX((x+half)%side_len, y)]+map[INDEX(x, (y-half+len)%side_len)]+map[INDEX(x, (y+half)%side_len)])/4+random_get()%r;
				if(r<5000)
					tmp-=r/2;
				else
					tmp=0;
				map[INDEX(x, y)]=tmp;
				if(!x)
					map[INDEX(side_len-1, y)]=tmp;
				if(!y)
					map[INDEX(x, side_len-1)]=tmp;
			}
	}
	
	#undef INDEX
}

struct dungeon_use *dungeon_generate_diamond_square(int size, int *seed) {
	struct dungeon_use *du;
	void *tmp_data;
	int i;
	//int tile_norm = 0;

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
	tmp_data = seed ? seed : calloc((size + 1) * (size + 1) * sizeof(int), 1);
	diamond_square(tmp_data, size + 1);
	util_blt(du->floor_info[0].tile_data, size, size, 0, 0, tmp_data, size + 1, size + 1, 1, 1);
	
	for (i = 0; i < size * size; i++) {
		//du->floor_info[0].tile_data[i] = du->floor_info[0].tile_data[i] > 5000 ? 0 : 1;
		//du->floor_info[0].tile_data[i] = (du->floor_info[0].tile_data[i]) ? (du->entrance = i, ROOM_TILE_FLOOR) : 0xF0000;
		du->floor_info[0].tile_data[i]=(du->floor_info[0].tile_data[i]==0?0:5000/du->floor_info[0].tile_data[i])>0?0xF0000:(du->entrance = i, ROOM_TILE_FLOOR);
	}
	
	if(!seed)
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


static int puzzle_struct_add(struct dungeon *dungeon) {
	struct dungeon_puzzle_part *tmp;

	if (!(tmp = realloc(dungeon->puzzle, sizeof(*tmp) * (++dungeon->puzzles))))
		return -1;
	dungeon->puzzle = tmp;
	return dungeon->puzzles - 1;
}


static int dungeon_add_object(struct dungeon_use *dngu) {
	int id;

	id = dngu->objects++;
	dngu->object = realloc(dngu->object, sizeof(*dngu->object) * dngu->objects);
	return id;
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
