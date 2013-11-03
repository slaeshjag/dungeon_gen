#ifndef __ENGINE_API_H__
#define	__ENGINE_API_H__


#include "aicommon.h"

void engine_api_send(struct ai_msgbuf *buf, int from, int to, void *data, int arg[8], int args);
void engine_api_invalid_msg(struct ai_msgbuf *buf, int from, int to);
void engine_api_direction_update(struct ai_msgbuf *buf, int chr);
void engine_api_replace_sprite(struct ai_msgbuf *buf, int chr, int sprite);
void engine_api_next_message(struct ai_msgbuf *buf, int from, int to);
void engine_api_follow_me(struct ai_msgbuf *buf, int chr);
void engine_api_request_player(struct ai_msgbuf *buf, int me);
void engine_api_set_player(struct ai_msgbuf *buf, int player, int from);
void engine_api_teleport(struct ai_msgbuf *buf, int me, int map, int tile_x, int tile_y, int layer);
void engine_api_teleport_table(struct ai_msgbuf *buf, int me, int entry);
void engine_api_kill(struct ai_msgbuf *buf, int kill);
void engine_api_spawn(struct ai_msgbuf *buf, int me, int gfx_slot, int tile_x, int tile_y, int layer, const char *ai_func);
void engine_api_request_faced(struct ai_msgbuf *buf, int me);
void engine_api_camera_jump(struct ai_msgbuf *buf, int me);
void engine_api_text_effect(struct ai_msgbuf *buf, int me, int time, int x, int y, int linel, int r, int g, int b, const char *str);
void engine_api_textbox(struct ai_msgbuf *buf, struct textbox_properties *tbp, int from, int face);


#endif
