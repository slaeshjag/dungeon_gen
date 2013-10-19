#ifndef __SAVEFILE_H__
#define	__SAVEFILE_H__

#define	SAVEFILE_VERSION		0x10002

#define	MAP_FLAG_EVENT			0x100000
#define	MAP_FLAG_TELEPORT		0x200000


/*** FOR SAVING CHARACTER GRAPHICS ***/

struct savefile_character_type {
	unsigned int			gender	: 1;
};

struct savefile_character_gfx {
	unsigned int			face_w;
	unsigned int			face_h;
	unsigned int			sprite_w;
	unsigned int			sprite_h;
	unsigned int			sprite_frames;
	unsigned int			directions;
	unsigned int			zface;
	unsigned int			zsprite;
	unsigned int			zspritedata;
	struct savefile_character_type	type;
};



/*** FOR BOTH OVERWORLD AND DUNGEON */

struct savefile_teleport_entry {
	int				map;
	int				x;
	int				y;
	int				l;
};


/*** FOR SAVING DUNGEONS ***/

struct savefile_dungeon_header {
	unsigned int			version;
	unsigned int			objects;
	unsigned int			tileset;
	unsigned int			map_w;
	unsigned int			map_h;
	unsigned int			layers;
	const char			area_name[32];
	const char			music[32];
	int				neighbours[8];
};


struct savefile_dungeon_object {
	int				x;
	int				y;
	int				f;
	int				gfx_slot;
	int				link;
	int				save_slot;
	char				ai_func[32];
};


/*** ITEMS ***/

enum savefile_item_flags {
	SAVEFILE_ITEM_OVERWORLD		= 01,
	SAVEFILE_ITEM_INVENTORY		= 02,
	SAVEFILE_ITEM_SPECBATTLE	= 04,
};

struct savefile_item_entry {
	unsigned int			gfx_slot;
	enum savefile_item_flags	flags;
	char				handler[32];
};


#endif
