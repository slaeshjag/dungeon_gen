## Project: dungeon_gen

include config.mk
MAKEFLAGS	+=	--no-print-directory
TOPDIR		=	$(shell pwd)
export TOPDIR

default:
	@mkdir -p bin/
	@echo " [ CD ] src/"
	+@make -C src/
	@echo
	@echo "*** Build complete ***"

clean:
	@echo " [ CD ] src/"
	+@make -C src/ clean
	@$(RM) bin/
