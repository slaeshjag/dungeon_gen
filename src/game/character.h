#ifndef __CHARACTER_H__
#define	__CHARACTER_H__

#include <darnit/darnit.h>

struct character_entry {
	void			(*loop)();
	DARNIT_SPRITE		*sprite;
};


struct character_data {
	unsigned int		characters;
	struct char_gfx		**gfx;
};

void character_init();
int character_load_graphics(unsigned int slot);


#endif
