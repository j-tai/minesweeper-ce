NAME ?= MINESCE
ARCHIVED ?= YES
COMPRESSED ?= NO
DESCRIPTION ?= "Minesweeper"

include $(CEDEV)/include/.makefile

compile_flags.txt:
	echo -I$(CEDEV)/include > $@

cemu: all
	cemu --send bin/$(NAME).8xp

cemu-debug: debug
	cemu --send bin/$(NAME).8xp
