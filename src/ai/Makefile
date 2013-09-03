# Project: dungeon_gen
include $(TOPDIR)/config.mk

SRCFILES	=	$(wildcard *.c)
OBJFILES	=	$(SRCFILES:.c=.o)
CFLAGS		+=	-I../common
.PHONY: all clean

all: $(OBJFILES) $(DEPENDS)
	@echo " [ LD ] $(LIB)"
	@$(CC) $(CFLAGS) $(OBJFILES) -o $(LIB) $(LDFLAGS)
	
clean:
	@echo
	@echo " [ RM ] $(OBJFILES)"
	@$(RM) $(OBJFILES)
	@echo "Done."
	@echo 

%.o: %.c %.h
	@echo " [ CC ] ai/$<"
	@$(CC) $(CFLAGS) -c -o $@ $<
	
