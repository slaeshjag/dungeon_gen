#include <darnit/darnit.h>
#include <string.h>
#include <time.h>

#include "world_gen/generate_world.h"
#include "common/random.h"
#include "save_loader.h"
#include "world.h"
#include "character.h"

int main(int argc, char **argv) {
	struct dungeon_map *dm;
	int x, y;

	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	generate_world(0);

	/* TODO: Move this into a separate function loading a world */
	world_init();
	d_fs_mount("world_0.save");
	character_init();

	dm = dungeon_load(0);
	character_spawn_entry(2, NULL, dm->entrance % dm->floor->tm->w * 32, 
		dm->entrance / dm->floor->tm->w * 32, dm->entrance_floor);

	x = (dm->entrance % dm->floor->tm->w) * 32 - 400;
	y = (dm->entrance / dm->floor->tm->w) * 32 - 240;
	d_tilemap_camera_move(dm->floor->tm, x, y);

	for (;;) {
		d_render_begin();
		d_tilemap_draw(dm->floor->tm);
		d_render_offset(x, y);
		d_render_blend_enable();
		d_sprite_draw(ws.char_data->entry[0]->sprite);
		d_render_blend_disable();
		d_render_end();
		d_loop();
	}

	d_quit();
	return 0;
}
