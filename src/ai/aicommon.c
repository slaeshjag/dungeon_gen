#include "aicommon.h"

struct ai_msgbuf *aicom_msgbuf_new(int size) {
	struct ai_msgbuf *msg;

	msg = malloc(sizeof(*msg));
	msg->msg = malloc(sizeof(*msg->msg) * size);
	msg->max_msg = size;
	msg->cur_msg = 0;

	return msg;
}


void aicom_msgbuf_push(struct ai_msgbuf *msg, struct aicomm_struct ac) {
	if (msg->cur_msg == msg->max_msg)
		return;
	msg->msg[msg->cur_msg++] = ac;
	return;
}


struct aicomm_struct aicom_msgbuf_pop(struct ai_msgbuf *msg) {
	struct aicomm_struct ac;

	if (!msg->cur_msg) {
		ac.msg = AICOMM_MSG_DONE;
		return ac;
	}

	return msg->msg[--msg->cur_msg];
}


void *aicom_msgbuf_free(struct ai_msgbuf *msg) {
	if (!msg)
		return NULL;
	free(msg->msg);
	free(msg);
	return NULL;
}
