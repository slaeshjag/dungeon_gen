#ifndef __AICOMMON_H__
#define	__AICOMMON_H__

#include "aicomm.h"

enum {
	CHAR_STAT_HP		= 0,
	CHAR_STAT_MP		= 1,
	CHAR_STAT_ATT		= 2,
	CHAR_STAT_DEF		= 3,
	CHAR_STAT_SPD		= 4,
} char_stat;

#define	CHAR_STAT_TOTAL		5

struct ai_msgbuf {
	struct aicomm_struct	*msg;
	int			max_msg;
	int			cur_msg;
};

struct ai_msgbuf *aicom_msgbuf_new(int size);
void aicom_msgbuf_push(struct ai_msgbuf *msg, struct aicomm_struct ac);
struct aicomm_struct aicom_msgbuf_pop(struct ai_msgbuf *msg);
void *aicom_msgbuf_free(struct ai_msgbuf *msg);


#endif
