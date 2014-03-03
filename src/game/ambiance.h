#ifndef __AMBIANCE_H__
#define	__AMBIANCE_H__

#include <darnit/darnit.h>


struct ambiance_flicker {
	DARNIT_RECT		*rect;
	int			r, g, b, a;
	int			flicker;
	int			flicker_offset;
	int			flicker_interval;
	/* Color filter */
	int			flicker_duration;
	int			filter_active;
};


struct ambiance {
	struct ambiance_flicker	flicker;
};


void ambiance_init();


#endif
