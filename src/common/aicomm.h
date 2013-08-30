#ifndef __AICOMM_H__
#define	__AICOMM_H__

enum aicomm_msg {
	AICOMM_MSG_INIT,
	AICOMM_MSG_LOOP,
	AICOMM_MSG_COLL,
	AICOMM_MSG_SEND,
	AICOMM_MSG_INCO,
	AICOMM_MSG_DIRU,
	AICOMM_MSG_DONE,
	AICOMM_MSG_NOAI,
	AICOMM_MSG_DESTROY,
};


struct aicomm_struct {
	enum aicomm_msg		msg;
	int			arg1;
	int			arg2;
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

	struct {
		unsigned int	pushing		: 1;
		unsigned int	jumping		: 1;
	} special_action;

	DARNIT_SPRITE		*sprite;
};

#endif
