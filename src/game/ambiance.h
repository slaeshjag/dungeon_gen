#ifndef __AMBIANCE_H__
#define	__AMBIANCE_H__

#include <darnit/darnit.h>

struct ambiance {
	/* Color filter */
	DARNIT_RECT		*rect;
	int			r, g, b, a;
	int			color_filter_active;
};


void ambiance_init();


#endif
