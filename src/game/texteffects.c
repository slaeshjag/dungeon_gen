#include "world.h"
#include <string.h>


void texteffect_init() {
	ws.te.te = NULL;
	ws.te.max_te = 0;
	ws.te.te_used = 0;
}


void texteffect_done() {
	unsigned i;

	for (i = 0; i < ws.te.max_te; i++)
		if (ws.te.te[i].time_left > 0)
			ws.te.te[i].s = d_text_surface_free(ws.te.te[i].s);
	
	free(ws.te.te);
	ws.te.te = NULL;
	ws.te.max_te = 0;
	ws.te.te_used = 0;

	return;
}


void texteffect_expand() {
	unsigned i;

	i = ws.te.max_te;
	ws.te.max_te += 16;
	ws.te.te = realloc(ws.te.te, sizeof(*ws.te.te) * ws.te.max_te);
	for (; i < ws.te.max_te; i++) {
		ws.te.te[i].time_left = 0;
		ws.te.te[i].s = NULL;
	}

	return;
}


int texteffect_add(const char *str, int time, int x, int y, int linel, unsigned r, unsigned g, unsigned b) {
	unsigned i;
	int j;

	if (ws.te.max_te == ws.te.te_used)
		texteffect_expand();
	for (i = 0; i < ws.te.max_te; i++) {
		if (ws.te.te[i].time_left)
			continue;
		else
			break;
	}

	/* Grr. I want to use strdup. But windows.. */
	y -= d_font_string_geometrics_o(ws.te.font, str, linel, &j);
	ws.te.te[i].s = d_text_surface_color_new(ws.te.font, strlen(str), linel, x - (j >> 1), y);
	ws.te.te[i].time_left = time;
	ws.te.te[i].total_time = time;
	d_text_surface_color_next(ws.te.te[i].s, r, g, b);
	d_text_surface_string_append(ws.te.te[i].s, str);
	ws.te.te_used++;

	return i;
}


void texteffect_del(int id) {
	ws.te.te[id].s = d_text_surface_free(ws.te.te[id].s);
	ws.te.te[id].time_left = 0;

	return;
}


void texteffect_loop() {
	unsigned i, j;
	int k;

	for (i = j = 0; i < ws.te.max_te && j < ws.te.te_used; i++) {
		if (!ws.te.te[i].time_left)
			continue;
		j++;
		k = (ws.te.te[i].time_left << 8) / ws.te.te[i].total_time;
		k = (k < 0) ? 0 : k;
		k = (k > 255) ? 255 : k;
		d_render_offset(ws.camera.x, ws.camera.y - (k >> 2));
		d_render_tint(255, 255, 255, k);
		d_text_surface_draw(ws.te.te[i].s);
		ws.te.te[i].time_left -= d_last_frame_time();
		if (ws.te.te[i].time_left <= 0)
			texteffect_del(i);
	}

	d_render_tint(255, 255, 255, 255);
	
	return;
}

	
