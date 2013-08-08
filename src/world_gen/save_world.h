#ifndef __SAVE_WORLD_H__
#define	__SAVE_WORLD_H__

#include "dungeon_generate.h"
#include <darnit/darnit.h>

struct save_dungeon_header {
	unsigned int			floors;
	unsigned int			objects;
	unsigned int			puzzles;
	unsigned int			entrance;
	unsigned int			entrance_floor;
};

struct save_dungeon_layer {
	int				stair_up;
	int				stair_down;
	int				layer_w;
	int				layer_h;
};


struct save_dungeon_object {
	int				x;
	int				y;
	int				l;
	int				type;
	int				subtype;
	int				link;
	int				save_slot;
};


int save_world_dungeon(struct dungeon_use *dngu, int index, DARNIT_LDI_WRITER *l);


#endif
