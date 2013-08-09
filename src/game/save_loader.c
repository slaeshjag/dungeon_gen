#include "save_loader.h"
#include "common.h"
#include "string.h"


void dungeon_load(int save_slot) {
	char name[32], *buf, *data;
	int i;
	DARNIT_FILE *f;
	off_t size;
	struct dungeon_map *dm;
	struct savefile_dungeon_header *dh;
	struct savefile_dungeon_floor *df;
	struct savefile_dungeon_object *dob;
	struct savefile_dungeon_puzzle_part *dp;

	sprintf(name, "world/dungeon_%i.lvl", save_slot);
	if (!(f = d_file_open(name, "rb"))) {
		fprintf(stderr, "Unable to open %s\n", name);
		exit(-1);
	}

	dm = malloc(sizeof(*dm));
	d_file_seek(f, 0, SEEK_END);
	size = d_file_tell(f);
	d_file_seek(f, 0, SEEK_SET);
	buf = malloc(size);
	size = d_util_decompress(buf, size, &data);
	free(buf);
	buf = data;
	
	dh = (void *) buf;
	buf += sizeof(*dh);
	d_util_endian_convert((void *) dh, INTS(*dh));
	
	dm->floors = dh->floors;
	dm->floor = malloc(sizeof(*dm->floor) * dh->floors);
	dm->objects = dh->objects;
	dm->object = malloc(sizeof(*dm->object) * dh->objects);
	dm->puzzles = dh->puzzles;
	dm->puzzle = malloc(sizeof(*dm->puzzle) * dh->puzzles);
	dm->entrance_floor = dh->entrance_floor;
	dm->entrance = dh->entrance;
	sprintf(name, "res/tileset_%i.png", dh->tileset);
	dm->ts = d_render_tilesheet_load(name, TILE_W, TILE_H, DARNIT_PFORMAT_RGB5A1);

	for (i = 0; i < dm->floors; i++) {
		df = (void *) buf;
		buf += sizeof(*df);
		d_util_endian_convert((void *) df, INTS(*df));
		dm->floor[i].stair_up = df->stair_up;
		dm->floor[i].stair_down = df->stair_down;

		dm->floor[i].tm = d_tilemap_new(0xFFF, dm->ts, 0xFFF, df->floor_w, df->floor_h);
		memcpy(dm->floor[i].tm->data, buf, df->floor_w * df->floor_h * sizeof(int));
		buf += df->floor_w * df->floor_h * sizeof(int);
		d_tilemap_recalc(dm->floor[i].tm);
	}

	dob = (void *) buf;
	buf += sizeof(*dob) * dm->objects;
	d_util_endian_convert((void *) dob, INTS(*dob) * dm->objects);
	memcpy(dm->object, dob, sizeof(*dm->object) * dm->objects);

	dp = (void *) buf;
	buf += (sizeof(*dp) * dm->puzzles);
	d_util_endian_convert((void *) dp, INTS(*dp) * dm->puzzles);
	memcpy(dm->puzzle, dp, sizeof(*dm->puzzle) * dm->puzzles);

	free(data);

	return;
}

