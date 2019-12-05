NAME ?= MINESCE
COMPRESSED ?= NO
DESCRIPTION ?= "Minesweeper"

include $(CEDEV)/include/.makefile

compile_flags.txt:
	echo -I$(CEDEV)/include > $@

cemu: all
	cemu --send bin/$(NAME).8xp
