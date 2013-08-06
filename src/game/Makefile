# Project: dungeon_gen
include $(TOPDIR)/config.mk

LIBOUT		=	../game.a
LIBOBJ		=	*.o

ifeq ($(strip $(OS)), Windows_NT)
	DEPENDS =
endif

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
CFLAGS		+=	-I../ -I../common
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
	@echo " [ CC ]	game/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 
