#include "save_loader.h"
#include "common.h"
#include "string.h"
#include "darnit/darnit.h"


int character_gfx_data_characters() {
	DARNIT_FILE *f;
	unsigned int chars;

	f = d_file_open("data/characters.dat", "rb");
	d_file_read_ints(&chars, 1, f);
	d_file_close(f);

	return chars;
}


struct char_gfx *character_gfx_data_load(unsigned int char_num) {
	DARNIT_FILE *f;
	unsigned int chars, pos, char_w, char_h, i, j, w, max_w, r, *ptr, *bptr;
	struct char_gfx *cg;
	struct savefile_character_gfx scg;
	void *buff, *sprite_b;

	f = d_file_open("data/characters.dat", "rb");
	d_file_read_ints(&chars, 1, f);
	if (char_num >= chars) {
		d_file_close(f);
		return NULL;
	}

	d_file_seek(f, char_num * 4, SEEK_CUR);
	d_file_read_ints(&pos, 1, f);
	d_file_seek(f, pos, SEEK_SET);

	cg = malloc(sizeof(*cg));
	d_file_read_ints(&scg, sizeof(scg) / 4, f);
	cg->face_w = scg.face_w;
	cg->face_h = scg.face_h;
	cg->sprite_w = scg.sprite_w;
	cg->sprite_h = scg.sprite_h;
	cg->frames = scg.sprite_frames;
	cg->directions = scg.directions;

	buff = malloc(scg.zface);
	d_file_read(buff, scg.zface, f);
	d_util_decompress(buff, scg.zface, &cg->face);
	d_util_endian_convert(cg->face, cg->face_w * cg->face_h);
	buff = realloc(buff, scg.zsprite);
	d_file_read(buff, scg.zsprite, f);
	d_util_decompress(buff, scg.zsprite, &sprite_b);
	d_util_endian_convert(sprite_b, scg.sprite_w * scg.sprite_h * scg.sprite_frames);

	char_h = scg.sprite_h;
	char_w = scg.sprite_w * scg.sprite_frames;
	max_w = 1024 / scg.sprite_w;
	max_w *= scg.sprite_w;
	ptr = sprite_b;

	if (char_w > max_w) {
		char_h *= (char_w / max_w);
		if (char_w % max_w)
			char_h += scg.sprite_h;
		char_w = max_w;
	}
	buff = realloc(buff, char_w * char_h * 4);
	bptr = buff;
	memset(buff, 0, char_w * char_h * 4);

	/* Split up into several rows of frames */
	if (char_h != scg.sprite_h) {
		w = scg.sprite_w * scg.sprite_frames;
		for (i = 0; i < (char_h / scg.sprite_h); i++) {
			r = w - (max_w * i);
			r = (r > max_w) ? max_w : r;
			for (j = 0; j < scg.sprite_h; j++)
				memcpy(bptr + j * max_w + i * max_w * char_h,
					ptr + max_w * i + w * j, r * 4);
		}
	} else
		memcpy(bptr, ptr, char_w * char_h * 4);

	cg->sprite_ts = d_render_tilesheet_new(char_w / scg.sprite_w, char_h / scg.sprite_h, 
		scg.sprite_w, scg.sprite_h, DARNIT_PFORMAT_RGB5A1);
	d_render_tilesheet_update(cg->sprite_ts, 0, 0, char_w, char_h, bptr);
	fprintf(stderr, "Updating area of size %i, %i\n", char_w, char_h);

	buff = realloc(buff, scg.zspritedata);
	d_file_read(buff, scg.zspritedata, f);
	d_util_endian_convert((void *) cg->sprite_data, 
		d_util_decompress(buff, scg.zspritedata, &cg->sprite_data) / 4);
	cg->sprite_hitbox = malloc(cg->directions * 16);
	d_file_read_ints(cg->sprite_hitbox, cg->directions * 4, f);
	free(buff);
	free(sprite_b);
	d_file_close(f);

	cg->link = 0;
	return cg;
}


void *character_gfx_data_unload(struct char_gfx *cg) {
	free(cg->face);
	free(cg->sprite_data);
	free(cg->sprite_hitbox);
	d_render_tilesheet_free(cg->sprite_ts);
	free(cg);
	return NULL;
}


struct dungeon_map *dungeon_load(int dungeon_number) {
	char name[32], *buf, *data;
	int i;
	DARNIT_FILE *f;
	off_t size;
	struct dungeon_map *dm;
	struct savefile_dungeon_header *dh;
	struct savefile_dungeon_floor *df;
	struct savefile_dungeon_object *dob;
	struct savefile_dungeon_puzzle_part *dp;

	sprintf(name, "world/dungeon_%i.lvl", dungeon_number);
	if (!(f = d_file_open(name, "rb"))) {
		fprintf(stderr, "Unable to open %s\n", name);
		exit(-1);
	}

	dm = malloc(sizeof(*dm));
	d_file_seek(f, 0, SEEK_END);
	size = d_file_tell(f);
	d_file_seek(f, 0, SEEK_SET);
	buf = malloc(size);
	d_file_read(buf, size, f);
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
		d_util_endian_convert((void *) buf, df->floor_w * df->floor_h);
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
	d_file_close(f);

	return dm;
}


void *dungeon_unload(struct dungeon_map *dm) {
	int i;

	if (!dm)
		return NULL;
	for (i = 0; i < dm->floors; i++)
		d_tilemap_free(dm->floor[i].tm);
	free(dm->floor);
	free(dm->puzzle);
	free(dm->object);
	d_render_tilesheet_free(dm->ts);

	return NULL;
}
