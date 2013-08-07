#include "generate_world.h"
#include "random.h"

#define	ROOM_W			16
#define	ROOM_H			16
#define	ROOMS_W			12
#define	ROOMS_H			12
#define	ROOM_MIN		16
#define	ROOM_MAX		24
#define	FLOORS_MIN		2
#define	FLOORS_MAX		7
#define	ENEMY_MAX		7


void generate_dungeons(int n, DARNIT_LDI_WRITER *lw);


void generate_world(int saveslot) {
	DARNIT_FILE *f;
	DARNIT_LDI_WRITER *lw;
	char name[32];

	sprintf(name, "world_%i.save", saveslot);
	f = d_file_open(name, "w+b");
	lw = d_file_ldi_write(f, 1);

	generate_dungeons(1, lw);

	d_file_ldi_write_end(lw);
	d_file_close(f);

	return;
}


void generate_dungeons(int n, DARNIT_LDI_WRITER *lw) {
	int i, floors;
	struct dungeon *dungeon;
	struct dungeon_use *dngu;

	for (i = 0; i < n; i++) {
		floors = random_get() % (FLOORS_MAX - FLOORS_MIN) + FLOORS_MIN;
		dungeon = dungeon_layout_new(ROOMS_W, ROOMS_H, ROOM_MAX, ROOM_MIN, 1, floors);

		/* FIXME: Pay more attention to entrance floor (last arg.) */
		dungeon_init_floor(dungeon, ROOM_W, ROOM_H, ENEMY_MAX, 0);
		/* TODO: Select a fitting autotiler */
		dngu = dungeon_make_usable(dungeon, NULL);
		dungeon_free_generate_dungeon(dungeon);

		save_world_dungeon(dngu, i, lw);
		dungeon_free_usable(dngu);
	}

	return;
}

