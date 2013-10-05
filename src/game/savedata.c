#include <darnit/darnit.h>
#include "savedata.h"
#include "world.h"

off_t savedata_present(const char *fname) {
	DARNIT_FILE *f;
	struct savedata_footer sf;
	struct savedata_header sh;
	off_t pos;

	if (!(f = d_file_open(fname, "rb")))
		return 0;
	d_file_seek(f, -8, SEEK_END);
	d_file_read_ints(&sf, 2, f);
	if (sf.magic != SAVEDATA_MAGIC)
		return d_file_close(f), 0;
	d_file_seek(f, -1 * (signed) sf.offset - 8, SEEK_END);
	pos = d_file_tell(f);
	d_file_read_ints(&sh, sizeof(sh) / 4, f);
	if (sh.magic != SAVEDATA_MAGIC)
		return d_file_close(f), 0;
	d_file_close(f);
	return pos;
}


void savedata_load(const char *fname) {
	struct savedata_header sh;
	DARNIT_FILE *f;
	void *data;
	off_t pos;

	if (!(pos = savedata_present(fname)))
		return;
	if (!(f = d_file_open(fname, "r+b")))
		return;
	d_file_seek(f, pos, SEEK_SET);
	d_file_read_ints(&sh, sizeof(sh) / 4, f);
	if (sh.ints > ws.savedata.is || sh.bytes > ws.savedata.bs) {
		fprintf(stderr, "ERROR!!! Savedata contains more data than specified\n");
		return;
	}

	data = malloc(sh.intz);
	d_file_read(data, sh.intz, f);
	d_util_decompress(data, sh.intz, (void **) &ws.savedata.i);
	d_util_endian_convert(ws.savedata.i, ws.savedata.is);

	data = realloc(data, sh.bytez);
	d_file_read(data, sh.bytez, f);
	d_util_decompress(data, sh.bytez, (void **) &ws.savedata.b);
	
	free(data);

	return;
}


void savedata_save(const char *file) {
	off_t pos;
	struct savedata_header sh;
	struct savedata_footer sf;
	void *intz, *bytez;
	DARNIT_FILE *f;

	d_fs_unmount(file);
	if (!(f = d_file_open(file, "r+b"))) {
		fprintf(stderr, "Unable to save %s\n", file);
		return;
	}

	if ((pos = savedata_present(file)))
		d_file_size_set(f, pos);
	
	d_file_seek(f, 0, SEEK_END);
	sh.magic = SAVEDATA_MAGIC;
	sh.ints = ws.savedata.is;
	sh.bytes = ws.savedata.bs;
	sh.info = 0;
	sh.infoz = 0;

	d_util_endian_convert(ws.savedata.i, ws.savedata.is);

	if (ws.savedata.is) {
		sh.intz = d_util_compress(ws.savedata.i, ws.savedata.is * 4, &intz);
	} else
		sh.intz = 0;
	if (ws.savedata.bs) {
		sh.bytez = d_util_compress(ws.savedata.b, ws.savedata.bs, &bytez);
	} else
		sh.bytez = 0;

	d_util_endian_convert(ws.savedata.i, ws.savedata.is);

	d_file_write_ints(&sh, sizeof(sh) / 4, f);
	if (sh.intz) {
		d_file_write(intz, sh.intz, f);
		free(intz);
	}
	if (sh.bytez) {
		d_file_write(bytez, sh.bytez, f);
		free(bytez);
	}

	sf.magic = SAVEDATA_MAGIC;
	sf.offset = d_file_tell(f) - pos;
	d_file_write_ints(&sf, 2, f);

	d_file_close(f);
	d_fs_mount(file);

	return;
}
