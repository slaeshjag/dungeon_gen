#include "character_gen.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <darnit/darnit.h>
#include "random.h"
#include "character_save_data.h"

struct generated_char *generate_character() {
	int i, sprite_cnt, dir_cnt, *sprite_data, *sprite_hitbox;
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
	
	/* TODO: Get suitable sprite data for the character generated */
	sprite_data = sprite_data_default;
	sprite_hitbox = sprite_hitbox_default;

	for (dir_cnt = 0, i = 1; sprite_data[i] != -1 || sprite_data[i-1] != -1; i++)
		if ((i & 1) && sprite_data[i - 1] == -1)
				dir_cnt++;
	dir_cnt++;
	sprite_cnt = i + 1;

	gc->face_w = face.w;
	gc->face_h = face.h;
	gc->sprite_w = CHAR_SPRITE_W;
	gc->sprite_h = CHAR_SPRITE_H;
	gc->sprite_dirs = dir_cnt;
	gc->sprite_frames = CHAR_SPRITE_FRAMES;
	gc->char_type.gender = random_get() & 1;
	gc->sprite_data_len = sprite_cnt;
	gc->sprite_data = malloc(sprite_cnt * 4);
	gc->sprite_hitbox = malloc(dir_cnt * 16);
	memcpy(gc->sprite_data, sprite_data, sprite_cnt * 4);
	memcpy(gc->sprite_hitbox, sprite_hitbox, dir_cnt * 16);

	return gc;
}


void generate_char_free(struct generated_char *gc) {
	free(gc->face);
	free(gc->sprite);
	free(gc->sprite_data);
	free(gc->sprite_hitbox);
	free(gc);
	return;
}
