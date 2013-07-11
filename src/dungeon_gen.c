#include <darnit/darnit.h>
#include "dungeon_generate.h"
#include "util.h"


int main(int argc, char **argv) {
	DARNIT_TILESHEET *ts;
	DARNIT_TILEMAP *tm;
	struct dungeon *dungeon;

	d_init("dungeon_gen", "dungeon_gen", NULL);

	ts = d_render_tilesheet_load("res/level_gfx.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	tm = d_tilemap_new(0xFFF, ts, 0xFFF, 16, 15);

	dungeon = dungeon_layout_new(tm->w, tm->h, 24, 16, 1);
	dungeon_layout_spawn_keylocks(dungeon, 5, 1);
	dungeon_init_floor(dungeon, 32, 32);

	util_blt(tm->data, tm->w, tm->h, 0, 0, dungeon->data, dungeon->w, dungeon->h, 0, 0);

	d_tilemap_recalc(tm);

	for (;;) {
		d_render_begin();
		d_tilemap_draw(tm);
		d_render_end();
		d_loop();
	}

	d_quit();
	return 0;
}
