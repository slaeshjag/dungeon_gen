#ifndef __GENERATE_WORLD_H__
#define	__GENERATE_WORLD_H__

#include "dungeon_generate.h"
#include "save_world.h"
#include "savefile.h"

void generate_world(int saveslot);


struct generate_world {
	DARNIT_LDI_WRITER		*lw;
	DARNIT_STRINGTABLE		*st;

	int				dungeons;
	int				characters;
	struct savefile_character_type	*char_type;
};


#endif
