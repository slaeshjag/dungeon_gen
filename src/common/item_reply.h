#ifndef __ITEM_REPLY_H__
#define	__ITEM_REPLY_H__

enum ITEM_SPREAD_TYPE {
	ITEM_SPREAD_TYPE_NONE,
	ITEM_SPREAD_TYPE_LINEAR,
	ITEM_SPREAD_TYPE_LOG,
};


struct item_reply_element {
	char			stat[32];
	int			diff;
	int			turns;
};


struct item_reply {
	struct item_reply_element	*src;
	int				srcs;
	enum ITEM_SPREAD_TYPE		srcp;
	/* How far it spreads */
	int				srcn;

	struct item_reply_element	*dst;
	int				dsts;
	enum ITEM_SPREAD_TYPE		dstp;
	/* How far it spreads */
	int				dstn;
	
	int				inventory_change;
};

#endif
