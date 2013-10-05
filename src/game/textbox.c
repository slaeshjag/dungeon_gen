#define	_EMIT_PALETTE
#include "textbox.h"
#include "world.h"
#include <string.h>


void textbox_init(int w, int h, int x, int y, int pad_x, int pad_y, int pad_x2, int pad_y2) {
	struct textbox *tb;
	DARNIT_FILE *f;
	int i, wt, ht;

	tb = malloc(sizeof(*tb));

	tb->message = NULL;
	tb->char_pos = 0;
	tb->row = 0;

	/* TODO: Implement */
	tb->option = NULL;
	tb->options = 0;

	tb->face = NULL;
	w -= (w % ws.camera.tile_w);
	x += (w % ws.camera.tile_w) / 2;
	h -= (h % ws.camera.tile_h);
	y += (h % ws.camera.tile_h);

	tb->surface_w = (w - pad_x - pad_x2);
	tb->text = d_text_surface_color_new(ws.font, 2048, tb->surface_w, x + pad_x, y + pad_y);
	tb->rows = (h - pad_y - pad_y2) / d_font_glyph_hs(ws.font);
	tb->tc = d_tilemap_new(0xFF, ws.camera.sys, 0xFFF, w / ws.camera.tile_w, h / ws.camera.tile_h);
	d_tilemap_camera_move(tb->tc, -x, -y);

	tb->ms_per_char = 200;
	tb->dt = 0;
	if ((f = d_file_open("res/PALETTE.VGA", "rb"))) {
		d_file_read(textbox_color_palette, 1024, f);
		d_file_close(f);
		for (i = 3; i < 1024; textbox_color_palette[i] = 0xFF, i += 4);
	}

	/* Generate textbox background */
	wt = w / ws.camera.tile_w;
	ht = h / ws.camera.tile_h;

	for (i = 1; i < wt * ht; i++)
		tb->tc->data[i] = 5;
	for (i = 1; i < wt - 1; i++)
		tb->tc->data[i] = 2;
	for (i = wt * (ht - 1); i < wt * ht; i++)
		tb->tc->data[i] = 8;
	for (i = 0; i < wt * ht; i += wt)
		tb->tc->data[i] = 4;
	for (i = wt - 1; i < wt * ht; i += wt)
		tb->tc->data[i] = 6;
	tb->tc->data[0] = 1;
	tb->tc->data[wt - 1] = 3;
	tb->tc->data[wt * (ht - 1)] = 7;
	/* TODO: Implement pagination mark */
	tb->tc->data[wt * ht - 1] = 9;

	d_tilemap_recalc(tb->tc);

	ws.textbox = tb;

	return;
}


void textbox_loop() {
	struct textbox *tb;
	unsigned char *p = textbox_color_palette;
	int i;

	/* TODO: Init */
	tb = ws.textbox;
	if (!tb->message)
		return;
	
	/* Improve or somth.. */
	tb->dt += d_last_frame_time();
	for (; tb->dt > tb->ms_per_char && tb->message[tb->char_pos]; tb->dt -= tb->ms_per_char) {
		if (tb->message[tb->char_pos]) {
			if (tb->message[tb->char_pos] == ' ') {
				tb->char_pos++;
				if (d_text_surface_pos(tb->text) + d_font_word_w(ws.font, 
				    &tb->message[tb->char_pos], NULL) >= tb->surface_w)
					d_text_surface_char_append(tb->text, "\n");
				else
					d_text_surface_char_append(tb->text, " ");
			} else if (tb->message[tb->char_pos] == '\x01') {
				tb->char_pos++;
				i = (((unsigned) tb->message[tb->char_pos]) << 2);
				d_text_surface_color_next(tb->text, p[i], p[i+1], p[i+2]);
				tb->char_pos++;
				tb->dt += tb->ms_per_char;
			} else if (tb->message[tb->char_pos] == '\x02') {
				tb->char_pos++;
				i = ((unsigned char) tb->message[tb->char_pos]);
				tb->dt += tb->ms_per_char;
				tb->ms_per_char = i << 2;
				tb->char_pos++;
			} else
				tb->char_pos += d_text_surface_char_append(tb->text, 
					&tb->message[tb->char_pos]);
		} else
			break;
	}

	return;
}


void textbox_add_message(const char *message) {
	struct textbox *tb = ws.textbox;
	if (tb)
		free(tb->message), tb->message = NULL;
	tb->char_pos = 0;
	tb->dt = 0;
	
	tb->message = malloc(strlen(message) + 1);
	strcpy(tb->message, message);

	return;
}


void textbox_draw() {
	struct textbox *tb = ws.textbox;
	
	if (!tb->message)
		return;
	d_tilemap_draw(tb->tc);
	d_text_surface_draw(tb->text);

	return;
}


void textbox_destroy() {
	struct textbox *tb = ws.textbox;
	
	free(tb->message);
	d_text_surface_free(tb->text);
	free(tb);
	ws.textbox = NULL;

	return;
}

