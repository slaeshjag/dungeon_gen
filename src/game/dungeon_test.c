#include <darnit/darnit.h>
#include <string.h>
#include <time.h>

#include "world_gen/generate_world.h"
#include "common/random.h"
#include "save_loader.h"
#include "world.h"
#include "character.h"

int main(int argc, char **argv) {
	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	generate_world(0);

	/* TODO: Move this into a separate function loading a world */
	world_init();
	d_fs_mount("world_0.save");
	character_init();
	save_load_deps();
	
	camera_init();
	ws.camera.follow_char = 0;
	ws.camera.player = 0;

	ws.active_dungeon = 0;
	ws.new_state = WORLD_STATE_DUNGEON;
	world_loop();



	for (;;) {
		world_loop();
		d_loop();
	}

	d_quit();
	return 0;
}
