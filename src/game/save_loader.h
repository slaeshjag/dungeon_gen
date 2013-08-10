#ifndef __SAVE_LOADER_H__
#define	__SAVE_LOADER_H__

#include "savefile.h"
#include <darnit/darnit.h>

#define	TILE_W					32
#define	TILE_H					32


struct dungeon_map_floor {
	DARNIT_TILEMAP				*tm;
	int					stair_up;
	int					stair_down;
};


struct dungeon_map {
	struct dungeon_map_floor		*floor;
	int					floors;
	struct savefile_dungeon_object		*object;
	int					objects;
	struct savefile_dungeon_puzzle_part	*puzzle;
	int					puzzles;
	int					entrance_floor;
	int					entrance;
	DARNIT_TILESHEET			*ts;
};


struct dungeon_map *dungeon_load(int dungeon_number);


#endif
