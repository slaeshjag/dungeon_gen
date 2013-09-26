#ifndef __SAVEDATA_H__
#define	__SAVEDATA_H__


#define	SAVEDATA_MAGIC			0xD033A300
#define	LDI_MAGIC			0x83B3661B

struct savedata_header {
	unsigned int			magic;
	unsigned int			ints;
	unsigned int			bytes;
	unsigned int			intz;
	unsigned int			bytez;
	unsigned int			info;
	unsigned int			infoz;
};


struct savedata_footer {
	unsigned int			magic;
	unsigned int			offset;
};


struct savedata {
	unsigned int			*i;
	unsigned char			*b;
	unsigned int			is;
	unsigned int			bs;
	unsigned int			world;
};

void savedata_load(const char *fname);
void savedata_save(const char *file);

off_t savedata_present(const char *fname);

#endif
