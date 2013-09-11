#include <darnit/darnit.h>
#include <math.h>

#include "camera.h"
#include "world.h"
#include "save_loader.h"
#include "character.h"

void camera_init() {
	DARNIT_PLATFORM p;

	p = d_platform_get();
	ws.camera.x = ws.camera.y = 0;
	ws.camera.follow_char = -1;

	ws.camera.screen_w = p.screen_w;
	ws.camera.screen_h = p.screen_h;
	ws.camera.player = -1;
	ws.camera.tile_w = TILE_W;
	ws.camera.tile_h = TILE_H;
	ws.camera.scroll_speed = 400;		/* 400 pixels / second */
	ws.camera.jump = 0;

	return;
}


void camera_loop() {
	int x, y, w, h, dx, dy, s2, t, f;

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
	dx = x - ws.camera.x;
	dy = y - ws.camera.y;

	t = ws.camera.scroll_speed * d_last_frame_time();
	t /= 1000;
	s2 = t * t;
	if (dx * dx + dy * dy > s2 && !ws.camera.jump) {
		f = ((int) sqrtf(dx * dx + dy * dy)) << 8;
		t = f / (sqrtf(s2));
		dx = (dx << 8) / t;
		dy = (dy << 8) / t;
		ws.camera.x += dx;
		ws.camera.y += dy;
	} else {
		ws.camera.jump = 0;
		ws.camera.x = x;
		ws.camera.y = y;
	}
	
	return;
}
