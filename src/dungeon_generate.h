#ifndef __DUNGEON_GENERATE_H__
#define	__DUNGEON_GENERATE_H__

#define	MAP_ROOM_HAS_KEY		0x80000000
#define	MAP_ROOM_HAS_LOCK		0x40000000
#define	MAP_ROOM_HAS_BOSSKEY		0x20000000
#define	MAP_ROOM_TMP_VISIT		0x1000

void dungeon_layout_new(unsigned int *data, int w, int h, int max_room, int min_room, int boss_s);
void dungeon_layout_spawn_keylocks(unsigned int *data, int w, int h, int keylocks, int boss);

enum MAP_ROOM_TYPE {
	MAP_ROOM_TYPE_NOTHING		= 0,
	MAP_ROOM_TYPE_ROOM		= 1,
	MAP_ROOM_TYPE_ENTRANCE		= 2,
	MAP_ROOM_TYPE_BOSS_ROOM		= 3,
};

#endif
