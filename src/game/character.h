#ifndef __CHARACTER_H__
#define	__CHARACTER_H__

#include <darnit/darnit.h>
#include "aicomm.h"
#include "teleport.h"



struct ai_lib {
	void			*lib;
	char			*ainame;
};


struct character_data {
	DARNIT_BBOX		*bbox;
	unsigned int		characters;
	struct char_gfx		**gfx;
	struct character_entry	**entry;
	int			*collision;
	int			entries;
	int			max_entries;
	struct ai_lib		*ai_lib;
	int			ai_libs;
	struct teleport		teleport;
};

void character_init();
void character_destroy();
int character_load_ai_lib(const char *fname);
int character_load_graphics(unsigned int slot);
int character_unload_graphics(unsigned int slot);
int character_spawn_entry(unsigned int slot, const char *ai, int x, int y, int l, int map, int save);
void character_spawn_map(int map_slot);
int character_find_visible();
void character_tell_all(struct aicomm_struct ac);
void character_message_loop(struct aicomm_struct ac);
void character_despawn(int entry);
void character_despawn_map(int map);
void character_loop();
void character_render_layer(int hits, int layer);


#endif
