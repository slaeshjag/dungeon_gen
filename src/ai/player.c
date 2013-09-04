#include "aicomm.h"
#include "player.h"
#include "aicommon.h"


static void player_init(struct aicomm_struct ac, struct player_state *ps) {
	int self = ac.self;

	ps->msg = aicom_msgbuf_new(32);
	ac.from = self;

	ac.msg = AICOMM_MSG_SETP;
	aicom_msgbuf_push(ps->msg, ac);

	ac.msg = AICOMM_MSG_FOLM;
	aicom_msgbuf_push(ps->msg, ac);

	return;
}


static void player_loop(struct aicomm_struct ac, struct player_state *ps) {
	int self = ac.self;
	DARNIT_KEYS keys;

	keys = d_keys_get();

	ac.ce[self]->dx = ac.ce[self]->dy = 0;
	if (keys.left) {
		ac.ce[self]->dx = PLAYER_SPEED * -1;
		ac.ce[self]->dir = 0;
	} else if (keys.right) {
		ac.ce[self]->dx = PLAYER_SPEED;
		ac.ce[self]->dir = 2;
	} else if (keys.up) {
		ac.ce[self]->dir = 1;
		ac.ce[self]->dy = PLAYER_SPEED * -1;
	} else if (keys.down) {
		ac.ce[self]->dir = 3;
		ac.ce[self]->dy = PLAYER_SPEED;
	} else {
		if (ac.ce[self]->special_action.animate) {
			ac.ce[self]->special_action.animate = 0;
			ac.msg = AICOMM_MSG_DIRU;
			ac.from = self;
			aicom_msgbuf_push(ps->msg, ac);
			return;
		}
	}

	ac.ce[self]->special_action.animate = 1;

	return;
}


struct aicomm_struct player_ai(struct aicomm_struct ac) {
	struct player_state *ps;

	if (ac.msg == AICOMM_MSG_INIT) {
		ac.ce[ac.self]->state = malloc(sizeof(struct player_state));
		ps = ac.ce[ac.self]->state;
		ps->init = 0;
		player_init(ac, ps);
	} else if (ac.msg == AICOMM_MSG_LOOP) {
		player_loop(ac, ac.ce[ac.self]->state);
	} else if (ac.msg == AICOMM_MSG_DESTROY) {
		ps = ac.ce[ac.self]->state;
		aicom_msgbuf_free(ps->msg);
		free(ps);
		ac.msg = AICOMM_MSG_DONE;
		return ac;
	}

	ps = ac.ce[ac.self]->state;

	return aicom_msgbuf_pop(ps->msg);
}


