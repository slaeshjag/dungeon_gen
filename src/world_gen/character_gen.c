#include "character_gen.h"
#include <stdlib.h>
#include <stdio.h>
#include "random.h"

struct generated_char *generate_character() {
	struct generated_char *gc;

	gc = calloc(sizeof(*gc), 1);
	gc->face = calloc(sizeof(*gc->face) * CHAR_FACE_W * CHAR_FACE_H, 1);
	gc->sprite = calloc(sizeof(*gc->sprite) * CHAR_SPRITE_W 
		* CHAR_SPRITE_H * CHAR_SPRITE_FRAMES, 1);
	gc->face_w = CHAR_FACE_W;
	gc->face_h = CHAR_FACE_H;
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
