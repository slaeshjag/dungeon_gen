# Project: dungeon_gen (No idea if this will become something)
include $(TOPDIR)/config.mk

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
BIN = ../bin/dungeon_gen.elf
.PHONY: all clean world_gen common

all: $(OBJFILES) $(DEPENDS) world_gen
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) -o $(BIN) $(LDFLAGS)
	@echo " Done."
	@echo

world_gen: common
	@echo " [ CD ] world_gen/"
	+@make -C world_gen/

common:
	@echo " [ CD ] common/"
	+@make -C common

clean:
	@echo " [ RM ]	$(OBJFILES)"
	@echo " [ RM ]	$(BIN)"
	@$(RM) $(OBJFILES)
	@$(RM) $(BIN)
	@echo " [ CD ] world_gen/"
	+@make -C world_gen clean
	@echo " [ CD ] common/"
	+@make -C common clean
	@echo "Done."
	@echo ""


%.o: %.c %.h
	@echo " [ CC ]	src/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 

