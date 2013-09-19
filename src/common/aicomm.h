#ifndef __AICOMM_H__
#define	__AICOMM_H__

#include <darnit/darnit.h>

enum aicomm_msg {
	AICOMM_MSG_INIT,	/* Initialize! */
	AICOMM_MSG_LOOP,	/* Frame update */
	AICOMM_MSG_COLL,	/* Collision notification */
	AICOMM_MSG_SEND,	/* Pass this on */
	AICOMM_MSG_INVM,	/* Invalid message */
	AICOMM_MSG_DIRU,	/* Update sprite */
	AICOMM_MSG_DONE,
	AICOMM_MSG_NOAI,
	AICOMM_MSG_NEXT,	/* Next message! */
	AICOMM_MSG_FOLM,	/* Follow me! */
	AICOMM_MSG_CAMN,	/* Make camera jump to followed char */
	AICOMM_MSG_GETP,	/* Get player entry */
	AICOMM_MSG_SETP,	/* Set player entry */
	AICOMM_MSG_TELP,	/* Teleport me! */
	AICOMM_MSG_TPME,	/* Teleport character using 
					teleport table */
	AICOMM_MSG_KILL,	/* Kill a character */
	AICOMM_MSG_SPWN,	/* Spawn a character */
	AICOMM_MSG_GETF,	/* Get character I'm facing */
	AICOMM_MSG_MAPE,	/* Character triggered map event */
	AICOMM_MSG_DESTROY,	/* Time to die! */

	AICOMM_MSG_END		/* END OF LIST */
};


struct aicomm_struct {
	enum aicomm_msg		msg;
	int			arg[8];
	void			*argp;
	int			from;
	struct character_entry	**ce;
	int			self;
};


struct character_save {
	unsigned int		*i;
	unsigned int		is;
	char			*b;
	unsigned int		bs;
};


struct character_entry {
	struct aicomm_struct	(*loop)(struct aicomm_struct ac);
	/* X and Y is fix-point 24.8 */
	int			x;
	int			y;
	int			l;
	int			dir;
	int			slot;
	int			self;

	/* In fix-pint 24.8 pixels/s */
	int			dx;
	int			dy;

	struct {
		unsigned int	pushing		: 1;
		unsigned int	jumping		: 1;
		unsigned int	walking		: 1;
		unsigned int	animate		: 1;
		unsigned int	solid		: 1;
		unsigned int	invisible	: 1;
	} special_action;

	DARNIT_SPRITE		*sprite;
	void			*state;
	struct character_save	save;
	char			ai[32];
};

#endif
