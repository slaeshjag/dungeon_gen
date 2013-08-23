#include "world.h"


void world_dungeon_load(struct world_state *ws, int dungeon) {
	dungeon_unload(ws->dm);
	ws->dm = dungeon_load(dungeon);
	ws->active_dungeon = dungeon;
}


void world_init() {
	struct world_state *ws;
	ws = malloc(sizeof(*ws));
	ws->active_dungeon = -1;
	ws->dm = NULL;
}
