#include "character.h"
#include "aicomm_f.h"
#include "savefile.h"
#include "world.h"
#include <string.h>


struct aicomm_struct character_message_next(struct aicomm_struct ac);
void character_update_sprite(int entry);


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
	t = ws.char_data->teleport.entry[ac.arg[0]];

	/* TODO: Implement overworld + room */
	if (t.map == ws.active_dungeon) {
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
	ws.char_data->teleport.to.room = t.room;

	return character_message_next(ac);
}
