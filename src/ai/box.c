#include "aicommon.h"
#include "aicomm.h"
#include <darnit/darnit.h>


struct aicomm_struct box_ai(struct aicomm_struct ac) {
	static struct textbox_properties tb;

	switch (ac.msg) {
		case AICOMM_MSG_INIT:
		case AICOMM_MSG_LOOP:
		case AICOMM_MSG_DESTROY:
		default:
			ac.from = ac.self;
			ac.msg = AICOMM_MSG_DONE;
			ac.ce[ac.self]->special_action.solid = 1;
			return ac;
		case AICOMM_MSG_SEND:
			ac.msg = AICOMM_MSG_TBOX;
			ac.from = ac.self;
			ac.arg[0] = ac.ce[ac.self]->slot;
			tb.message = "Boo?";
			tb.question = "Yes\nNo\nMaybe\nNever";
			ac.argp = &tb;
			return ac;
	}

	ac.from = ac.self;
	ac.msg = AICOMM_MSG_DONE;

	return ac;
}
