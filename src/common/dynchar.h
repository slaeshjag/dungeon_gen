#ifndef __DYNCHAR_H__
#define	__DYNCHAR_H__


struct dynchar {
	char			*buff;
	unsigned int		size;
};

struct dynchar *dynchar_new();
void dynchar_append(struct dynchar *dc, const char *str);
void *dynchar_free(struct dynchar *dc);

#endif
