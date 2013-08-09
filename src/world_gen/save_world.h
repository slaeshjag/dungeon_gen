#ifndef __SAVE_WORLD_H__
#define	__SAVE_WORLD_H__

#include "dungeon_generate.h"
#include "savefile.h"
#include <darnit/darnit.h>

int save_world_dungeon(struct dungeon_use *dngu, int index, DARNIT_LDI_WRITER *l, int tileset);


#endif
