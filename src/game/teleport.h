#ifndef __TELEPORT_H__
#define	__TELEPORT_H__

#include "savefile.h"

/* Han teleporterar taliban! Han teleporterar taliban! Han teleporterar taaaliibaaan han teleporterar taliban! */


void teleport_load();
void teleport_unload();


struct teleport {
	struct savefile_teleport_entry	*entry;
	unsigned int			entries;
};


#endif
