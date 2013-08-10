#include <darnit/darnit.h>
#include <string.h>
#include <time.h>

#include "world_gen/generate_world.h"
#include "common/random.h"
#include "save_loader.h"

int main(int argc, char **argv) {

	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	generate_world(0);
	d_fs_mount("world_0.save");
	dungeon_load(0);

	for (;;) {
		d_loop();
	}

	d_quit();
	return 0;
}
