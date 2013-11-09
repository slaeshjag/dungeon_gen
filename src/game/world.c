#include <darnit/darnit.h>
#include <string.h>

#include "teleport.h"
#include "world.h"
#include "save_loader.h"
#include "character.h"
#include "camera.h"


void world_dungeon_load(int dungeon) {
	/* TODO: Fix */
	dungeon_unload();
	ws.active_world = dungeon;
	dungeon_load(dungeon);
}


void world_init() {
	ws.active_world = -1;
	ws.state = WORLD_STATE_MAINMENU;
	ws.new_state = WORLD_STATE_MAINMENU;
	ws.dm = NULL;
	ws.textbox = NULL;

	ws.font = d_font_load("res/mainfont.ttf", MAIN_FONT_SIZE, 512, 512);
	ws.te.font = d_font_load("res/mainfont.ttf", TEXTEFFECT_FONT_SIZE, 256, 256);
}


void world_reset() {
	ws.active_world = -1;
	dungeon_unload(ws.dm);
	ws.camera.follow_char = -1;
	character_destroy();
	textbox_destroy();
	texteffect_done();

	return;
}


void world_save() {
	char fname[256];

	sprintf(fname, "world_%i.save", ws.savedata.world);
	savedata_save(fname);

	return;
}



void world_load(int world_num) {
	char fname[256];

	ws.savedata.is = ws.savedata.bs = 0;

	ws.loading_from_save = 1;
	sprintf(fname, "world_%i.save", world_num);
	d_fs_mount(fname);
	character_init();
	camera_init();
	ws.camera.sys = d_render_tilesheet_load("res/ui.png", ws.camera.tile_w, ws.camera.tile_h,
		DARNIT_PFORMAT_RGB5A1);
	dungeon_init();
	texteffect_init();

	/* Just testing textbox */
	textbox_init(800, 96, 0, 480-96, 10, 10, 10, 10);

	save_load_deps();
	teleport_load();
	savedata_load(fname);

	ws.savedata.world = world_num;
	/* FIXME: Temporary hack to test code */
	ws.camera.follow_char = 0;
	ws.camera.player = 0;
	ws.active_world = 0;
	savedata_save(fname);
	item_destroy(item_init("res/item.list"));

	return;
}


void world_loop() {
	int i;
	struct teleport_to t;	/* T för Taliban att Teleportera! */

	if (ws.new_state != ws.state) {
		if (!d_render_fade_status())
			d_render_fade_in(WORLD_FADE_TIME, 0, 0, 0);
		else if (d_render_fade_status() == 2) {
			switch (ws.state) {
				case WORLD_STATE_MAINMENU:
					break;
				case WORLD_STATE_MAPSTATE:
					dungeon_unload();
				case WORLD_STATE_CHANGEMAP:
					/* TODO: Insert music playback */
					dungeon_unload();
					dungeon_load(4);
					t = ws.char_data->teleport.to;
					t.x *= ws.camera.tile_w;
					t.y *= ws.camera.tile_h;
					if (t.slot > 0)
						character_spawn_entry(t.slot, t.ai, t.x, t.y, t.l, 4, -1);
					t.slot = -1;
					ws.new_state = WORLD_STATE_MAPSTATE;
					break;
				default:
					break;
			}

			switch (ws.new_state) {
				case WORLD_STATE_MAINMENU:
					break;
				case WORLD_STATE_MAPSTATE:
					dungeon_load(4);
					/* TODO: Replace with proper code */
					character_spawn_entry(0, "player_ai", 400, 400, 0, 0, -1);
					ws.loading_from_save = 0;
					break;
				case WORLD_STATE_CHANGEMAP:
				default:
					break;
			}

			ws.state = ws.new_state;
			d_render_fade_out(WORLD_FADE_TIME);
		}
	}
			


	switch (ws.state) {
		case WORLD_STATE_MAINMENU:
			/* TODO: Implement */
			break;
		case WORLD_STATE_MAPSTATE:
			character_loop();
			camera_loop();
			textbox_loop();

			d_render_begin();
			for (i = 0; i < ws.dm->grid[4].layers; i++) {
				d_tilemap_camera_move(ws.dm->grid[4].layer[i], ws.camera.x, ws.camera.y);
				d_tilemap_draw(ws.dm->grid[4].layer[i]);
				/* NOTE: This depends on the collision buffer not changing */
				d_render_offset(ws.camera.x, ws.camera.y);
				d_render_blend_enable();
				character_render_layer(character_find_visible(), i);
				d_render_blend_disable();
				d_render_offset(0, 0);
			}
	
			d_render_blend_enable();
			d_render_offset(ws.camera.x, ws.camera.y);
			texteffect_loop();
			d_render_offset(0, 0);
			textbox_draw();
			d_render_blend_disable();

			/* TODO: Render a UI of some sort */
			d_render_end();
		default:
			break;
	}

	return;
}


int world_calc_tile(int x, int y, int l) {
	x /= ws.camera.tile_w;
	y /= ws.camera.tile_h;
	if (x < 0 || y < 0) {
		return -1;
	}

	switch (ws.state) {
		case WORLD_STATE_MAPSTATE:
			if (x >= ws.dm->grid[4].layer[l]->w)
				return -1;
			if (y >= ws.dm->grid[4].layer[l]->h)
				return -1;
			return x + y * ws.dm->grid[4].layer[l]->w;
			break;
		default:
			return 0;
	}

	return 0;
}


unsigned int world_get_tile_i(int i, int l) {
	if (i < 0)
		return ~0;
	switch (ws.state) {
		case WORLD_STATE_MAPSTATE:
			return ws.dm->grid[4].layer[l]->data[i];
			break;
		default:
			return 0;
			break;
	}

	return 0;
}


unsigned int world_get_tile(int x, int y, int l) {
	return world_get_tile_i(world_calc_tile(x, y, l), l);
}


