#include <darnit/darnit.h>
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
	d_bbox_sortmode(ws.char_data->bbox, DARNIT_BBOX_SORT_Y);

	for (i = 0; i < ws.char_data->max_entries; i++)
		ws.char_data->entry[i] = NULL;

	return;
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

		ac.ce = ws.char_data->entry;

		switch (ac.msg) {
			case AICOMM_MSG_DONE:
				return;
			case AICOMM_MSG_INIT:
			case AICOMM_MSG_NOAI:
			case AICOMM_MSG_DESTROY:
			case AICOMM_MSG_LOOP:
				/* Invalid return messages */
				fprintf(stderr, "WARNING: char %i returned invalid message %i\n",
					ac.from, ac.msg);
				return;
			case AICOMM_MSG_COLL:
			case AICOMM_MSG_SEND:
			case AICOMM_MSG_INVM:
			case AICOMM_MSG_NEXT:
				/* These are just passed on */
				break;
			case AICOMM_MSG_FOLM:
				ws.camera.follow_char = ac.from;
				ac.self = ac.from;
				ac.from = -1;
				ac.msg = AICOMM_MSG_NEXT;
				break;
			case AICOMM_MSG_DIRU:
				if (ac.from < 0 || ac.from >= ws.char_data->max_entries ||
				    !ws.char_data->entry[ac.from]) {
					ac.msg = AICOMM_MSG_NOAI;
					ac.self = ac.from;
					ac.from = -1;
					break;
				}
				d_sprite_direction_set(ws.char_data->entry[ac.from]->sprite,
				    ws.char_data->entry[ac.from]->dir);
				if (ws.char_data->entry[ac.from]->special_action.animate)
					d_sprite_animate_start(ws.char_data->entry[ac.from]->sprite);
				else
					d_sprite_animate_stop(ws.char_data->entry[ac.from]->sprite);
				break;
			default:
				ac.self = ac.from;
				ac.from = -1;
				ac.msg = AICOMM_MSG_INVM;
				break;
		}

	}

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
	d_bbox_sortmode(ws.char_data->bbox, DARNIT_BBOX_SORT_Y);

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


int character_unload_graphics(unsigned int slot) {
	if (slot >= ws.char_data->characters)
		return 0;
	if (!ws.char_data->gfx[slot])
		return 0;
	ws.char_data->gfx[slot]->link--;
	if (!ws.char_data->gfx[slot]->link)
		ws.char_data->gfx[slot] = character_gfx_data_unload(ws.char_data->gfx[slot]);
	return 1;
}


int character_spawn_entry(unsigned int slot, const char *ai, int x, int y, int l) {
	int i, j, k, h;
	struct character_entry *ce;
	struct char_gfx *cg;
	struct aicomm_struct ac;
	struct {
		int		tile;
		int		time;
	} *sprite;

	if (ws.char_data->characters <= slot)
		return -1;

	character_load_graphics(slot);
	sprite = (void *) ws.char_data->gfx[slot]->sprite_data;
	ce = malloc(sizeof(*ce));
	ce->sprite = d_sprite_new(ws.char_data->gfx[slot]->sprite_ts);
	cg = ws.char_data->gfx[slot];


	for (i = j = k = 0; sprite[i].tile != -1 || sprite[i].time != -1; i++) {
		h = j * 4;
		if (sprite[i].tile >= 0) {
			if (!k)
				d_sprite_hitbox_set(ce->sprite, j, 0, 
					cg->sprite_hitbox[h], cg->sprite_hitbox[h+1], 
					cg->sprite_hitbox[h+2], cg->sprite_hitbox[h+3]);
			d_sprite_frame_entry_set(ce->sprite, j, k++, sprite[i].tile, sprite[i].time);
		} else
			j++, k = 0;
	}

	ce->x = x << 8;
	ce->y = y << 8;
	ce->l = l;
	ce->slot = slot;
	ce->dir = 0;
	*((unsigned int *) (&ce->special_action)) = 0;

	d_sprite_activate(ce->sprite, 0);
	d_sprite_move(ce->sprite, ce->x >> 8, ce->y >> 8);
	if (ws.char_data->entries == ws.char_data->max_entries)
		character_expand_entries();
	for (i = 0; i < ws.char_data->max_entries; i++)
		if (!ws.char_data->entry[i])
			break;
	ws.char_data->entry[i] = ce;
	ws.char_data->entry[i]->self = i;
	
	/* TODO: Init character AI */
	ce->loop = NULL;
	ac.msg = AICOMM_MSG_INIT;
	ac.from = -1;
	ac.self = ce->self;
	character_message_loop(ac);

	return i;
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
		if (ws.char_data->entry[e]->l != ws.char_data->entry[ce->self]->l)
			continue;
		if (e == ce->self)
			continue;
		ac.arg1 = ce->self;
		ac.self = e;
		character_message_loop(ac);
		ac.arg1 = e;
		ac.self = ce->self;
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


void character_despawn(int entry) {
	struct character_entry *ce;
	struct aicomm_struct ac;

	if (entry < 0 || entry >= ws.char_data->max_entries)
		return;

	ce = ws.char_data->entry[entry];
	if (!ce)
		return;
	
	ac.ce = ws.char_data->entry;
	ac.msg = AICOMM_MSG_DESTROY;
	ac.from = -1;
	ac.self = entry;
	character_message_loop(ac);
	
	d_sprite_free(ce->sprite);
	free(ce);
	ws.char_data->entry[entry] = NULL;

	return;
}

