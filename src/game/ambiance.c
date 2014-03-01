#include "world.h"


void ambiance_init() {
	ws.ambiance.rect = d_render_rect_new(1);
	d_render_rect_move(ws.ambiance.rect, 0, -1, -1, ws.camera.screen_w + 1, ws.camera.screen_h + 1);
	ws.ambiance.r = ws.ambiance.g = ws.ambiance.b = 255;
	ws.ambiance.a = 0;
	ws.ambiance.color_filter_active = 0;

	return;
}


void ambiance_draw() {
	if (ws.ambiance.color_filter_active) {
		d_render_offset(0, 0);
		d_render_tint(ws.ambiance.r, ws.ambiance.g, ws.ambiance.b, ws.ambiance.a);
		d_render_rect_draw(ws.ambiance.rect, 1);
	}

	return;
}
