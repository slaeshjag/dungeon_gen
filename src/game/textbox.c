#define	_EMIT_PALETTE
#include "textbox.h"
#include "world.h"
#include "aicomm.h"
#include "character.h"
#include <string.h>
#include <limits.h>


void textbox_init_background(DARNIT_TILEMAP *tc, int w, int h) {
	int wt, ht, i;

	/* Generate textbox background */
	wt = w / ws.camera.tile_w;
	ht = h / ws.camera.tile_h;

	for (i = 1; i < wt * ht; i++)
		tc->data[i] = 5;
	for (i = 1; i < wt - 1; i++)
		tc->data[i] = 2;
	for (i = wt * (ht - 1); i < wt * ht; i++)
		tc->data[i] = 8;
	for (i = 0; i < wt * ht; i += wt)
		tc->data[i] = 4;
	for (i = wt - 1; i < wt * ht; i += wt)
		tc->data[i] = 6;
	tc->data[0] = 1;
	tc->data[wt - 1] = 3;
	tc->data[wt * (ht - 1)] = 7;
	tc->data[wt * ht - 1] = 9;

	d_tilemap_recalc(tc);
}


void textbox_init(int w, int h, int x, int y, int pad_x, int pad_y, int pad_x2, int pad_y2) {
	struct textbox *tb;
	DARNIT_FILE *f;
	int i;

	tb = malloc(sizeof(*tb));

	tb->message = NULL;
	tb->char_pos = 0;
	tb->row = 0;

	/* TODO: Implement */
	tb->option = NULL;
	tb->options = 0;

	tb->face = d_render_tile_new(1, NULL);
	tb->face_ts = NULL;
	w -= (w % ws.camera.tile_w);
	x += (w % ws.camera.tile_w) / 2;
	h -= (h % ws.camera.tile_h);
	y += (h % ws.camera.tile_h);

	tb->surface_w = (w - pad_x - pad_x2);
	tb->text = d_text_surface_color_new(ws.font, 2048, tb->surface_w, x + pad_x, y + pad_y);
	tb->rows = (h - pad_y - pad_y2) / d_font_glyph_hs(ws.font);
	tb->tc = d_tilemap_new(0xFF, ws.camera.sys, 0xFFF, w / ws.camera.tile_w, h / ws.camera.tile_h);
	d_tilemap_camera_move(tb->tc, -x, -y);
	tb->w = w, tb->h = h, tb->x = x, tb->y = y;

	tb->ms_per_char = DEFAULT_TEXT_SPEED;
	tb->dt = 0;
	if ((f = d_file_open("res/PALETTE.VGA", "rb"))) {
		d_file_read(textbox_color_palette, 1024, f);
		d_file_close(f);
		for (i = 3; i < 1024; textbox_color_palette[i] = 0xFF, i += 4);
	}

	textbox_init_background(tb->tc, w, h);
	ws.textbox = tb;

	return;
}


void textbox_update_pointers(struct textbox *tb) {
	int y;

	y = d_font_glyph_hs(ws.font);
	y *= tb->selection;
	y += tb->y_selection;
	y -= ws.camera.tile_h / 2;
	y += (d_font_glyph_hs(ws.font) / 2);

	d_render_tile_move(tb->pointer, 0, ws.camera.screen_w - ws.camera.tile_w  * tb->qt->w, y);
	d_render_tile_move(tb->pointer, 1, ws.camera.screen_w - ws.camera.tile_w, y);

	return;
}


void textbox_loop() {
	struct textbox *tb;
	unsigned char *p = textbox_color_palette;
	struct aicomm_struct ac;
	int i, next;
	DARNIT_KEYS k;

	/* TODO: Init */
	tb = ws.textbox;
	next = 0;
	if (!tb->message)
		return;
	
	/* Improve or somth.. */
	tb->dt += d_last_frame_time();
	for (; tb->dt > tb->ms_per_char && tb->message[tb->char_pos]; tb->dt -= tb->ms_per_char) {
		if (d_keys_get().BUTTON_ACCEPT) {
			tb->dt = INT_MAX;
			next = 1;
			k = d_keys_zero();
			k.BUTTON_ACCEPT = 1;
			d_keys_set(k);
		}

		if (tb->row == tb->rows) {
			if (tb->tc->data[tb->tc->w * tb->tc->h - 1] != 10) {
				tb->tc->data[tb->tc->w * tb->tc->h - 1] = 10;
				d_tilemap_recalc(tb->tc);
			}
			
			tb->dt = 0;
			break;
		}
			
		if (tb->message[tb->char_pos]) {
			if (tb->message[tb->char_pos] == ' ') {
				tb->char_pos++;
				d_text_surface_char_append(tb->text, " ");
				if (d_text_surface_pos(tb->text) + d_font_word_w(ws.font, 
				    &tb->message[tb->char_pos], NULL) >= tb->current_surface_w) {
					d_text_surface_char_append(tb->text, "\n");
					d_text_surface_offset_next_set(tb->text, tb->pad_start);
					tb->row++;
				}
			} else if (tb->message[tb->char_pos] == '\n') {
				tb->char_pos++;
				d_text_surface_char_append(tb->text, "\n");
				d_text_surface_offset_next_set(tb->text, tb->pad_start);
				tb->row++;
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
			} else if (tb->message[tb->char_pos] == '\x03') {
				tb->char_pos++;
				tb->dt += tb->ms_per_char;
				tb->ms_per_char = DEFAULT_TEXT_SPEED;
			} else
				tb->char_pos += d_text_surface_char_append(tb->text, 
					&tb->message[tb->char_pos]);
		} else
			break;
	}

	if (d_keys_get().down) {
		tb->selection++;
		if (tb->selection >= (signed) tb->options)
			tb->selection = 0;
		k = d_keys_zero();
		k.down = 1;
		textbox_update_pointers(tb);
		d_keys_set(k);
	}

	if (d_keys_get().up) {
		tb->selection--;
		if (tb->selection < 0)
			tb->selection = tb->options - 1;
		k = d_keys_zero();
		k.up = 1;
		textbox_update_pointers(tb);
		d_keys_set(k);
	}

	if (!tb->message[tb->char_pos]) {
		if (d_keys_get().BUTTON_ACCEPT || next) {
			k = d_keys_zero();
			k.BUTTON_ACCEPT = 1;
			d_keys_set(k);
			
			/* Close textbox */
			free(tb->message), tb->message = NULL;
			d_render_tilesheet_free(tb->face_ts);
			d_tilemap_free(tb->qt);
			d_text_surface_free(tb->qts);
			d_render_tile_free(tb->pointer);
			tb->face_ts = NULL;
			free(tb->option), tb->option = NULL;

			ac.msg = AICOMM_MSG_BOXR;
			ac.from = -1;
			ac.arg[0] = tb->selection;
			ac.self = tb->char_pingback;
			character_message_loop(ac);

			ac.msg = AICOMM_MSG_SILE;
			ac.arg[0] = 0;
			character_tell_all(ac);
			return;
		}
	}

	if (next) {
		tb->tc->data[tb->tc->w * tb->tc->h - 1] = 9;
		d_tilemap_recalc(tb->tc);
		d_text_surface_reset(tb->text);
		d_text_surface_offset_next_set(tb->text, tb->pad_start);
		tb->row = 0;
		next = 0;
	}

	return;
}


void textbox_add_message(const char *message, const char *question, int face, int pingback) {
	struct textbox *tb = ws.textbox;
	struct char_gfx *cg;
	struct aicomm_struct ac;
	int blol, w, h, wt, ht, x, y, i;
	const void *bluh;

	if (tb)
		free(tb->message), tb->message = NULL;
	tb->char_pos = 0;
	tb->row = 0;
	tb->dt = 0;
	tb->char_pingback = pingback;
	if (tb->face_ts) {
		d_render_tilesheet_free(tb->face_ts), tb->face_ts = NULL;
		character_unload_graphics(tb->face_id);
	}
	if (face >= 0) {
		character_load_graphics(face);
		tb->face_id = face;
		cg = ws.char_data->gfx[face];
		tb->face_ts = d_render_tilesheet_new(1, 1, cg->face_w, cg->face_h, 
			DARNIT_PFORMAT_RGB5A1);
		d_render_tilesheet_update(tb->face_ts, 0, 0, cg->face_w, cg->face_h, cg->face);
		d_render_tile_tilesheet(tb->face, tb->face_ts);
		d_render_tile_move(tb->face, 0, tb->x + 8, (tb->h - cg->face_h) / 2 + tb->y);
		d_render_tile_set(tb->face, 0, 0);
		blol = cg->face_w + 16;
	} else
		blol = 0;

	tb->current_surface_w = tb->surface_w - blol;
	tb->pad_start = blol;
	d_text_surface_reset(tb->text);

	tb->message = malloc(strlen(message) + 1);
	strcpy(tb->message, message);
	if (question) {
		tb->option = malloc(strlen(question) + 1);
		strcpy(tb->option, question);
	} else
		tb->option = NULL, tb->options = 0;
	
	d_text_surface_offset_next_set(tb->text, tb->pad_start);

	ac.from = -1;
	ac.msg = AICOMM_MSG_SILE;
	ac.arg[0] = 1;
	character_tell_all(ac);

	/* Figure out question box */

	if (!question)
		question = "";
	w = 0;
	h = d_font_string_geometrics_o(ws.font, question, ws.camera.screen_w / 2, &w);
	w += ws.camera.tile_w * 2 + 5;
	h += 16;
	wt = w / ws.camera.tile_w;
	if (w % ws.camera.tile_w)
		wt++;
	ht = h / ws.camera.tile_h;
	if (h % ws.camera.tile_h)
		ht++;

	tb->pointer = d_render_tile_new(2, ws.camera.sys);
	d_render_tile_set(tb->pointer, 0, 12);
	d_render_tile_set(tb->pointer, 1, 13);

	tb->qt = d_tilemap_new(0xFFF, ws.camera.sys, 0xFFF, wt, ht);
	textbox_init_background(tb->qt, wt * ws.camera.tile_w, ht * ws.camera.tile_h);
	d_tilemap_camera_move(tb->qt, -(ws.camera.screen_w - wt * ws.camera.tile_w), -(ws.camera.screen_h - (ht + tb->tc->h) * ws.camera.tile_h));

	x = y = 0;
	x = ws.camera.screen_w - ws.camera.tile_w * wt;
	x += (wt * ws.camera.tile_w - w) / 2 + ws.camera.tile_w;
	y = ws.camera.screen_h - ht * ws.camera.tile_h;
	y -= (tb->tc->h * ws.camera.tile_h);
	y += (ws.camera.tile_h * ht - h) / 2;
	tb->qts = d_text_surface_new(ws.font, strlen(question), w - 2 * ws.camera.tile_w, x, y);
	d_text_surface_string_append(tb->qts, question);
	bluh = question;
	if (!strlen(question))
		i = 0;
	else
		for (i = 0; bluh; i++)
			bluh = strchr(bluh + 1, '\n');
	tb->options = i;
	tb->y_selection = y;
	tb->selection = 0;
	textbox_update_pointers(tb);

	return;
}


void textbox_draw() {
	struct textbox *tb = ws.textbox;
	
	if (!tb->message)
		return;
	d_tilemap_draw(tb->tc);
	if (tb->option)
		d_tilemap_draw(tb->qt);
	
	d_text_surface_draw(tb->text);
	if (tb->option)
		d_text_surface_draw(tb->qts);
	d_render_tile_draw(tb->face, 1);

	if (tb->option) {
		d_render_tile_draw(tb->pointer, 2);
	}


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

