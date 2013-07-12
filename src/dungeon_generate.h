#ifndef __DUNGEON_GENERATE_H__
#define	__DUNGEON_GENERATE_H__

#define	MAP_ROOM_HAS_KEY		0x80000000
#define	MAP_ROOM_HAS_LOCK		0x40000000
#define	MAP_ROOM_HAS_BOSSKEY		0x20000000
#define	MAP_ROOM_TMP_VISIT		0x1000


struct dungeon {
	int				w;
	int				h;
	unsigned int			*data;
	unsigned int			**room_map;
	int				room_w;
	int				room_h;
};


struct dungeon *dungeon_layout_new(int w, int h, int max_room, int min_room, int boss_s);
void dungeon_layout_spawn_keylocks(struct dungeon *dungeon, int keylocks, int boss);
void dungeon_init_floor(struct dungeon *dungeon, int room_w, int room_h);

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
};


#endif
