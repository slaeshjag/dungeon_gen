#ifndef __TEXTEFFECTS_H__
#define	__TEXTEFFECTS_H__

#define	TEXTEFFECT_FONT_SIZE	16
#define	TEXTEFFECT_EXPAND_SIZE	16

struct texteffect_e {
	DARNIT_TEXT_SURFACE	*s;
	int			time_left;
	int			total_time;
};


struct texteffect {
	struct texteffect_e	*te;
	unsigned int		max_te;
	unsigned int		te_used;
	
	DARNIT_FONT		*font;
};

void texteffect_init();
void texteffect_done();
int texteffect_add(const char *str, int time, int x, int y, int linel, unsigned r, unsigned g, unsigned b);
void texteffect_del(int id);
void texteffect_loop();


#endif
