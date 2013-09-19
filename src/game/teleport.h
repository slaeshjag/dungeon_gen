#ifndef __TELEPORT_H__
#define	__TELEPORT_H__

#include "savefile.h"

/* Han teleporterar taliban! Han teleporterar taliban! Han teleporterar taaaliibaaan han teleporterar taliban! */


void teleport_load();
void teleport_unload();


struct teleport_to {
	int				slot;
	char				ai[32];
	unsigned int			x;
	unsigned int			y;
	unsigned int			l;
	int				dungeon;
	int				room;
};


struct teleport {
	struct savefile_teleport_entry	*entry;
	unsigned int			entries;
	struct teleport_to		to;
};


#endif
