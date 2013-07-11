# Project: dungeon_gen (No idea if this will become something)
include $(TOPDIR)/config.mk

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
BIN = ../bin/dungeon_gen
.PHONY: all clean

all: $(OBJFILES) $(DEPENDS)
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) -o $(BIN) $(LDFLAGS)
	@echo " Done."
	@echo

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(OBJFILES)
	@$(RM) $(BIN)
	@echo "Done."
	@echo ""


%.o: %.c %.h
	@echo " [ CC ]	src/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 

