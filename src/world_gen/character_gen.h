#ifndef __CHARACTER_GEN_H__
#define __CHARACTER_GEN_H__

#define	CHAR_FACE_W		64
#define	CHAR_FACE_H		64
#define	CHAR_SPRITE_W		32
#define	CHAR_SPRITE_H		48
#define	CHAR_SPRITE_DIRECTIONS	1

struct generated_char {
	unsigned int		*face;
	int			face_w;
	int			face_h;
	unsigned int		*sprite;
	unsigned int		sprite_w;
	unsigned int		sprite_h;
	unsigned int		sprite_dirs;
};


#endif
