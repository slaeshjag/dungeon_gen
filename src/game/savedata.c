#include <darnit/darnit.h>
#include "savedata.h"
#include "world.h"

int savedata_present(const char *fname) {
	DARNIT_FILE *f;
	struct savedata_footer sf;
	struct savedata_header sh;

	if (!(f = d_file_open(fname, "rb")))
		return 0;
	d_file_seek(f, -8, SEEK_END);
	d_file_read_ints(&sf, 2, f);
	if (sf.magic != SAVEDATA_MAGIC)
		return d_file_close(f), 0;
	d_file_seek(f, -1 * (signed) sf.offset - 8, SEEK_END);
	d_file_read_ints(&sh, sizeof(sh) / 4, f);
	if (sh.magic != SAVEDATA_MAGIC)
		return d_file_close(f), 0;
	d_file_close(f);
	return 1;
}


void savedata_load(const char *fname) {
	struct savedata_header sh;
	struct savedata_footer sf;
	DARNIT_FILE *f;
	void *data;

	if (!savedata_present(fname))
		return;
	if (!(f = d_file_open(fname, "r+")))
		return;
	d_file_seek(f, -8, SEEK_END);
	d_file_read_ints(&sf, 2, f);
	d_file_seek(f, -8 + ((signed) sf.offset) * -1, SEEK_END);
	d_file_read_ints(&sh, sizeof(sh) / 4, f);
	if (sh.ints > ws.savedata.is || sh.bytes > ws.savedata.bs) {
		fprintf(stderr, "ERROR!!! Savedata contains more data than specified\n");
		return;
	}

	data = malloc(sh.intz);
	d_file_read(data, sh.intz, f);
	d_util_decompress(data, sh.intz, ws.savedata.i);
	d_util_endian_convert(ws.savedata.i, ws.savedata.is);

	data = realloc(data, sh.bytez);
	d_file_read(data, sh.bytez, f);
	d_util_decompress(data, sh.bytez, ws.savedata.b);
	
	free(data);

	return;
}
