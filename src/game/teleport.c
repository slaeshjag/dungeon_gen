#include "world.h"
#include "character.h"

void teleport_load() {
	DARNIT_FILE *f;
	void *data;
	unsigned int teleports;

	ws.char_data->teleport.entry = NULL;
	ws.char_data->teleport.entries = 0;

	if (!(f = d_file_open("world/teleports.dat", "rb"))) {
		fprintf(stderr, "Unable to load file world/teleports.dat\n");
		return;
	}

	d_file_read_ints(&teleports, 1, f);
	ws.char_data->teleport.entry = malloc(sizeof(*ws.char_data->teleport.entry));
	ws.char_data->teleport.entries = teleports;
	d_file_read_ints(&teleports, 1, f);

	data = malloc(teleports);
	d_file_read(data, teleports, f);
	d_util_decompress(data, teleports, &ws.char_data->teleport.entry);
	free(data);
	d_util_endian_convert((void *) ws.char_data->teleport.entry, ws.char_data->teleport.entries);

	return;
}


void teleport_unload() {
	free(ws.char_data->teleport.entry);
	ws.char_data->teleport.entry = NULL;
	ws.char_data->teleport.entries = 0;

	return;
}
