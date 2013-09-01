#include <darnit/darnit.h>
#include "camera.h"
#include "world.h"
#include "character.h"

void camera_init() {
	DARNIT_PLATFORM p;

	p = d_platform_get();
	ws.camera.x = ws.camera.y = 0;
	ws.camera.follow_char = -1;

	ws.camera.screen_w = p.screen_w;
	ws.camera.screen_h = p.screen_h;
	ws.camera.player = -1;

	return;
}


void camera_loop() {
	int x, y, w, h;

	if (ws.camera.follow_char < 0)
		return;
	if (ws.camera.follow_char >= ws.char_data->max_entries)
		return;
	if (!ws.char_data->entry[ws.camera.follow_char]) {
		ws.camera.follow_char = -1;
		return;
	}

	d_sprite_hitbox(ws.char_data->entry[ws.camera.follow_char]->sprite, &x, &y, &w, &h);
	x -= (ws.camera.screen_w >> 1);
	y -= (ws.camera.screen_h >> 1);
	x += (w >> 1);
	y += (h >> 1);
	x += (ws.char_data->entry[ws.camera.follow_char]->x >> 8);
	y += (ws.char_data->entry[ws.camera.follow_char]->y >> 8);
	ws.camera.x = x;
	ws.camera.y = y;
	
	return;
}
