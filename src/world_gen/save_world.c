#include "save_world.h"
#include "dungeon_generate.h"
#include <string.h>


int save_world_dungeon(struct dungeon_use *dngu, int index, DARNIT_LDI_WRITER *lw, int tileset) {
	int i;
	unsigned int size;
	struct savefile_dungeon_header h;
	struct savefile_dungeon_floor l;
	struct savefile_dungeon_object o;
	char *data, *next, name[32];

	size = sizeof(h) + sizeof(l) * dngu->floors;
	size += (sizeof(struct dungeon_puzzle_part) * dngu->puzzles);
	
	for (i = 0; i < dngu->floors; i++)
		size += (dngu->w[i] * dngu->h[i] * sizeof(unsigned int));
	size += dngu->objects * sizeof(o);
	sprintf(name, "world/dungeon_%i.lvl", index);
	fprintf(stderr, "calculated dungeon size to %i octets\n", size);
	data = next = malloc(size);

	h.floors = dngu->floors;
	h.objects = dngu->objects;
	h.puzzles = dngu->puzzles;
	h.tileset = tileset;
	h.entrance = dngu->entrance;
	h.entrance_floor = dngu->entrance_floor;

	d_util_endian_convert((void *) &h, sizeof(h) / sizeof(unsigned int));
	memcpy(next, &h, sizeof(h));
	next += sizeof(h);

	for (i = 0; i < dngu->floors; i++) {
		l.stair_up = dngu->floor_info[i].stair_up;
		l.stair_down = dngu->floor_info[i].stair_down;
		l.floor_w = dngu->w[i];
		l.floor_h = dngu->h[i];
		d_util_endian_convert((void *) &l, sizeof(l) / sizeof(unsigned int));
		memcpy(next, &l, sizeof(l));
		next += sizeof(l);
		d_util_endian_convert((void *) dngu->tile_data[i], dngu->w[i] * dngu->h[i]);
		memcpy(next, dngu->tile_data[i], dngu->w[i] * dngu->h[i] * 4);
		d_util_endian_convert((void *) dngu->tile_data[i], dngu->w[i] * dngu->h[i]);
		next += (dngu->w[i] * dngu->h[i] * 4);
	}

	for (i = 0; i < dngu->objects; i++) {
		o.x = dngu->object[i].x;
		o.y = dngu->object[i].y;
		o.f = dngu->object[i].l;
		o.type = dngu->object[i].type;
		o.subtype = dngu->object[i].subtype;
		o.link = dngu->object[i].link;
		o.save_slot = dngu->object[i].saveslot;

		d_util_endian_convert((void *) &o, sizeof(o) / sizeof(unsigned int));
		memcpy(next, &o, sizeof(o));
		next += sizeof(o);
	}

	d_util_endian_convert((void *) dngu->puzzle, sizeof(*(dngu->puzzle)) / sizeof(unsigned int) * dngu->puzzles);
	memcpy(next, dngu->puzzle, sizeof(*(dngu->puzzle)) * dngu->puzzles);
	next += sizeof(*(dngu->puzzle)) * dngu->puzzles;
	d_util_endian_convert((void *) dngu->puzzle, sizeof(*(dngu->puzzle)) / sizeof(unsigned int) * dngu->puzzles);
	
	size = d_util_compress(data, size, &next);
	d_file_ldi_write_file(lw, name, next, size);
	free(data);
	free(next);

	return 1;
}
