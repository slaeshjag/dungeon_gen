#include <darnit/darnit.h>
#include <string.h>

#include "dungeon_generate.h"
#include "util.h"

#define	ROOM_W			12
#define	ROOM_H			12
#define	FLOORS			5

int main(int argc, char **argv) {
	DARNIT_TILESHEET *ts, *room_ts;
	DARNIT_TILEMAP *tm, *room_tm;
	int cam_x, cam_y, room, current_room, current_floor;
	struct dungeon *dungeon;

	d_init("dungeon_gen", "dungeon_gen", NULL);

	ts = d_render_tilesheet_load("res/level_gfx.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	room_ts = d_render_tilesheet_load("res/room_gfx.png", 32, 32, DARNIT_PFORMAT_RGB5A1);
	tm = d_tilemap_new(0xFFF, ts, 0xFFF, 16, 15);
	room_tm = d_tilemap_new(0xFFF, room_ts, 0xFFF, ROOM_W, ROOM_H);
	cam_x = cam_y = 0;

	dungeon = dungeon_layout_new(tm->w, tm->h, 24, 16, 1, FLOORS);
	dungeon_init_floor(dungeon, ROOM_W, ROOM_H, 4, FLOORS - 1);
	memcpy(room_tm->data, dungeon->room_map[FLOORS-1][dungeon->entrance], sizeof(unsigned int) * room_tm->w * room_tm->h);
	current_room = dungeon->entrance;
	current_floor = dungeon->entrance_floor;
	d_tilemap_recalc(room_tm);

	util_blt(tm->data, tm->w, tm->h, 0, 0, dungeon->data[current_floor], dungeon->w[current_floor], dungeon->h[current_floor], 0, 0);
	d_tilemap_recalc(tm);

	for (;;) {
		room = -1;
		if (d_keys_get().left)
			cam_x -= 4;
		if (d_keys_get().right)
			cam_x += 4;
		if (d_keys_get().up)
			cam_y -= 4;
		if (d_keys_get().down)
			cam_y += 4;
		if (d_keys_get().l)
			room = util_dir_conv(current_room, 0, dungeon->w[current_floor], dungeon->h[current_floor]);
		if (d_keys_get().r)
			room = util_dir_conv(current_room, 2, dungeon->w[current_floor], dungeon->h[current_floor]);
		if (d_keys_get().y)
			room = util_dir_conv(current_room, 1, dungeon->w[current_floor], dungeon->h[current_floor]);
		if (d_keys_get().x)
			room = util_dir_conv(current_room, 3, dungeon->w[current_floor], dungeon->h[current_floor]);
		if (room != -1 && dungeon->room_map[current_floor][room]) {
			d_keys_set(d_keys_get());
			memcpy(room_tm->data, dungeon->room_map[current_floor][room], sizeof(unsigned int) * room_tm->w * room_tm->h);
			d_tilemap_recalc(room_tm);
			current_room = room;
		}
			
		d_tilemap_camera_move(room_tm, cam_x, cam_y);
		d_render_begin();
		/*d_tilemap_draw(tm);*/
		d_tilemap_draw(room_tm);
		d_render_end();
		d_loop();
	}

	d_quit();
	return 0;
}
