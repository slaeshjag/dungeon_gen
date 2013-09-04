#ifndef __CHARACTER_SAVE_DATA_H__
#define	__CHARACTER_SAVE_DATA_H__


static int sprite_data_default[] = {
	/* Data in pairs, tile, time. Set tile to -1 for next dir,
		both to -1 for end of sprite */
	/* Left */
	1, 10,		0, 100,		1, 100,		2, 100,		1, 90,		-1, 0,
	/* Up */
	4, 10,		3, 100,		4, 100,		5, 100,		4, 90,		-1, 0,
	/* Right */
	7, 10,		6, 100,		7, 100,		8, 100,		7, 90,		-1, 0,
	/* Down */
	10, 10,		9, 100,		10, 100,	11, 100,	10, 90,		-1, 0,
	/* Terminate */
	-1, -1,
};


static int sprite_hitbox_default[] = {
	/* Left */
	0,	16,	32,	32,
	/* Up */
	0,	16,	32,	32,
	/* Right */
	0,	16,	32,	32,
	/* Down */
	0,	16,	32,	32,
};


#endif
