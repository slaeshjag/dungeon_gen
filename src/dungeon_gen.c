#include <darnit/darnit.h>
#include "dungeon_generate.h"


int main(int argc, char **argv) {
	DARNIT_TILESHEET *ts;
	DARNIT_TILEMAP *tm;

	d_init("dungeon_gen", "dungeon_gen", NULL);

	ts = d_render_tilesheet_load("res/level_gfx.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	tm = d_tilemap_new(0xFFF, ts, 0xFFF, 16, 15);

	dungeon_layout_new(tm->data, tm->w, tm->h, 24, 16, 1);
	d_tilemap_recalc(tm);

	for (;;) {
		d_render_begin();
		if (d_keys_get().start) {
			d_keys_set(d_keys_get());
			dungeon_layout_new(tm->data, tm->w, tm->h, 24, 16, 1);
			d_tilemap_recalc(tm);
		}
		
		d_tilemap_draw(tm);
		d_render_end();
		d_loop();
	}

	d_quit();
	return 0;
}
