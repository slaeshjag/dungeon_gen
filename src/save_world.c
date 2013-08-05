#include "save_world.h"
#include "dungeon_generate.h"


int save_world_dungeon(struct dungeon_use *dngu, int index, DARNIT_LDI_WRITER *l) {
	int i;
	unsigned int size;
	union {
		int		*i;
		unsigned int	*u;
	} write_data;

	size = sizeof(struct save_dungeon_header) + sizeof(struct save_dungeon_layer) * dngu->floors;

	for (i = 0; i < dngu->floors; i++)
		size += (dngu->w[i] * dngu->h[i] * sizeof(unsigned int));
	size += dngu->objects * sizeof(struct save_dungeon_object);
	fprintf(stderr, "calculated dungeon size to %i octets\n", size);

	return 1;
}
