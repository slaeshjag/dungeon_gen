#include <darnit/darnit.h>
#include <string.h>
#include <time.h>

#include "world_gen/generate_world.h"
#include "common/random.h"
#include "save_loader.h"
#include "world.h"
#include "character.h"

int main(int argc, char **argv) {
	int x, y;

	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	generate_world(0);

	/* TODO: Move this into a separate function loading a world */
	world_init();
	d_fs_mount("world_0.save");
	character_init();

	ws.dm = dungeon_load(0);
	character_spawn_entry(2, NULL, ws.dm->entrance % ws.dm->floor->tm->w * 32, 
		ws.dm->entrance / ws.dm->floor->tm->w * 32, ws.dm->entrance_floor);

	#if 0
	x = (ws.dm->entrance % dm->floor->tm->w) * 32 - 400;
	y = (ws.dm->entrance / dm->floor->tm->w) * 32 - 240;
	#endif

	ws.state = WORLD_STATE_DUNGEON;
	camera_init();
	ws.camera.follow_char = 0;
	ws.camera.player = 0;

	for (;;) {
		#if 0
		d_render_begin();
		d_tilemap_draw(dm->floor->tm);
		d_render_offset(x, y);
		d_render_blend_enable();
		d_sprite_draw(ws.char_data->entry[0]->sprite);
		d_render_blend_disable();
		d_render_end();
		#endif
		world_loop();
		d_loop();
	}

	d_quit();
	return 0;
}
