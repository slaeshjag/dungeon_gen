# Project: dungeon_gen
include $(TOPDIR)/config.mk

LIBOUT		=	../common.a
LIBOBJ		=	*.o

ifeq ($(strip $(OS)), Windows_NT)
	DEPENDS =
endif

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
.PHONY: all clean

all: $(OBJFILES)
	@echo " [ AR ] $(LIBOUT)"
	@$(AR) -cr $(LIBOUT) $(OBJFILES) $(LIBOBJ)

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo " [ RM ] $(LIBOUT)"
	@$(RM) $(LIBOUT)
	@echo ""

%.o: %.c %.h
	@echo " [ CC ]	common/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
