#include <darnit/darnit.h>

#include "world.h"
#include "save_loader.h"
#include "character.h"
#include "camera.h"


void world_dungeon_load(int dungeon) {
	dungeon_unload(ws.dm);
	ws.dm = dungeon_load(dungeon);
	ws.active_dungeon = dungeon;
}


void world_init() {
	ws.active_dungeon = -1;
	ws.dm = NULL;
}


void world_reset() {
	ws.active_dungeon = -1;
	ws.dm = dungeon_unload(ws.dm);
	ws.camera.follow_char = -1;
	character_destroy();

	return;
}


void world_loop() {
	int p, f;

	switch (ws.state) {
		case WORLD_STATE_MAINMENU:
		case WORLD_STATE_OVERWORLD:
			/* TODO: Implement */
			break;
		case WORLD_STATE_DUNGEON:
			character_loop();
			camera_loop();
			p = ws.camera.player;
			f = ws.char_data->entry[p]->l;

			d_render_begin();
			d_tilemap_camera_move(ws.dm->floor[f].tm, ws.camera.x, ws.camera.y);
			d_tilemap_draw(ws.dm->floor[f].tm);
			/* NOTE: This depends on the collision buffer not changing */
			d_render_offset(ws.camera.x, ws.camera.y);
			d_render_blend_enable();
			character_render_layer(character_find_visible(), f);
			d_render_blend_disable();
			d_render_offset(0, 0);
			/* TODO: Render a UI of some sort */
			d_render_end();
	}

	return;
}
