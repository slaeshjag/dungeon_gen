#ifndef __AICOMM_H__
#define	__AICOMM_H__

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
	AICOMM_MSG_GETP,	/* Get player entry */
	AICOMM_MSG_SETP,	/* Set player entry */
	AICOMM_MSG_TELP,	/* Teleport me! */
	AICOMM_MSG_DESTROY,	/* Time to die! */
};


struct aicomm_struct {
	enum aicomm_msg		msg;
	int			arg[8];
	void			*argp;
	int			from;
	struct character_entry	**ce;
	int			self;
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

	struct {
		unsigned int	pushing		: 1;
		unsigned int	jumping		: 1;
		unsigned int	walking		: 1;
		unsigned int	animate		: 1;
		unsigned int	invisible	: 1;
	} special_action;

	DARNIT_SPRITE		*sprite;
};

#endif
