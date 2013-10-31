#include "aicomm.h"
#include "player.h"
#include "aicommon.h"
#include "savefile.h"


void player_init_stats(struct character_entry *ce) {
	ce->stat[CHAR_STAT_HP].cur = 10;
	ce->stat[CHAR_STAT_HP].max = 15;

}


static void player_init(struct aicomm_struct ac, struct player_state *ps) {
	int self = ac.self;

	ps->msg = aicom_msgbuf_new(32);
	ac.from = self;

	ac.msg = AICOMM_MSG_SETP;
	aicom_msgbuf_push(ps->msg, ac);

	ac.msg = AICOMM_MSG_FOLM;
	aicom_msgbuf_push(ps->msg, ac);

	ac.msg = AICOMM_MSG_CAMN;
	aicom_msgbuf_push(ps->msg, ac);
	
	ps->tbp.message = "Räksmörgås!\x01\x03  сыр вкуснее!\x01\x0F\x02\x10\nDet var allt.\x03\nFast egentligen inte, jag har mycket mer saker att säga, så här kommer du få jobba lite! Du vet, typ trycka vänster ALT några gånger i bland medans all den här jobbiga texten scrollar. Det här fungerar va? Annars blir jag ledsen i ögat.. :'(";
	ps->tbp.question = NULL;
	ac.msg = AICOMM_MSG_TBOX;
	ac.arg[0] = 0;
	ac.argp = &ps->tbp;
	aicom_msgbuf_push(ps->msg, ac);

	ac.ce[ac.self]->stat = malloc(sizeof(*ac.ce[ac.self]->stat) * CHAR_STAT_TOTAL);
	ac.ce[ac.self]->stats = CHAR_STAT_TOTAL - 1;
	player_init_stats(ac.ce[ac.self]);

	ac.ce[self]->special_action.solid = 1;

	return;
}


static void player_handle_send(struct aicomm_struct ac, struct player_state *ps) {
	int t;

	if (ac.self == ac.from)
		return;
	ac.arg[1] += PLAYER_PROG_OFFSET;

	/* Get progress */
	if (ac.arg[0] == 1) {
		if (ac.arg[1] >= (signed) ac.ce[ac.self]->save.is || ac.arg[1] < 0)
			ac.arg[0] = 0;
		else
			ac.arg[0] = ac.ce[ac.self]->save.i[ac.arg[1]];
	} else if (ac.arg[0] == 2) {	/* Set progress */
		if (ac.arg[1] >= (signed) ac.ce[ac.self]->save.is || ac.arg[1] < 0)
			ac.arg[0] = 0;
		else {
			ac.ce[ac.self]->save.i[ac.arg[1]] = ac.arg[2];
			ac.arg[0] = ac.arg[2];
		}
	} else 
		ac.arg[0] = 0;
	
	ac.arg[1] -= PLAYER_PROG_OFFSET;
	t = ac.self;
	ac.self = ac.from;
	ac.from = t;
	aicom_msgbuf_push(ps->msg, ac);

	return;
}


static void player_loop(struct aicomm_struct ac, struct player_state *ps) {
	int self = ac.self;
	int n;
	DARNIT_KEYS keys;

	keys = d_keys_get();

	ac.ce[self]->dx = ac.ce[self]->dy = 0;
	n = -1;
	if (ps->freeze)
		goto nomove;
	if (keys.left) {
		ac.ce[self]->dx = PLAYER_SPEED * -1;
		ac.ce[self]->dir = 0;
		n = 0;
	}
	
	if (keys.right) {
		ac.ce[self]->dx = PLAYER_SPEED;
		ac.ce[self]->dir = 2;
		n = 0;
	}
	
	if (keys.up) {
		ac.ce[self]->dir = 1;
		ac.ce[self]->dy = PLAYER_SPEED * -1;
		n = 0;
	}
	
	if (keys.down) {
		ac.ce[self]->dir = 3;
		ac.ce[self]->dy = PLAYER_SPEED;
		n = 0;
	}

	if (keys.select) {
		ac.msg = AICOMM_MSG_SPWN;
		ac.arg[0] = 0;
		ac.arg[1] = (ac.ce[ac.self]->x >> 8) / 32 - 2;
		ac.arg[2] = (ac.ce[ac.self]->y >> 8) / 32 - 2;
		ac.arg[3] = ac.ce[ac.self]->l;
		ac.argp = "box_ai";
		ac.from = ac.self;
		aicom_msgbuf_push(ps->msg, ac);
	}


	if (keys.y) {
		ac.msg = AICOMM_MSG_TXTE;
		ac.arg[0] = 2000;
		ac.arg[1] = (ac.ce[ac.self]->x >> 8) + 16;
		ac.arg[2] = (ac.ce[ac.self]->y >> 8) - 32;
		ac.arg[3] = 400;
		ac.arg[4] = 255;
		ac.arg[5] = 127;
		ac.arg[6] = 127;
		ac.argp = "Fiskmåsar i sjön\n+ 10 XP";
		ac.from = ac.self;
		aicom_msgbuf_push(ps->msg, ac);
		d_keys_set(d_keys_get());
	}

	if (keys.BUTTON_ACCEPT) {
		ac.msg = AICOMM_MSG_GETF;
		ac.from = ac.self;
		aicom_msgbuf_push(ps->msg, ac);
		d_keys_set(d_keys_get());
	}

	nomove:
	
	if (n < 0) {
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
	int n;

	if (ac.msg == AICOMM_MSG_INIT) {
		ac.ce[ac.self]->state = malloc(sizeof(struct player_state));
		ps = ac.ce[ac.self]->state;
		ps->init = 0;
		ps->freeze = 0;
		player_init(ac, ps);
	} else if (ac.msg == AICOMM_MSG_LOOP) {
		player_loop(ac, ac.ce[ac.self]->state);
	} else if (ac.msg == AICOMM_MSG_MAPE) {
		ps = ac.ce[ac.self]->state;
		if (ac.arg[1] & MAP_FLAG_TELEPORT) {
			ac.msg = AICOMM_MSG_TPME;
			ac.from = ac.self;
			ac.arg[0] = ((unsigned) ac.arg[1]) >> 14;
			aicom_msgbuf_push(ps->msg, ac);
		}
	} else if (ac.msg == AICOMM_MSG_SILE) {
		ps = ac.ce[ac.self]->state;
		ps->freeze = ac.arg[0];
		ac.from = ac.self;
	} else if (ac.msg == AICOMM_MSG_SEND) {
		player_handle_send(ac, ac.ce[ac.self]->state);
	} else if (ac.msg == AICOMM_MSG_GETF) {
		ps = ac.ce[ac.self]->state;
		n = ac.self;
		ac.self = ac.from;
		ac.from = n;
		ac.arg[0] = 0;
		ac.msg = AICOMM_MSG_SEND;
		aicom_msgbuf_push(ps->msg, ac);
		ac.self = n;
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


