# Project: dungeon_gen
include $(TOPDIR)/config.mk

LIBOUT		=	../world_gen.a
LIBOBJ		=	*.o
CFLAGS		+=	-I../common

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
	@echo " [ CC ]	world_gen/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
