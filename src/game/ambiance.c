#include "world.h"


void ambiance_init() {
	ws.ambiance.flicker.rect = d_render_rect_new(1);
	d_render_rect_move(ws.ambiance.flicker.rect, 0, -1, -1, ws.camera.screen_w + 1, ws.camera.screen_h + 1);
	ws.ambiance.flicker.r = ws.ambiance.flicker.g = ws.ambiance.flicker.b = 255;
	ws.ambiance.flicker.a = 0;
	ws.ambiance.flicker.filter_active = 0;
	ws.ambiance.flicker.flicker_interval = 0;

	return;
}


void ambiance_draw() {
	int flicker_offset;

	if (ws.ambiance.flicker.flicker_interval) {
		if (ws.ambiance.flicker.flicker) {
			ws.ambiance.flicker.flicker += d_last_frame_time();
			flicker_offset = ws.ambiance.flicker.flicker;
			if (flicker_offset >= ws.ambiance.flicker.flicker_duration / 2)
				flicker_offset = ws.ambiance.flicker.flicker_duration / 2 - flicker_offset;
			flicker_offset *= ws.ambiance.flicker.flicker_offset;
			flicker_offset /= ws.ambiance.flicker.flicker_duration;
			ws.ambiance.flicker.flicker = flicker_offset;
		}
	}
	if (ws.ambiance.flicker.filter_active) {
		d_render_offset(0, 0);
		d_render_tint(ws.ambiance.flicker.r, ws.ambiance.flicker.g, ws.ambiance.flicker.b, ws.ambiance.flicker.a + ws.ambiance.flicker.flicker);
		d_render_rect_draw(ws.ambiance.flicker.rect, 1);
	}

	return;
}
