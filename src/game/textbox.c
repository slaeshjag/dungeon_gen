#include "textbox.h"
#include "world.h"


struct textbox *textbox_init(unsigned int w, unsigned int h, int x, int y) {
	struct textbox *tb;

	tb = malloc(sizeof(*tb));

	tb->message = NULL;
	tb->char_pos = 0;
	tb->row = 0;

	/* TODO: Implement */
	tb->option = NULL;
	tb->options = 0;

	tb->face = NULL;
	tb->text = d_text_surface_new(ws.font, 2048, w, x, y);
	tb->rows = h / d_font_glyph_hs(ws.font);

	return tb;
}
