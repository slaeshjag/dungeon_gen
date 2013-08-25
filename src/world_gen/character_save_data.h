#ifndef __CHARACTER_SAVE_DATA_H__
#define	__CHARACTER_SAVE_DATA_H__


static int sprite_data[] = {
	/* Data in pairs, tile, time. Set tile to -1 for next dir,
		both to -1 for end of sprite */
	/* Down */
	1, 200,		0, 200,		1, 200,		2, 200,		-1, 0,
	/* Up */
	4, 200,		3, 200,		4, 200,		5, 200,		-1, 0,
	/* Left */
	7, 200,		6, 200,		7, 200,		8, 200,		-1, 0,
	/* Right */
	10, 200,	9, 100,		10, 200,	11, 200,	-1, 0,
	/* Terminate */
	-1, -1,
};


#endif