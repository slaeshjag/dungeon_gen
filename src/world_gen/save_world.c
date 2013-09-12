#include "save_world.h"
#include "dungeon_generate.h"
#include "dungeon_object.h"
#include "character_gen.h"
#include "savefile.h"
#include "dynchar.h"
#include <string.h>
#include <stdlib.h>
#include <darnit/darnit.h>


void save_map_info(DARNIT_LDI_WRITER *lw) {
	char buff[128];
	struct dynchar *dc;

	dc = dynchar_new();

	dynchar_append(dc, "require_ai ai\n");
	sprintf(buff, "save_version %i\n", SAVEFILE_VERSION);
	dynchar_append(dc, buff);

	d_file_ldi_write_file(lw, "world.info", dc->buff, dc->size - 1);
	dynchar_free(dc);

	return;
}


int save_characters(struct generated_char **gc, int characters, DARNIT_LDI_WRITER *lw) {
	char *data, *zdata, *zdata2, *zdata3;
	int i, next;
	unsigned int *iptr;
	struct savefile_character_gfx scg;

	
	memset(&scg, 0, sizeof(scg));
	data = malloc(sizeof(*iptr) + characters * 4);
	iptr = (void *) data;
	next = sizeof(*iptr);
	next += characters * sizeof(*iptr);
	*iptr = characters;
	d_util_endian_convert(iptr, 1);
	iptr++;
	
	for (i = 0; i < characters; i++) {
		scg.face_w = gc[i]->face_w;
		scg.face_h = gc[i]->face_h;
		scg.sprite_w = gc[i]->sprite_w;
		scg.sprite_h = gc[i]->sprite_h;
		scg.directions = gc[i]->sprite_dirs;
		scg.sprite_frames = gc[i]->sprite_frames;
		scg.type = gc[i]->char_type;

		d_util_endian_convert((void *) gc[i]->face, scg.face_w * scg.face_h);
		scg.zface = d_util_compress(gc[i]->face, scg.face_w * scg.face_h * 4, &zdata);
		
		d_util_endian_convert((void *) gc[i]->sprite, 
			scg.sprite_w * scg.sprite_h * scg.sprite_frames);
		scg.zsprite = d_util_compress(gc[i]->sprite, 
			scg.sprite_w * scg.sprite_h * 4 * scg.sprite_frames, &zdata2);
		
		d_util_endian_convert((void *) gc[i]->sprite_data, gc[i]->sprite_data_len);
		scg.zspritedata = d_util_compress(gc[i]->sprite_data, gc[i]->sprite_data_len * 4,
			&zdata3);
		data = realloc(data, next + sizeof(scg) + scg.zface + scg.zsprite + scg.zspritedata
			+ gc[i]->sprite_dirs * 16);
		d_util_endian_convert((void *) &scg, sizeof(scg) / 4);
		memcpy(data + next, &scg, sizeof(scg));
		d_util_endian_convert((void *) &scg, sizeof(scg) / 4);
		d_util_endian_convert((void *) gc[i]->sprite_hitbox, 4 * gc[i]->sprite_dirs);
		next += sizeof(scg);
		memcpy(data + next, zdata, scg.zface); 
		next += scg.zface;
		memcpy(data + next, zdata2, scg.zsprite); 
		next += scg.zsprite;
		memcpy(data + next, zdata3, scg.zspritedata);
		next += scg.zspritedata;
		memcpy(data + next, gc[i]->sprite_hitbox, gc[i]->sprite_dirs * 16);
		next += gc[i]->sprite_dirs * 16;
		free(zdata);
		free(zdata2);
		free(zdata3);
		iptr = (unsigned int *) (data + 4);
		iptr[i] = next;
	}

	d_util_endian_convert(iptr, characters);
	d_file_ldi_write_file(lw, "data/characters.dat", data, next);
	free(data);

	return 1;
}



int save_world_dungeon(struct dungeon_use *dngu, int index, DARNIT_LDI_WRITER *lw, int tileset) {
	int i;
	unsigned int size;
	struct savefile_dungeon_header h;
	struct savefile_dungeon_floor l;
	struct savefile_dungeon_object o;
	struct dungeon_floor_info *fi;
	char *data, *next, name[32];

	size = sizeof(h) + sizeof(l) * dngu->floors;
	size += (sizeof(struct dungeon_puzzle_part) * dngu->puzzles);
	
	for (i = 0; i < dngu->floors; i++)
		size += (dngu->floor_info[i].w * dngu->floor_info[i].h * sizeof(unsigned int) * 2);
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
		fi = &dngu->floor_info[i];
		l.stair_up = fi->stair_up;
		l.stair_down = fi->stair_down;
		l.floor_w = fi->w;
		l.floor_h = fi->h;
		d_util_endian_convert((void *) &l, sizeof(l) / sizeof(unsigned int));
		memcpy(next, &l, sizeof(l));
		next += sizeof(l);
		d_util_endian_convert((void *) fi->tile_data, fi->w * fi->h);
		memcpy(next, fi->tile_data, fi->w * fi->h * 4);
		d_util_endian_convert((void *) fi->tile_data, fi->w * fi->h);
		next += (fi->w * fi->h * 4);
		d_util_endian_convert((void *) fi->overlay_data, fi->w * fi->h);
		memcpy(next, fi->overlay_data, fi->w * fi->h * 4);
		d_util_endian_convert((void *) fi->overlay_data, fi->w * fi->h);
		next += (fi->w * fi->h * 4);
	}

	for (i = 0; i < dngu->objects; i++) {
		o.x = dngu->object[i].x;
		o.y = dngu->object[i].y;
		o.f = dngu->object[i].l;
		o.gfx_slot = dungeon_object_graphics(dngu->object[i].type, dngu->object[i].subtype);
		memset(o.ai_func, 0, 32);
		strcpy(o.ai_func, dungeon_object_ai(dngu->object[i].type, dngu->object[i].subtype));
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
