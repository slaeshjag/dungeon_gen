#include "engine_api.h"
#include <string.h>

void engine_api_send(struct ai_msgbuf *buf, int from, int to, void *data, int arg[8], int args) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_SEND;
	ac.argp = data;
	memcpy(ac.arg, arg, 4 * args);
	ac.from = from;
	ac.self = to;
	
	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_invalid_msg(struct ai_msgbuf *buf, int from, int to) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_INVM;
	ac.from = from;
	ac.self = to;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_direction_update(struct ai_msgbuf *buf, int chr) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_DIRU;
	ac.from = chr;
	
	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_replace_sprite(struct ai_msgbuf *buf, int chr, int sprite) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_NSPR;
	ac.from = chr;
	ac.arg[0] = sprite;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_next_message(struct ai_msgbuf *buf, int from, int to) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_NEXT;
	ac.from = from;
	ac.self = to;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_follow_me(struct ai_msgbuf *buf, int chr) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_FOLM;
	ac.from = chr;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_request_player(struct ai_msgbuf *buf, int me) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_GETP;
	ac.from = me;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_set_player(struct ai_msgbuf *buf, int player, int from) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_SETP;
	ac.from = from;
	ac.self = player;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_teleport(struct ai_msgbuf *buf, int me, int map, int tile_x, int tile_y, int layer) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_TELP;
	ac.from = me;
	ac.arg[0] = map;
	ac.arg[1] = tile_x;
	ac.arg[2] = tile_y;
	ac.arg[3] = layer;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_teleport_table(struct ai_msgbuf *buf, int me, int entry) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_TPME;
	ac.from = me;
	ac.arg[0] = entry;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_kill(struct ai_msgbuf *buf, int kill) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_KILL;
	ac.from = kill;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_spawn(struct ai_msgbuf *buf, int me, int gfx_slot, int tile_x, int tile_y, int layer, const char *ai_func) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_SPWN;
	ac.from = me;
	ac.arg[0] = gfx_slot;
	ac.arg[1] = tile_x;
	ac.arg[2] = tile_y;
	ac.arg[3] = layer;
	ac.argp = (void *) ai_func;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_request_faced(struct ai_msgbuf *buf, int me) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_GETF;
	ac.from = me;
	
	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_camera_jump(struct ai_msgbuf *buf, int me) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_CAMN;
	ac.from = me;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_text_effect(struct ai_msgbuf *buf, int me, int time, int x, int y, int linel, int r, int g, int b, const char *str) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_TXTE;
	ac.from = me;
	ac.argp = (void *) str;
	ac.arg[0] = time;
	ac.arg[1] = x;
	ac.arg[2] = y;
	ac.arg[3] = linel;
	ac.arg[4] = r;
	ac.arg[5] = g;
	ac.arg[6] = b;
	
	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_textbox(struct ai_msgbuf *buf, struct textbox_properties *tbp, int from, int face) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_TBOX;
	ac.from = from;
	ac.arg[0] = face;
	ac.argp = tbp;

	aicom_msgbuf_push(buf, ac);
	return;
}

void engine_api_request_preload(struct ai_msgbuf *buf, const char *fname, enum character_resource type, int tile_w, int tile_h, int from) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_PREL;
	ac.from = from;
	ac.argp = (char *) fname;
	ac.arg[0] = type;
	ac.arg[1] = tile_w;
	ac.arg[2] = tile_h;

	aicom_msgbuf_push(buf, ac);
	return;
}


void engine_api_preload_unload(struct ai_msgbuf *buf, const char *fname, int from) {
	struct aicomm_struct ac;

	ac.msg = AICOMM_MSG_UNLO;
	ac.from = from;
	ac.argp = (char *) fname;

	aicom_msgbuf_push(buf, ac);
	return;
}
