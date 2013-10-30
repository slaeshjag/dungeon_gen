#ifndef __PLAYER_H__
#define	__PLAYER_H__

#include "aicommon.h"
#include "aicomm.h"

#define	PLAYER_SPEED 		(256 << 8)
#define	PLAYER_PROG_OFFSET	CHAR_STAT_TOTAL


struct player_state {
	unsigned int			init;
	struct ai_msgbuf		*msg;
	struct textbox_properties	tbp;
	int				freeze;
};

#endif
