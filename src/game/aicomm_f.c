#include "character.h"
#include "aicomm_f.h"
#include "savefile.h"
#include "textbox.h"
#include "world.h"
#include <string.h>


int character_get_character_looked_at(int src);
struct aicomm_struct character_message_next(struct aicomm_struct ac);
void character_update_sprite(int entry);
void character_despawn(int entry);


struct aicomm_struct aicomm_f_diru(struct aicomm_struct ac) {
	if (ac.from < 0 || ac.from >= ws.char_data->max_entries ||
	    !ws.char_data->entry[ac.from]) {
		ac.msg = AICOMM_MSG_NOAI;
		ac.self = ac.from;
		ac.from = -1;
		return ac;
	}

	character_update_sprite(ac.from);
	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_tpme(struct aicomm_struct ac) {
	struct savefile_teleport_entry t;
	
	if (ac.arg[0] >= (signed) ws.char_data->teleport.entries) {
		ac.self = ac.from;
		ac.from = -1;
		ac.msg = AICOMM_MSG_INVM;
		return ac;
	}
	
	/* TODO: Add teleport ID offset */
	t = ws.char_data->teleport.entry[ac.arg[0]];

	if (t.map == ws.active_world) {
		ac.ce[ac.from]->x = (t.x << 8) * ws.camera.tile_w;
		ac.ce[ac.from]->y = (t.y << 8) * ws.camera.tile_h;
		ac.ce[ac.from]->l = t.l;
		return character_message_next(ac);
	}

	ws.new_state = WORLD_STATE_CHANGEMAP;
	
	ws.char_data->teleport.to.slot = ac.ce[ac.from]->slot;
	strcpy(ws.char_data->teleport.to.ai, ac.ce[ac.from]->ai);
	ws.char_data->teleport.to.x = t.x;
	ws.char_data->teleport.to.y = t.y;
	ws.char_data->teleport.to.l = t.l;
	ws.char_data->teleport.to.dungeon = t.map;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_folm(struct aicomm_struct ac) {
	ws.camera.follow_char = ac.from;
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_NEXT;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_setp(struct aicomm_struct ac) {
	ws.camera.player = ac.self;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_getp(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = ws.camera.player;
	
	return ac;
}


struct aicomm_struct aicomm_f_kill(struct aicomm_struct ac) {
	character_despawn(ac.self);
	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_spwn(struct aicomm_struct ac) {
	int x, y;

	x = ac.arg[1] * ws.camera.tile_w;
	y = ac.arg[2] * ws.camera.tile_h;
	character_spawn_entry(ac.arg[0], ac.argp, x, y, ac.arg[3], 4, -1);

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_getf(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = character_get_character_looked_at(ac.self);

	return ac;
}


struct aicomm_struct aicomm_f_camn(struct aicomm_struct ac) {
	ws.camera.jump = 1;

	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_tbox(struct aicomm_struct ac) {
	struct textbox_properties *tp;

	tp = ac.argp;
	if (tp)
		textbox_add_message(tp->message, tp->question, ac.arg[0], ac.from);
	return character_message_next(ac);
}


struct aicomm_struct aicomm_f_invm(struct aicomm_struct ac) {
	ac.self = ac.from;
	ac.from = -1;
	ac.msg = AICOMM_MSG_INVM;

	return ac;
}


struct aicomm_struct aicomm_f_dummy(struct aicomm_struct ac) {
	return ac;
}
