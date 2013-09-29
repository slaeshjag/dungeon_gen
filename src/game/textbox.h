#ifndef __TEXTBOX_H__
#define	__TEXTBOX_H__

#include <darnit/darnit.h>

struct textbox {
	char			*message;
	unsigned int		char_pos;
	unsigned int		row;
	unsigned int		rows;
	char			*option;
	unsigned int		options;
	DARNIT_TILE		*face;
	DARNIT_TEXT_SURFACE	*text;
	unsigned int		surface_w;
};


#endif
