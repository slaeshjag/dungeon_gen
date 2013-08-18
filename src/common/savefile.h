#ifndef __SAVEFILE_H__
#define	__SAVEFILE_H__

#define	SAVEFILE_VERSION		0x10001


/*** FOR SAVING CHARACTER GRAPHICS ***/
struct savefile_character_gfx {
	unsigned int			face_w;
	unsigned int			face_h;
	unsigned int			sprite_w;
	unsigned int			sprite_h;
	unsigned int			directions;
	unsigned int			zface;
	unsigned int			zsprite;
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
	int				type;
	int				subtype;
	int				link;
	int				save_slot;
};


#endif
