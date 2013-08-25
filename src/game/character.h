#ifndef __CHARACTER_H__
#define	__CHARACTER_H__

struct world_state;

struct character_entry {
	void			(*loop)();
};


struct character_data {
	unsigned int		characters;
	struct char_gfx		**gfx;
};

void character_init(struct world_state *s);
int character_load_graphics(struct world_state *s, unsigned int slot);


#endif
