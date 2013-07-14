#ifndef __DUNGEON_GENERATE_H__
#define	__DUNGEON_GENERATE_H__

#define	MAP_ROOM_HAS_KEY		0x80000000
#define	MAP_ROOM_HAS_LOCK		0x40000000
#define	MAP_ROOM_HAS_BOSSKEY		0x20000000
#define	MAP_ROOM_LOCK_UNTIL_SAFE	0x10000000
#define	MAP_ROOM_TMP_VISIT		0x1000


struct dungeon_puzzle_part {
	int				room_link;
	unsigned int			status;
};


struct dungeon {
	int				*w;
	int				*h;
	unsigned int			**data;
	int				floors;
	struct dungeon_puzzle_part	*puzzle;
	unsigned int			***room_map;
	int				room_w;
	int				room_h;
	int				entrance;
	int				entrance_floor;
	unsigned int			*room_scratchpad;
	unsigned int			*layout_scratchpad;
};


struct dungeon *dungeon_layout_new(int w, int h, int max_room, int min_room, int boss_s, int floors);
void dungeon_init_floor(struct dungeon *dungeon, int room_w, int room_h, int max_enemy, int entrance_floor);

enum MAP_ROOM_TYPE {
	MAP_ROOM_TYPE_NOTHING		= 0,
	MAP_ROOM_TYPE_ROOM		= 1,
	MAP_ROOM_TYPE_ENTRANCE		= 2,
	MAP_ROOM_TYPE_BOSS_ROOM		= 3,
};


enum ROOM_TILE {
	ROOM_TILE_VOID			= 0,
	ROOM_TILE_WALL			= 1,
	ROOM_TILE_DOOR			= 2,
	ROOM_TILE_DOOR_LOCK		= 3,
	ROOM_TILE_DOOR_BOSS		= 4,
	ROOM_TILE_KEY_CHEST		= 5,
	ROOM_TILE_BOSS_KEY_CHEST	= 6,
	ROOM_TILE_CHEST			= 7,
	ROOM_TILE_FLOOR			= 10,
	ROOM_TILE_WATER			= 11,

	ROOM_TILE_ENEMY0		= 32,
};


enum PUZZLE_TYPE {
	PUZZLE_TYPE_NONE		= 0,
	PUZZLE_TYPE_KILL_ENEMIES	= 1,
};


#endif
