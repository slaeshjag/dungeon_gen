#include <darnit/darnit.h>
#include "character.h"
#include "world.h"
#include "aicomm.h"

void character_expand_entries();
void character_update_sprite(int entry);

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
	character_expand_entries();

	return;
}


void character_destroy() {
	unsigned int i;

	for (i = 0; (signed) i < ws.char_data->max_entries; i++)
		character_despawn(i);
	free(ws.char_data->entry);
	for (i = 0; i < ws.char_data->characters; i++)
		character_unload_graphics(i);
	free(ws.char_data->gfx);
	free(ws.char_data->collision);
	d_bbox_free(ws.char_data->bbox);
	free(ws.char_data);
	ws.char_data = NULL;

	return;
}


struct aicomm_struct character_message_next(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_NEXT;

	return ac;
}


void character_message_loop(struct aicomm_struct ac) {
	int x, y;

	for (;;) {
		ac.ce = ws.char_data->entry;
		if (!ws.char_data->entry[ac.self] || !ws.char_data->entry[ac.self]->loop) {
			ac.msg = AICOMM_MSG_NOAI;
			if (ac.from < 0 || ac.from >= ws.char_data->max_entries)
				return;
			if (!ws.char_data->entry[ac.from]->loop)
				return;
			ac.self = ac.from;
			ac.from = -1;
			ac = ws.char_data->entry[ac.from]->loop(ac);
		} else
			ac = ws.char_data->entry[ac.self]->loop(ac);

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
				ac = character_message_next(ac);
				break;
			case AICOMM_MSG_DIRU:
				if (ac.from < 0 || ac.from >= ws.char_data->max_entries ||
				    !ws.char_data->entry[ac.from]) {
					ac.msg = AICOMM_MSG_NOAI;
					ac.self = ac.from;
					ac.from = -1;
					break;
				}
				character_update_sprite(ac.from);
				ac = character_message_next(ac);
				break;
			case AICOMM_MSG_SETP:
				ws.camera.player = ac.self;
				ac = character_message_next(ac);
				break;
			case AICOMM_MSG_GETP:
				ac.self = ac.from;
				ac.from = ws.camera.player;
				break;
			case AICOMM_MSG_KILL:
				character_despawn(ac.self);
				ac = character_message_next(ac);
				break;
			case AICOMM_MSG_SPWN:
				x = ac.arg[1] * ws.camera.tile_w * 256;
				y = ac.arg[2] * ws.camera.tile_h * 256;
				character_spawn_entry(ac.arg[0], ac.argp, x, y, ac.arg[3]);
				ac = character_message_next(ac);
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

	if (!ws.char_data->entry[entry])
		return;

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
	ws.char_data->bbox = d_bbox_new(ws.char_data->max_entries);
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


int character_test_collision(int entry, int dx, int dy) {
	struct aicomm_struct ac;
	struct character_entry *ce;
	int x, y, w, h, e, i, s, n;

	ce = ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x += ((ce->x + dx) >> 8);
	y += ((ce->y + dy) >> 8);
	n = d_bbox_test(ws.char_data->bbox, x, y, w, h, (unsigned *) ws.char_data->collision, 
		ws.char_data->max_entries);
	
	ac.msg = AICOMM_MSG_COLL;
	ac.from = -1;
	
	for (i = s = 0; i < n; i++) {
		e = ws.char_data->collision[i];
		if (ws.char_data->entry[e]->l != ws.char_data->entry[ce->self]->l)
			continue;
		if (e == ce->self)
			continue;
		ac.arg[0] = ce->self;
		ac.self = e;
		character_message_loop(ac);
		ac.arg[0] = e;
		ac.self = ce->self;
		character_message_loop(ac);
		if (ws.char_data->entry[entry]->special_action.solid
		    && ws.char_data->entry[e]->special_action.solid)
			s = 1;
		
	}

	return s;
}


void character_update_sprite(int entry) {
	int x, y;
	struct character_entry *ce;

	ce = ws.char_data->entry[entry];

	d_sprite_hitbox(ce->sprite, &x, &y, NULL, NULL);
	x *= -1;
	y *= -1;
	x += (ce->x >> 8);
	y += (ce->y >> 8);

	d_sprite_direction_set(ce->sprite, ce->dir);
	(ce->special_action.animate?d_sprite_animate_start:d_sprite_animate_stop)(ce->sprite);
	d_sprite_move(ce->sprite, x, y);

	return;
}


void character_handle_movement(int entry) {
	int dx, dy;

	dx = ws.char_data->entry[entry]->dx;
	dy = ws.char_data->entry[entry]->dy;

	if (!dx && !dy)
		return;

	dx *= d_last_frame_time();
	dy *= d_last_frame_time();
	dx /= 1000;
	dy /= 1000;

	if (!character_test_collision(entry, dx, 0))
		ws.char_data->entry[entry]->x += dx;
	if (!character_test_collision(entry, 0, dy))
		ws.char_data->entry[entry]->y += dy;
	character_update_sprite(entry);
	
	return;
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
			d_sprite_hitbox_set(ce->sprite, j, 0, cg->sprite_hitbox[h], 
			    cg->sprite_hitbox[h+1], cg->sprite_hitbox[h+2],cg->sprite_hitbox[h+3]);
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
	character_set_hitbox(i);

	return i;
}


void character_loop_entry(struct character_entry *ce) {
	struct aicomm_struct ac;


	ac.msg = AICOMM_MSG_LOOP;
	ac.self = ce->self;
	character_message_loop(ac);
	character_handle_movement(ce->self);

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
	
	ac.msg = AICOMM_MSG_DESTROY;
	ac.from = -1;
	ac.self = entry;
	character_message_loop(ac);
	
	d_sprite_free(ce->sprite);
	free(ce);
	ws.char_data->entry[entry] = NULL;

	if (ws.camera.player == entry)
		ws.camera.player = -1;

	return;
}


int character_find_visible() {
	return d_bbox_test(ws.char_data->bbox, ws.camera.x - 96, ws.camera.y - 96,
		ws.camera.screen_w + 192, ws.camera.screen_h + 192, 
		(unsigned *) ws.char_data->collision, ws.char_data->max_entries);
}
	

void character_render_layer(int hits, int layer) {
	int i, e;

	for (i = 0; i < hits; i++) {
		e = ws.char_data->collision[i];
		if (!ws.char_data->entry[e])
			continue;
		if (ws.char_data->entry[e]->l != layer)
			continue;
		d_sprite_draw(ws.char_data->entry[e]->sprite);
	}

	return;
}
