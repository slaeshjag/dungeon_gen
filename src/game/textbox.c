#define	_EMIT_PALETTE
#include "textbox.h"
#include "world.h"


void textbox_init(unsigned int w, unsigned int h, int x, int y) {
	struct textbox *tb;

	tb = malloc(sizeof(*tb));

	tb->message = NULL;
	tb->char_pos = 0;
	tb->row = 0;

	/* TODO: Implement */
	tb->option = NULL;
	tb->options = 0;

	tb->face = NULL;
	tb->text = d_text_surface_color_new(ws.font, 2048, w, x, y);
	tb->rows = h / d_font_glyph_hs(ws.font);
	tb->surface_w = w;

	tb->ms_per_char = 200;
	tb->dt = 0;

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
			} else if (tb->message[tb->char_pos] == '\x01') {
				tb->char_pos++;
				i = (tb->char_pos << 2);
				d_text_surface_color_next(tb->text, p[i], p[i+1], p[i+2]);
				tb->char_pos++;
			} else if (tb->message[tb->char_pos] == '\x02') {
				tb->char_pos++;
				i = ((unsigned) tb->message[tb->char_pos]);
				i <<= 2;
				tb->ms_per_char = i;
			} else
				tb->char_pos += d_text_surface_char_append(tb->text, 
					&tb->message[tb->char_pos]);
		}
	}

	return;
}


void textbox_draw() {
	if (!ws.textbox->message)
		return;
	d_text_surface_draw(ws.textbox->text);

	return;
}
