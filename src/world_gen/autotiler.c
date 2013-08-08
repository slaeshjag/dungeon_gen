#include "autotiler.h"
#include "util.h"

struct autotile *autotile_new(DARNIT_STRINGTABLE *st) {
	struct autotile *at;
	char name[32], mode;
	int rules, from, to, i, b;

	at = malloc(sizeof(*at));
	at->tiles = atoi(d_stringtable_entry(st, "TILES"));
	at->tile = malloc(sizeof(*at->tile) * at->tiles);
	rules = atoi(d_stringtable_entry(st, "RULES"));
	
	for (i = 0; i < at->tiles; i++) {
		at->tile[i].borders = 0;
		at->tile[i].border = NULL;
	}

	for (i = 0; i < rules; i++) {
		mode = from = to = b = 0;
		sprintf(name, "RULE_%i", i);
		sscanf(d_stringtable_entry(st, name), "%i %i%c", &from, &to, &mode);
		b = at->tile[from].borders++;
		at->tile[from].border = realloc(at->tile[from].border, at->tile[from].borders * sizeof(*at->tile[from].border));
		at->tile[from].border[b].neighbour = from;
		at->tile[from].border[b].start = to;
		at->tile[from].border[b].mode = (mode == 'x') ? 1 : 0;
	}

	return at;
}


void *autotile_destroy(struct autotile *at) {
	int i;

	for (i = 0; i < at->tiles; i++)
		free(at->tile[i].border);
	free(at->tile);
	free(at);

	return NULL;
}


static int autotile_lookup(struct autotile *at, unsigned int tile, int neighbour, int mask) {
	int i;

	for (i = 0; i < at->tile[tile].borders; i++)
		if (at->tile[tile].border[i].neighbour == neighbour)
			return at->tile[tile].border[i].start + (at->tile[tile].border[i].mode) ? mask : 0;
	
	if (neighbour < 0)
		return tile;
	return autotile_lookup(at, tile, -1, mask);
}


void autotile_run(struct autotile *at, unsigned int *src, unsigned int *dst, const int w, const int h) {
	int i, j, k, l, m;

	for (i = 0; i < w * h; i++) {
		l = (src[i] & 0xFFF);
		for (j = m = 0; j < 4; j++) {
			if ((k = util_dir_conv(i, j, w, h)) == i)
				continue;
			if ((src[i] & 0xFFF) != (src[j] & 0xFFF) && src[j] > 0) {
				m |= (1 << j);
				l = (src[j] & 0xFFF);
			}
		}

		if (m && (unsigned int) l == (src[i] & 0xFFF))
			l = -1;
		dst[i] = autotile_lookup(at, (src[i] & 0xFFF), l, m);
	}

	return;
}
