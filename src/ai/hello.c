#include "aicomm.h"


struct aicomm_struct ai_dummy(struct aicomm_struct ac) {
	fprintf(stderr, "Hello!\n");
	ac.msg = AICOMM_MSG_DONE;

	return ac;
}
