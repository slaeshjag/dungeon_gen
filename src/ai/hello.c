#include "aicomm.h"


struct aicomm_struct ai_dummy(struct aicomm_struct ac) {
	ac.ce[ac.self]->dx = 1024;
	ac.ce[ac.self]->dir = 2;
	ac.ce[ac.self]->special_action.animate = 1;
	ac.msg = AICOMM_MSG_DONE;

	return ac;
}
