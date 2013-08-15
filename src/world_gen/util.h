#ifndef __UTIL_H__
#define __UTIL_H__

int util_dir_conv(int i, int dir, int w, int h);
void util_blt(unsigned int *dest, int dw, int dh, int dx, int dy, unsigned int *src, int sw, int sh, int sx, int sy);
void util_blt_trans(unsigned int *dest, int dw, int dh, int dx, int dy, unsigned int *src, int sw, int sh, unsigned int trans);
void util_order_randomize(int *order, int count);
int util_local_to_global_coord(int w, int room_w, int room_h, int room, int tile);

#endif
