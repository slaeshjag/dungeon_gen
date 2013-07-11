#ifndef __UTIL_H__
#define __UTIL_H__

void util_blt(unsigned int *dest, int dw, int dh, int dx, int dy, unsigned int *src, int sw, int sh, int sx, int sy);
void util_order_randomize(int *order, int count);

#endif
