#include "dungeon_object.h"

int dungeon_object_graphics(int type, int subtype) {
	if (type == 0 && subtype == 0)
		return 0;
	/* TODO: Implement for real */
	return 0;
}


const char *dungeon_object_ai(int type, int subtype) {
	/* TODO: Implement for real */
	if (type == 0 && subtype == 0)
		return "ai_dummy";
	return "ai_dummy";
}
