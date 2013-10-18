#include "generate_world.h"
#include "autotiler.h"
#include "random.h"
#include <string.h>

#define	ROOM_W			16
#define	ROOM_H			16
#define	ROOMS_W			12
#define	ROOMS_H			12
#define	ROOM_MIN		16
#define	ROOM_MAX		24
#define	FLOORS_MIN		2
#define	FLOORS_MAX		7
#define	ENEMY_MAX		7


void generate_dungeons(struct generate_world *w);
void generate_character_graphics(struct generate_world *w, int characters);


void generate_world(int saveslot) {
	struct generate_world w;
	DARNIT_FILE *f;
	char name[32];

	d_fs_mount("bin/char_gen.ldi");
	w.st = d_stringtable_open("bin/autotile.stz");
	sprintf(name, "world_%i.save", saveslot);
	f = d_file_open(name, "w+b");
	w.lw = d_file_ldi_write(f, 3);

	w.dungeons = 1;
	fprintf(stderr, "Generating dungeons... ");
	generate_dungeons(&w);
	fprintf(stderr, "Generating character graphics... ");
	generate_character_graphics(&w, 4);
	fprintf(stderr, "\n");
	save_map_info(w.lw);

	d_file_ldi_write_end(w.lw);
	d_file_close(f);
	free(w.char_type);

	return;
}


void generate_character_graphics(struct generate_world *w, int characters) {
	struct generated_char **gc;
	int i;
	
	gc = malloc(sizeof(*gc) * characters);
	w->char_type = malloc(sizeof(*w->char_type) * characters);
	for (i = 0; i < characters; i++) {
		gc[i] = generate_character();
		w->char_type[i] = gc[i]->char_type;
	}
	save_characters(gc, characters, w->lw);
	for (i = 0; i < characters; i++)
		generate_char_free(gc[i]);
	w->characters = characters;

	return;
}
	

void generate_dungeons(struct generate_world *w) {
	int i, floors, n;
	unsigned int *tmp_tile;
	struct dungeon_use *dngu;
	struct autotile *at;

	n = w->dungeons;
	tmp_tile = NULL;
	d_stringtable_section_load(w->st, "DUNGEON");
	at = autotile_new(w->st);

	for (i = 0; i < n; i++) {
		/* TODO: Select a fitting autotiler */
		floors = random_get() % (FLOORS_MAX - FLOORS_MIN) + FLOORS_MIN;
		#if 0		/* Testing h4xxel's generator */
		dungeon = dungeon_layout_new(ROOMS_W, ROOMS_H, ROOM_MAX, ROOM_MIN, 1, floors);

		/* FIXME: Pay more attention to entrance floor (last arg.) */
		dungeon_init_floor(dungeon, ROOM_W, ROOM_H, ENEMY_MAX, 0);
		dngu = dungeon_make_usable(dungeon);
		
		for (j = 0; j < dngu->floors; j++) {
			tmp_tile = realloc(tmp_tile, sizeof(*tmp_tile) * dngu->w[j] * dngu->h[j]);
			autotile_run(at, dngu->tile_data[j], tmp_tile, dngu->w[j], dngu->h[j]);
			memcpy(dngu->tile_data[j], tmp_tile, sizeof(*tmp_tile) * dngu->w[j] * dngu->h[j]);
		}
	
		dungeon_free_generate_dungeon(dungeon);
		#else
		dngu = dungeon_generate_diamond_square(64, NULL);
		#endif

		save_world_dungeon(dngu, i, w->lw, 0);
		dungeon_free_usable(dngu);
	}

	free(tmp_tile);
	autotile_destroy(at);
	d_stringtable_section_unload(w->st, "DUNGEON");

	return;
}

