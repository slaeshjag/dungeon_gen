#include <darnit/darnit.h>
#include <string.h>
#include <time.h>

#include "world_gen/generate_world.h"
#include "common/random.h"

int main(int argc, char **argv) {
	DARNIT_TILESHEET *ts, *room_ts;
	DARNIT_TILEMAP *tm, *room_tm;
	DARNIT_STRINGTABLE *st;
	struct autotile *at;
	int cam_x, cam_y, room, current_room, current_floor;
	struct dungeon *dungeon;

	d_init("dungeon_gen", "dungeon_gen", NULL);
	random_seed(time(NULL));

	generate_world(0);

	for (;;) {
		d_loop();
	}

	d_quit();
	return 0;
}
