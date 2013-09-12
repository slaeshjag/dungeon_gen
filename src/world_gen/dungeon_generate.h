#ifndef __DUNGEON_GENERATE_H__
#define	__DUNGEON_GENERATE_H__

#include "random.h"
#include "autotiler.h"
#include <darnit/darnit.h>
#define	MAP_ROOM_TMP_VISIT		0x1000


enum ROOM_OBJECT_TYPE {
	ROOM_OBJECT_TYPE_NONE		= 0,
	ROOM_OBJECT_TYPE_NPC		= 1,
	ROOM_OBJECT_TYPE_PUZZLE_EL	= 2,
};


struct dungeon_puzzle_part {
	int				room_link;
	int				layer;
	int				group;
	int				depend;
	int				provide;
};


struct dungeon_floor_info {
	int				stair_up;
	int				stair_down;
	int				w;
	int				h;
	unsigned int			*tile_data;
	unsigned int			*overlay_data;
};


struct dungeon {
	int				*w;
	int				*h;
	unsigned int			**data;
	int				floors;
	struct dungeon_puzzle_part	*puzzle;
	int				puzzles;
	struct dungeon_floor_info	*info;
	unsigned int			***room_map;
	int				room_w;
	int				room_h;
	int				entrance_tile;
	int				entrance;
	int				entrance_floor;
	unsigned int			*room_scratchpad;
	unsigned int			*layout_scratchpad;
	int				layout_scratchuse;
	int				group_cnt;
};


struct dungeon_object {
	int				x;
	int				y;
	int				l;
	enum ROOM_OBJECT_TYPE		type;
	unsigned int			link;
	int				group;
	int				subtype;
	int				saveslot;
};


struct dungeon_use {
	int				floors;
	struct dungeon_floor_info	*floor_info;
	struct dungeon_object		*object;
	int				objects;
	struct dungeon_puzzle_part	*puzzle;
	int				puzzles;
	int				entrance;
	int				entrance_floor;
};


struct dungeon_use *dungeon_generate_diamond_square(int size);
struct dungeon *dungeon_layout_new(int w, int h, int max_room, int min_room, int boss_s, int floors);
void dungeon_init_floor(struct dungeon *dungeon, int room_w, int room_h, int max_enemy, int entrance_floor);
void *dungeon_free_generate_dungeon(struct dungeon *dungeon);
void *dungeon_free_usable(struct dungeon_use *dngu);
struct dungeon_use *dungeon_make_usable(struct dungeon *dungeon);

enum MAP_ROOM_TYPE {
	MAP_ROOM_TYPE_NOTHING		= 0,
	MAP_ROOM_TYPE_ROOM		= 1,
	MAP_ROOM_TYPE_ENTRANCE		= 2,
	MAP_ROOM_TYPE_BOSS_ROOM		= 3,
};


enum MAP_ROOM_BLOCK {
	MAP_ROOM_BLOCK_SOUTH		= 0x80000000,
	MAP_ROOM_BLOCK_EAST		= 0x40000000,
	MAP_ROOM_BLOCK_NORTH		= 0x20000000,
	MAP_ROOM_BLOCK_WEST		= 0x10000000,
};


enum ROOM_TILE {
	ROOM_TILE_VOID			= 0,
	ROOM_TILE_WALL			= 1,
	ROOM_TILE_DOOR			= 2,
	ROOM_TILE_DOOR_BOSS		= 3,
	ROOM_TILE_WAY_DOWN		= 4,
	ROOM_TILE_WAY_UP		= 5,
	ROOM_TILE_BOSS_KEY_CHEST	= 6,
	ROOM_TILE_CHEST			= 7,
	ROOM_TILE_FLOOR			= 10,
	ROOM_TILE_FLOOR_KEEP		= 11,
	ROOM_TILE_WATER			= 12,
	ROOM_TILE_ENTRANCE		= 16,

	ROOM_TILE_ENEMY0		= 32,

	ROOM_TILE_PUZZLE_BUTTON		= 48,
	ROOM_TILE_PUZZLE_SLIDEBLOCK	= 49,
};


enum PUZZLE_LINK {
	ROOM_TILE_PUZZLE_DEPEND		= 0x80000000,
	ROOM_TILE_PUZZLE_PROVIDE	= 0x40000000,
	ROOM_TILE_PUZZLE_COULD_DEPEND	= 0x20000000,
};


enum PUZZLE_TYPE {
	PUZZLE_TYPE_NONE		= 0,
	PUZZLE_TYPE_KILL_ENEMIES	= 1,
};


#endif
