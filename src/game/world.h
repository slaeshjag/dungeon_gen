#ifndef __WORLD_H__
#define	__WORLD_H__

#include "save_loader.h"

enum world_state_e {
	WORLD_STATE_OVERWORLD,
	WORLD_STATE_DUNGEON,
};

struct {
	enum world_state_e		state;
	int				active_dungeon;
	struct dungeon_map		*dm;
	struct character_data		*char_data;
} ws;


#endif
