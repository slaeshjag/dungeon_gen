#ifndef __CHARACTER_H__
#define	__CHARACTER_H__

#include <darnit/darnit.h>
#include "aicomm.h"



struct character_data {
	DARNIT_BBOX		*bbox;
	unsigned int		characters;
	struct char_gfx		**gfx;
	struct character_entry	**entry;
	int			*collision;
	int			entries;
	int			max_entries;
};

void character_init();
int character_load_graphics(unsigned int slot);


#endif
