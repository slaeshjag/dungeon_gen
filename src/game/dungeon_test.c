#include <darnit/darnit.h>
#include <string.h>
#include <time.h>

#include "world_gen/generate_world.h"
#include "common/random.h"
#include "save_loader.h"

int main(int argc, char **argv) {
	struct dungeon_map *dm;
	int x, y;

	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	generate_world(0);
	d_fs_mount("world_0.save");
	dm = dungeon_load(0);
	
	x = (dm->entrance % dm->floor->tm->w) * 32 - 400;
	y = (dm->entrance / dm->floor->tm->w) * 32 - 240;
	d_tilemap_camera_move(dm->floor->tm, x, y);

	for (;;) {
		d_render_begin();
		d_tilemap_draw(dm->floor->tm);
		d_render_end();
		d_loop();
	}

	d_quit();
	return 0;
}
