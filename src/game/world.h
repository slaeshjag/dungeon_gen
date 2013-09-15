#ifndef __WORLD_H__
#define	__WORLD_H__

#include "save_loader.h"
#include "camera.h"
#include "savedata.h"

#define	WORLD_FADE_TIME			500

enum world_state_e {
	WORLD_STATE_MAINMENU,
	WORLD_STATE_OVERWORLD,
	WORLD_STATE_DUNGEON,
};

struct world_state {
	enum world_state_e		state;
	enum world_state_e		new_state;
	int				active_dungeon;
	struct dungeon_map		*dm;
	struct character_data		*char_data;
	struct camera			camera;
	struct savedata			savedata;
} ws;

void world_load(int world_num);
void world_init();
void world_loop();
int world_calc_tile(int x, int y, int l);
unsigned int world_get_tile(int x, int y, int l);
unsigned int world_get_tile_i(int i, int l);

#endif
