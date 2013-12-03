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
	AICOMM_MSG_NSPR,	/* Request new sprite */
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
	AICOMM_MSG_BOXR,	/* Return value from textbox */
	AICOMM_MSG_KILL,	/* Kill a character */
	AICOMM_MSG_SPWN,	/* Spawn a character */
	AICOMM_MSG_GETF,	/* Get character I'm facing */
	AICOMM_MSG_MAPE,	/* Character triggered map event */
	AICOMM_MSG_SILE,	/* Silence! Ignore input */
	AICOMM_MSG_TBOX,	/* Spawn a textbox */
	AICOMM_MSG_TXTE,	/* Add text scrolling */
	AICOMM_MSG_STAT,	/* Status changed */
	AICOMM_MSG_PREL,	/* Preload resource */
	AICOMM_MSG_UNLO,	/* Unload preloaded resource */
	AICOMM_MSG_DESTROY,	/* Time to die! */

	AICOMM_MSG_END		/* END OF LIST */
};


enum character_resource {
	CHARACTER_RES_TILESHEET,
	CHARACTER_RES_ANIMATION,
};


enum character_effect_t {
	CHARACTER_EFFECT_ANIMATION,
	CHARACTER_EFFECT_PARTICLE,
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


struct textbox_properties {
	const char		*message;
	const char		*question;
};


struct stat_list {
	const char		*name;
	unsigned int		cur;
	unsigned int		max;
};


struct char_preload {
	void			*resource;
	char			*name;
	enum character_resource	cr;
};


struct char_effect {
	void			*resource;
	enum character_effect_t	cet;
	int			loop;
};


struct character_entry {
	struct aicomm_struct	(*loop)(struct aicomm_struct ac);
	/* X and Y is fix-point 24.8 */
	int			x;
	int			y;
	int			l;
	int			map;
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

	/* Stats, implement! */
	struct stat_list	*stat;
	unsigned int		stats;
	

	DARNIT_SPRITE		*sprite;
	void			*state;
	struct character_save	save;
	char			ai[32];
	struct char_preload	*char_preload;
	int			char_preloads;
	struct char_effect	*char_effect;
	int			char_effects;

	struct item_reply_element	*item_reply;
	int			item_replies;
};

#endif
