#include "character.h"
#include "world.h"


void character_init(struct world_state *s) {
	unsigned int chars;

	chars = character_gfx_data_characters();
	s->char_data = malloc(sizeof(*s->char_data));
	s->char_data->characters = chars;
	s->char_data->gfx = calloc(sizeof(*s->char_data->gfx) * chars, 1);

	return;
}


int character_load_graphics(struct world_state *s, unsigned int slot) {
	if (slot >= s->char_data->characters)
		return 0;
	if (!s->char_data->gfx[slot])
		s->char_data->gfx[slot] = character_gfx_data_load(slot);

	s->char_data->gfx[slot]->link++;
	return 1;
}

#if 0
char character_spawn
#endif
