#include "character.h"
#include "world.h"


void character_init() {
	unsigned int chars;

	chars = character_gfx_data_characters();
	ws.char_data = malloc(sizeof(*ws.char_data));
	ws.char_data->characters = chars;
	ws.char_data->gfx = calloc(sizeof(*ws.char_data->gfx) * chars, 1);

	return;
}


int character_load_graphics(unsigned int slot) {
	if (slot >= ws.char_data->characters)
		return 0;
	if (!ws.char_data->gfx[slot])
		ws.char_data->gfx[slot] = character_gfx_data_load(slot);

	ws.char_data->gfx[slot]->link++;
	return 1;
}


struct character_entry *character_spawn_entry(unsigned int slot) {
	int i, j, k;
	struct character_entry *ce;
	struct {
		int		tile;
		int		time;
	} *sprite;

	if (ws.char_data->characters <= slot)
		return NULL;

	sprite = (void *) ws.char_data->gfx[slot]->sprite_data;
	character_load_graphics(slot);
	ce = malloc(sizeof(*ce));
	ce->sprite = d_sprite_new(ws.char_data->gfx[slot]->sprite_ts);

	/* TODO: Init character AI */
	
	for (i = j = k = 0; sprite[i].tile != -1 || sprite[i].time != -1; i++) {
		if (sprite[i].tile >= 0)
			d_sprite_frame_entry(ce->sprite, j, k++, sprite[i].tile, sprite[i].time);
		else
			j++, k = 0;
	}

	return ce;
}


void *character_free_entry(struct character_entry *ce) {
	if (!ce)
		return NULL;
	/* TODO: Add call to character AI for cleanup */
	d_sprite_free(ce->sprite);
	free(ce);

	return NULL;
}
