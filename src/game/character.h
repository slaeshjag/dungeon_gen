#ifndef __CHARACTER_H__
#define	__CHARACTER_H__

#include <darnit/darnit.h>

struct character_entry {
	void			(*loop)();
	/* X and Y is fix-point 24.8 */
	int			x;
	int			y;
	int			l;
	int			dir;
	int			slot;
	DARNIT_SPRITE		*sprite;
};


struct character_data {
	DARNIT_BBOX		*bbox;
	unsigned int		characters;
	struct char_gfx		**gfx;
	struct character_entry	**entry;
	int			entries;
	int			max_entries;
};

void character_init();
int character_load_graphics(unsigned int slot);


#endif
