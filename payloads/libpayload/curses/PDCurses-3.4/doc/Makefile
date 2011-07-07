# Makefile for PDCurses manext program.

all: manual

manual: PDCurses.txt

PDCurses.txt: manext
	cat intro.txt > PDCurses.txt
	echo PDCurses Definitions and Variables >> PDCurses.txt
	echo ================================== >> PDCurses.txt
	./manext ../curses.h >> PDCurses.txt
	echo PDCurses Functions >> PDCurses.txt
	echo ================== >> PDCurses.txt
	./manext ../pdcurses/*.c >> PDCurses.txt
	./manext ../x11/*.c >> PDCurses.txt
	cat x11.txt >> PDCurses.txt
	echo >> PDCurses.txt
	echo >> PDCurses.txt
	echo \
-------------------------------------------------------------------------- \
>> PDCurses.txt
	echo >> PDCurses.txt
	cat sdl.txt >> PDCurses.txt

manext: manext.c

install:
	echo Does nothing at the moment

clean:
	-rm -rf *.o manext PDCurses.txt

distclean: clean

mostlyclean: clean

realclean: distclean
