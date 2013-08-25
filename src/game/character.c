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

