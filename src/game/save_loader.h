#ifndef __SAVE_LOADER_H__
#define	__SAVE_LOADER_H__

#include "savefile.h"
#include <darnit/darnit.h>

#define	TILE_W					32
#define	TILE_H					32

struct char_gfx {
	unsigned int				link;
	unsigned int				face_w;
	unsigned int				face_h;
	unsigned int				sprite_w;
	unsigned int				sprite_h;
	unsigned int				frames;
	unsigned int				directions;
	unsigned int				*face;
	int					*sprite_data;
	int					*sprite_hitbox;
	DARNIT_TILESHEET			*sprite_ts;
};


struct dungeon_map_floor {
	DARNIT_TILEMAP				*tm;
	DARNIT_TILEMAP				*overlay;
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

int character_gfx_data_characters();
struct char_gfx *character_gfx_data_load(unsigned int char_num);
void *character_gfx_data_unload(struct char_gfx *cg);

struct dungeon_map *dungeon_load(int dungeon_number);
void *dungeon_unload(struct dungeon_map *dm);

int save_load_deps();

#endif
