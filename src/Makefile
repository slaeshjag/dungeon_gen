# Project: dungeon_gen (No idea if this will become something)
include $(TOPDIR)/config.mk

SRCFILES=$(wildcard *.c)
OBJFILES=$(SRCFILES:.c=.o)
BIN = ../bin/dungeon_gen.elf
GAMELIBS = game.a world_gen.a common.a
.PHONY: all clean world_gen common game

all: $(OBJFILES) $(DEPENDS) game
	@echo " [ LD ]	$(BIN)"
	@$(CC) $(OBJFILES) $(GAMELIBS) -o $(BIN) $(LDFLAGS)
	@echo " Done."
	@echo


game: world_gen
	@echo " [ CD ] game/"
	+@make -C game/

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
	@echo " [ CD ] game/"
	+@make -C game clean
	@echo "Done."
	@echo ""


%.o: %.c %.h
	@echo " [ CC ]	src/$<"
	@$(CC) $(CFLAGS) -c -o $@ $< 

