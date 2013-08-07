#include <darnit/darnit.h>
#include <string.h>
#include <time.h>

#include "world_gen/generate_world.h"
#include "common/random.h"

int main(int argc, char **argv) {

	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	generate_world(0);

	for (;;) {
		d_loop();
	}

	d_quit();
	return 0;
}
