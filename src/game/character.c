#include <darnit/darnit.h>
#include <string.h>
#include "character.h"
#include "world.h"
#include "aicomm.h"

void character_expand_entries();
void character_update_sprite(int entry);


int character_get_character_looked_at(int src) {
	int x, y, w, h, xt, yt, n;
	unsigned int t;

	if (src < 0 || src >= ws.char_data->max_entries)
		return -1;
	if (!ws.char_data->entry[src])
		return -1;
	d_sprite_hitbox(ws.char_data->entry[src]->sprite, &x, &y, &w, &h);
	x += (ws.char_data->entry[src]->x >> 8);
	y += (ws.char_data->entry[src]->y >> 8);

	switch (ws.char_data->entry[src]->dir) {
		case 0:	/* West */
			xt = x - (w >> 1);
			yt = y + (h >> 1);
			break;
		case 1:	/* North */
			xt = x + (w >> 1);
			yt = y - (h >> 1);
			break;
		case 2:	/* East */
			xt = x + w;
			yt = y + (h >> 1);
			break;
		case 3:	/* South */
			xt = x + (w >> 1);
			yt = y + h;
			break;
		case 4: /* North-west */
			xt = x - (w >> 1);
			yt = y - (h >> 1);
			break;
		case 5:	/* North-east */
			xt = x + w;
			yt = y - (h >> 1);
			break;
		case 6:	/* South-east */
			xt = x + w;
			yt = y + h;
			break;
		case 7:	/* South-west */
			xt = x - (w >> 1);
			yt = y + h;
			break;
		default:
			return -1;
			break;
	}

	n = d_bbox_test(ws.char_data->bbox, xt, yt, (w >> 1), (h >> 1), &t, 1);
	if (!n)
		return -1;
	return t;
}
			


int character_load_ai_lib(const char *fname) {
	int i;

	i = ws.char_data->ai_libs++;
	ws.char_data->ai_lib = realloc(ws.char_data->ai_lib, 
		sizeof(*ws.char_data->ai_lib) * ws.char_data->ai_libs);
	if (!(ws.char_data->ai_lib[i].lib = d_dynlib_open(fname))) {
		ws.char_data->ai_libs--;
		return 0;
	}

	ws.char_data->ai_lib[i].ainame = malloc(strlen(fname) + 1);
	strcpy(ws.char_data->ai_lib[i].ainame, fname);

	return 1;
}


void *character_find_ai_func(const char *name) {
	int i;
	void *func = NULL;

	for (i = 0; i < ws.char_data->ai_libs && !func; i++)
		func = d_dynlib_get(ws.char_data->ai_lib[i].lib, name);
	return func;
}


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
	ws.char_data->ai_lib = NULL;
	ws.char_data->ai_libs = 0;
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
	for (i = 0; (signed) i < ws.char_data->ai_libs; i++) {
		ws.char_data->ai_lib[i].lib = d_dynlib_close(ws.char_data->ai_lib[i].lib);
		free(ws.char_data->ai_lib[i].ainame);
	}

	free(ws.char_data->ai_lib);
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
		if (ac.self < 0)
			return;
		else if (!ws.char_data->entry[ac.self] || !ws.char_data->entry[ac.self]->loop) {
			ac.msg = AICOMM_MSG_NOAI;
			if (ac.from < 0 || ac.from >= ws.char_data->max_entries)
				return;
			if (!ws.char_data->entry[ac.from])
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
			case AICOMM_MSG_GETF:
				ac.self = ac.from;
				ac.from = character_get_character_looked_at(ac.self);
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


int character_test_map(int entry, int dx, int dy) {
	int x, y, w, h, x2, y2, x3, y3, t, t2, t3, t4, dir, l;
	struct character_entry *ce;
	struct aicomm_struct ac;
	
	ce = ws.char_data->entry[entry];
	d_sprite_hitbox(ce->sprite, NULL, NULL, &w, &h);
	x2 = ((ce->x + dx) >> 8);
	y2 = ((ce->y + dy) >> 8);
	x = (ce->x >> 8);
	y = (ce->y >> 8);
	l = ce->l;
	if (x < 0 || x2 < 0 || y < 0 || y2 < 0)
		return 1;

	t3 = world_calc_tile(x2, y2, ce->l);
	t4 = world_calc_tile(x, y, ce->l);
	x3 = x2;
	y3 = y2;

	if (dx > 0) {
		x += w;
		x2 += w;
	} else if (dy > 0) {
		y += h;
		y2 += h;
	}

	t = world_calc_tile(x, y, ce->l);
	t2 = world_calc_tile(x2, y2, ce->l);

	if (t < 0 || t2 < 0 || t3 < 0)
		return 1;
	if (dx < 0 || dy < 0) {
		if (t == t2)
			return 0;
	} else {
		if (t4 == t3)
			return 0;
	}

	if (dx) {
		dir = (dx < 0) ? 0x4 : 0x1;
		if (world_get_tile(x2, y2, l) & (dir << 16))
			return 1;
		if (world_get_tile(x2, y3, l) & (dir << 16))
			return 1;
	} else if (dy) {
		dir = (dy < 0) ? 0x8 : 0x2;
		if (world_get_tile(x2, y2, l) & (dir << 16))
			return 1;
		if (world_get_tile(x3, y2, l) & (dir << 16))
			return 1;
	}

	ac.from = -1;
	ac.msg = AICOMM_MSG_MAPE;
	ac.self = entry;
	if (world_get_tile(x, y, ce->l) & MAP_FLAG_EVENT) {
		ac.arg[0] = t;
		ac.arg[1] = world_get_tile(x, y, ce->l);
		character_message_loop(ac);
	}

	if (world_get_tile(x2, y2, ce->l) & MAP_FLAG_EVENT) {
		ac.arg[0] = t2;
		ac.arg[1] = world_get_tile(x2, y2, ce->l);
		character_message_loop(ac);
	}
	
	return 0;
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
	int x, y, w, h;
	struct character_entry *ce;

	ce = ws.char_data->entry[entry];

	d_sprite_hitbox(ce->sprite, &x, &y, &w, &h);
	x *= -1;
	y *= -1;
	x += (ce->x >> 8);
	y += (ce->y >> 8);

	d_sprite_direction_set(ce->sprite, ce->dir);
	(ce->special_action.animate ? d_sprite_animate_start : d_sprite_animate_stop)(ce->sprite);
	d_sprite_move(ce->sprite, x, y);
	d_bbox_move(ws.char_data->bbox, ce->self, x, y);
	d_bbox_resize(ws.char_data->bbox, ce->self, w, h);

	return;
}


void character_handle_movement(int entry) {
	int dx, dy;
	struct character_entry *e;

	e = ws.char_data->entry[entry];
	dx = e->dx;
	dy = e->dy;

	if (!dx && !dy)
		return;

	dx *= d_last_frame_time();
	dy *= d_last_frame_time();
	dx /= 1000;
	dy /= 1000;

	while (dx) {
		if (!character_test_collision(entry, dx, 0) && !character_test_map(entry, dx, 0)) {
			ws.char_data->entry[entry]->x += dx;
			break;
		}
		if (!(dx / 256))
			break;
		dx += (dx < 0) ? 256 : -256;
	}

	while (dy) {
		if (!character_test_collision(entry, 0, dy) && !character_test_map(entry, 0, dy)) {
			ws.char_data->entry[entry]->y += dy;
			break;
		}
		if (!(dy / 256))
			break;
		dy += (dy < 0) ? 256 : -256;
	}

	character_update_sprite(entry);

	e->dx = 0, e->dy = 0;
	
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
			d_sprite_hitbox_set(ce->sprite, j, k, cg->sprite_hitbox[h], 
			    cg->sprite_hitbox[h+1], cg->sprite_hitbox[h+2],cg->sprite_hitbox[h+3]);
			d_sprite_frame_entry_set(ce->sprite, j, k++, sprite[i].tile, sprite[i].time);
		} else
			j++, k = 0;
	}

	ce->x = x << 8;
	ce->y = y << 8;
	ce->l = l;
	ce->dx = ce->dy = 0;
	ce->slot = slot;
	ce->dir = 0;
	ce->state = NULL;
	*((unsigned int *) (&ce->special_action)) = 0;

	d_sprite_activate(ce->sprite, 0);
	if (ws.char_data->entries == ws.char_data->max_entries)
		character_expand_entries();
	for (i = 0; i < ws.char_data->max_entries; i++)
		if (!ws.char_data->entry[i])
			break;
	ws.char_data->entry[i] = ce;
	ws.char_data->entry[i]->self = i;
	character_update_sprite(i);
	
	ce->loop = character_find_ai_func(ai);
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
	ac.from = -1;
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
