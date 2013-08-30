#include "character.h"
#include "world.h"
#include "aicomm.h"


void character_init() {
	unsigned int chars;
	int i;

	chars = character_gfx_data_characters();
	ws.char_data = malloc(sizeof(*ws.char_data));
	ws.char_data->characters = chars;
	ws.char_data->gfx = calloc(sizeof(*ws.char_data->gfx) * chars, 1);
	ws.char_data->max_entries = 8;
	ws.char_data->entries = 0;
	ws.char_data->entry = malloc(sizeof(*ws.char_data->entry) * ws.char_data->max_entries);
	ws.char_data->collision = malloc(sizeof(unsigned int) * ws.char_data->max_entries);
	ws.char_data->bbox = d_bbox_new(ws.char_data->max_entries);

	for (i = 0; i < ws.char_data->max_entries; i++)
		ws.char_data->entry[i] = NULL;

	return;
}


void character_set_hitbox(int entry) {
	int x, y, w, h;
	struct character_entry *ce;

	/* If the behaviour of darnit bbox ever changes, this will break */
	ce = ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += (ce->x >> 8);
	y += (ce->y >> 8);
	d_bbox_move(ws.char_data->bbox, entry, x, y);
	d_bbox_resize(ws.char_data->bbox, entry, (unsigned) w, (unsigned) h);

	return;
}


void character_expand_entries() {
	int i, nz;

	nz = (ws.char_data->max_entries << 1);
	ws.char_data->entry = realloc(ws.char_data->entry, nz * sizeof(*ws.char_data->entry));
	ws.char_data->collision = realloc(ws.char_data->collision, nz * sizeof(unsigned int));
	for (i = 0; i < ws.char_data->max_entries; i++)
		ws.char_data->entry[i + ws.char_data->max_entries] = NULL;
	
	ws.char_data->max_entries <<= 1;
	d_bbox_free(ws.char_data->bbox);
	d_bbox_new(ws.char_data->max_entries);

	for (i = 0; i < ws.char_data->max_entries; i++)
		d_bbox_add(ws.char_data->bbox, ~0, ~0, 0, 0);
	
	for (i = 0; i < (ws.char_data->max_entries >> 1); i++)
		character_set_hitbox(i);
	
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


int character_spawn_entry(unsigned int slot, const char *ai, int x, int y, int l) {
	int i, j, k, h;
	struct character_entry *ce;
	struct char_gfx *cg;
	struct {
		int		tile;
		int		time;
	} *sprite;

	if (ws.char_data->characters <= slot)
		return -1;

	sprite = (void *) ws.char_data->gfx[slot]->sprite_data;
	character_load_graphics(slot);
	ce = malloc(sizeof(*ce));
	ce->sprite = d_sprite_new(ws.char_data->gfx[slot]->sprite_ts);
	cg = ws.char_data->gfx[slot];

	/* TODO: Init character AI */
	ce->loop = NULL;
	
	for (i = j = k = 0; sprite[i].tile != -1 || sprite[i].time != -1; i++) {
		h = j * 4;
		if (sprite[i].tile >= 0) {
			if (!k)
				d_sprite_hitbox_set(ce->sprite, j, 0, 
					cg->sprite_hitbox[h], cg->sprite_hitbox[h+1], 
					cg->sprite_hitbox[h+2], cg->sprite_hitbox[h+3]);
			d_sprite_frame_entry(ce->sprite, j, k++, sprite[i].tile, sprite[i].time);
		} else
			j++, k = 0;
	}

	ce->x = x << 8;
	ce->y = y << 8;
	ce->l = l;
	ce->slot = slot;
	ce->dir = 0;
	*((unsigned int *) (&ce->special_action)) = 0;

	d_sprite_move(ce->sprite, ce->x >> 8, ce->y >> 8);
	if (ws.char_data->entries == ws.char_data->max_entries)
		character_expand_entries();
	for (i = 0; i < ws.char_data->max_entries; i++)
		if (!ws.char_data->entry)
			break;
	ws.char_data->entry[i] = ce;

	return i;
}


void character_message_loop(struct aicomm_struct ac) {
	for (;;) {
		if (!ws.char_data->entry[ac.self]->loop) {
			ac.msg = AICOMM_MSG_NOAI;
			if (ac.from < 0 || ac.from >= ws.char_data->max_entries)
				return;
			if (!ws.char_data->entry[ac.from]->loop)
				return;
			ac = ws.char_data->entry[ac.from]->loop(ac);
		} else
			ac = ws.char_data->entry[ac.self]->loop(ac);

		if (ac.msg == AICOMM_MSG_DONE)
			return;
	}

	return;
}


void character_loop_entry(struct character_entry *ce) {
	int x, y, w, h, n, e, i;
	struct aicomm_struct ac;

	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += (ce->x >> 8);
	y += (ce->y >> 8);
	n = d_bbox_test(ws.char_data->bbox, x, y, w, h, (unsigned *) ws.char_data->collision, 
		ws.char_data->max_entries);
	
	ac.ce = ws.char_data->entry;

	ac.msg = AICOMM_MSG_COLL;
	ac.from = -1;
	
	for (i = 0; i < n; i++) {
		e = ws.char_data->collision[i];
		if (e == ce->slot)
			continue;
		ac.arg1 = ce->slot;
		ac.self = e;
		character_message_loop(ac);
		ac.arg1 = e;
		ac.self = ce->slot;
		character_message_loop(ac);
	}

	ac.msg = AICOMM_MSG_LOOP;
	for (i = 0; i < ws.char_data->max_entries; i++) {
		ac.self = i;
		character_message_loop(ac);
	}

	return;
}


void character_loop() {
	int i;

	for (i = 0; i < ws.char_data->max_entries; i++) {
		if (!ws.char_data->entry[i])
			continue;
		character_loop_entry(ws.char_data->entry[i]);
	}

	return;
}


void *character_free_entry(struct character_entry *ce) {
	if (!ce)
		return NULL;
	/* TODO: Add call to character AI for cleanup */
	d_sprite_free(ce->sprite);
	free(ce);

	return NULL;
}

