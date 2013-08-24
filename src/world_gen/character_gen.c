#include "character_gen.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <darnit/darnit.h>
#include "random.h"

struct generated_char *generate_character() {
	struct generated_char *gc;
	DARNIT_IMAGE_DATA base;
	DARNIT_IMAGE_DATA face;

	gc = calloc(sizeof(*gc), 1);

	/* This will be replaced with something that actually generates a face */
	face = d_img_load_raw("char_gen/f_base0_0.png");
	gc->face = face.data;

	/* This will be replaced with something that actually generates a sprite */
	base = d_img_load_raw("char_gen/s_base0_0.png");
	gc->sprite = base.data;

	gc->face_w = face.w;
	gc->face_h = face.h;
	gc->sprite_w = CHAR_SPRITE_W;
	gc->sprite_h = CHAR_SPRITE_H;
	gc->sprite_dirs = CHAR_SPRITE_DIRECTIONS;
	gc->sprite_frames = CHAR_SPRITE_FRAMES;
	gc->char_type.gender = random_get() & 1;

	return gc;
}


void generate_char_free(struct generated_char *gc) {
	free(gc->face);
	free(gc->sprite);
	free(gc);
	return;
}
