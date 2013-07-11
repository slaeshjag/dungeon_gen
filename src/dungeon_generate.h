#ifndef __DUNGEON_GENERATE_H__
#define	__DUNGEON_GENERATE_H__

void dungeon_layout_new(unsigned int *data, int w, int h, int max_room, int min_room, int boss_s);

enum MAP_ROOM_TYPE {
	MAP_ROOM_TYPE_NOTHING		= 0,
	MAP_ROOM_TYPE_ROOM		= 1,
	MAP_ROOM_TYPE_ENTRANCE		= 2,
	MAP_ROOM_TYPE_BOSS_ROOM		= 3,
};

#endif
