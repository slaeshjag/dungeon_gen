#include "world.h"


void world_dungeon_load(int dungeon) {
	dungeon_unload(ws.dm);
	ws.dm = dungeon_load(dungeon);
	ws.active_dungeon = dungeon;
}


void world_init() {
	ws.active_dungeon = -1;
	ws.dm = NULL;
}
