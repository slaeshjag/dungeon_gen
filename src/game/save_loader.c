#include "save_loader.h"
#include "savedata.h"
#include "common.h"
#include "string.h"
#include <darnit/darnit.h>
#include "character.h"
#include "world.h"


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
	d_util_decompress(buff, scg.zface, (void **) &cg->face);
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

	buff = realloc(buff, scg.zspritedata);
	d_file_read(buff, scg.zspritedata, f);
	d_util_endian_convert((void *) cg->sprite_data, 
		d_util_decompress(buff, scg.zspritedata, (void **) &cg->sprite_data) / 4);
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


void dungeon_init() {
	ws.dm = calloc(sizeof(*ws.dm), 1);

	return;
}


/* ns for neighbour slot */
void dungeon_load(int ns) {
	char name[32], *buf, *data;
	int i, n;
	DARNIT_FILE *f;
	off_t size;
	struct dungeon_map *dm = ws.dm;
	struct savefile_dungeon_header *dh;
	struct savefile_dungeon_object *dob;

	n = ns;
	
	if (n != 4) {
		if (n > 4)
			n -= 1;
		if (ws.dm->grid[4].neighbours[n] == -1)
			return;
	} else
		n = ws.active_world;

	sprintf(name, "world/map_%i.lvl", n);
	if (!(f = d_file_open(name, "rb"))) {
		fprintf(stderr, "Unable to open %s\n", name);
		exit(-1);
	}

	d_file_seek(f, 0, SEEK_END);
	size = d_file_tell(f);
	d_file_seek(f, 0, SEEK_SET);

	buf = malloc(size);
	d_file_read(buf, size, f);
	size = d_util_decompress(buf, size, (void **) &data);
	free(buf);
	buf = data;
	
	dh = (void *) buf;
	buf += sizeof(*dh);
	d_util_endian_convert((void *) dh, INTS(*dh));

	fprintf(stderr, "Loading %i\n", ns);
	memcpy(dm->grid[ns].neighbours, dh->neighbours, 32);
	dm->grid[ns].layers = dh->layers;
	dm->grid[ns].layer = malloc(sizeof(*dm->grid[ns].layer) * dh->layers);
	dm->grid[ns].objects = dh->objects;
	dm->grid[ns].object = malloc(sizeof(*dm->grid[ns].object) * dh->objects);
	sprintf(name, "res/tileset_%i.png", dh->tileset);
	dm->grid[ns].ts = d_render_tilesheet_load(name, TILE_W, TILE_H, DARNIT_PFORMAT_RGB5A1);

	for (i = 0; i < dm->grid[ns].layers; i++) {
		dm->grid[ns].layer[i] = d_tilemap_new(0xFFF, dm->grid[ns].ts, 0xFFF, dh->map_w, dh->map_h);
		d_util_endian_convert((void *) buf, dh->map_w * dh->map_h);
		memcpy(dm->grid[ns].layer[i]->data, buf, dh->map_w * dh->map_h * sizeof(int));
		buf += dh->map_w * dh->map_h * sizeof(int);

		d_tilemap_recalc(dm->grid[ns].layer[i]);
	}

	dob = (void *) buf;
	buf += sizeof(*dob) * dm->grid[ns].objects;
	d_util_endian_convert((void *) dob, INTS(*dob) * dm->grid[ns].objects);
	memcpy(dm->grid[ns].object, dob, sizeof(*dm->grid[ns].object) * dm->grid[ns].objects);

	free(data);
	d_file_close(f);
	character_spawn_map(ns);

	return;
}


void dungeon_unload_slot(int ns) {
	int i;
	struct dungeon_map *dm = ws.dm;

	if (!dm->grid[ns].layer)
		return;

	for (i = 0; i < dm->grid[ns].layers; i++)
		d_tilemap_free(dm->grid[ns].layer[i]);
	free(dm->grid[ns].layer);
	dm->grid[ns].layer = NULL;
	dm->grid[ns].layers = 0;
	free(dm->grid[ns].object);
	dm->grid[ns].object = NULL;
	dm->grid[ns].objects = 0;
	d_render_tilesheet_free(dm->grid[ns].ts);
	character_despawn_map(dm->grid[ns].id);
	dm->grid[ns].id = -1;

	return;
}


void dungeon_unload() {
	int i;

	if (!ws.dm)
		return;
	for (i = 0; i < 9; i++)
		dungeon_unload_slot(i);

	ws.dm = NULL;
	return;
}


void dungeon_do_shift(int op[24]) {
	int i;

	if (*op == -1)
		return;
	for (i = 0; i < 3; i++)
		dungeon_unload_slot(op[i]);
	for (i = 0; i < 6; i++) {
		ws.dm->grid[op[4 + (i << 1)]] = ws.dm->grid[op[3 + (i << 1)]];
		ws.dm->grid[op[3 + (1 << 1)]].layers = 0;
		ws.dm->grid[op[3 + (1 << 1)]].layer = NULL;
	}
	for (i = 0; i < 3; i++)
		dungeon_load(op[23 - i]);
	return;
}


void dungeon_shift_slot(int x, int y) {
	int num[24];

	if (!x && !y)
		return;
	if (y < 0) {
		int t[] = { 6, 7, 8, 3, 6, 4, 7, 5, 8, 0, 3, 1, 4, 2, 5, 0, 1, 2 };
		memcpy(num, t, sizeof(t));
	} else if (!y)
		*num = -1;
	else {
		int t[] = { 0, 1, 2, 3, 0, 4, 1, 5, 2, 6, 3, 7, 4, 8, 5, 6, 7, 8 };
		memcpy(num, t, sizeof(t));
	}

	dungeon_do_shift(num);

	if (x < 0) {
		int t[] = { 2, 5, 8, 1, 2, 4, 5, 7, 8, 0, 1, 3, 4, 6, 7, 0, 3, 6 };
		memcpy(num, t, sizeof(t));
	} else if (!y)
		*num = -1;
	else {
		int t[] = { 0, 3, 6, 1, 0, 4, 3, 7, 6, 2, 1, 5, 4, 8, 7, 2, 5, 8 };
		memcpy(num, t, sizeof(t));
	}

	dungeon_do_shift(num);
}

		


int save_load_deps() {
	DARNIT_FILE *f;
	char cmd[128], arg[128], line[256];

	f = d_file_open("world.info", "rb");
	while (!d_file_eof(f)) {
		d_file_getln(line, 256, f);
		sscanf(line, "%s %s", cmd, arg);
		if (!strcmp(cmd, "require_ai")) {
			sprintf(line, "bin/" PLATFORM ".%s", arg);
			if (!character_load_ai_lib(line)) {
				fprintf(stderr, "Unable to load required lib %s\n", line);
				return 0;
			}
		} else if (!strcmp(cmd, "savedata_i")) {
			ws.savedata.is = atoi(arg);
		} else if (!strcmp(cmd, "savedata_b")) {
			ws.savedata.bs = atoi(arg);
		}
	}

	d_file_close(f);

	return 1;
}


/* Validates a save-file */
enum savefile_status save_load_validate(int save) {
	char ch[64];
	DARNIT_FILE *f;
	
	sprintf(ch, "world_%i.save", save);
	if (!(f = d_file_open(ch, "r+b")))
		return SAVEFILE_STATUS_BLANK;
	d_file_close(f);
	if (!savedata_present(ch))
		return SAVEFILE_STATUS_NOSAVE;
	return SAVEFILE_STATUS_DATAOK;
}
