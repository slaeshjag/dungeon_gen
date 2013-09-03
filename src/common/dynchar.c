#include "dynchar.h"
#include <stdlib.h>
#include <string.h>

struct dynchar *dynchar_new() {
	struct dynchar *dc;

	dc = malloc(sizeof(*dc));
	dc->buff = NULL;
	dc->size = 0;

	return dc;
}


void dynchar_append(struct dynchar *dc, const char *str) {
	unsigned int pos;

	pos = dc->size - (dc->size ? 1 : 0);
	dc->size = pos + strlen(str) + 1;
	dc->buff = realloc(dc->buff, dc->size);
	strcpy(&dc->buff[pos], str);

	return;
}


void *dynchar_free(struct dynchar *dc) {
	free(dc->buff);
	free(dc);

	return 0;
}
