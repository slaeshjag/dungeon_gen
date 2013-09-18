#ifndef __SAVEFILE_H__
#define	__SAVEFILE_H__

#define	SAVEFILE_VERSION		0x10001

#define	MAP_FLAG_EVENT			0x100000


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
	/* -1 for overworld */
	int				map;
	/* -1 for no room */
	int				room;
	int				x;
	int				y;
	int				l;
};


/*** FOR SAVING DUNGEONS ***/
struct savefile_dungeon_header {
	unsigned int			floors;
	unsigned int			objects;
	unsigned int			puzzles;
	unsigned int			tileset;
	unsigned int			entrance;
	unsigned int			entrance_floor;
};


struct savefile_dungeon_floor {
	int				stair_up;
	int				stair_down;
	int				floor_w;
	int				floor_h;
};


struct savefile_dungeon_puzzle_part {
	int				link;
	int				floor;
	int				group;
	int				depend;
	int				provide;
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


#endif
