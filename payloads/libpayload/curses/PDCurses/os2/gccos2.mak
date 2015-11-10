# GNU MAKE Makefile for PDCurses library - OS/2 emx 0.9c+
#
# Usage: make -f [path\]gccos2.mak [DEBUG=Y] [EMXVIDEO=Y] [DLL=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.a|testcurs.exe...]
#
# The EMXVIDEO option compiles with the emx video library, which
# enables a PDCurses program to run under OS/2 and DOS.

O = o

ifndef PDCURSES_SRCDIR
	PDCURSES_SRCDIR = ..
endif

include $(PDCURSES_SRCDIR)/version.mif
include $(PDCURSES_SRCDIR)/libobjs.mif

osdir		= $(PDCURSES_SRCDIR)/os2

PDCURSES_OS2_H	= $(osdir)/pdcos2.h

CC		= gcc

CFLAGS = -I$(PDCURSES_SRCDIR) -c -Wall

ifeq ($(EMXVIDEO),Y)
	CFLAGS += -DEMXVIDEO
	CCLIBS = -lvideo
	BINDFLAGS = -acm
else
	CCLIBS =
	BINDFLAGS = 
endif

ifeq ($(DEBUG),Y)
	CFLAGS  += -g -DPDCDEBUG
	LDFLAGS = -g
else
	CFLAGS  += -O2
	LDFLAGS =
endif

BASEDEF		= $(PDCURSES_SRCDIR)\exp-base.def

DEFDEPS		= $(BASEDEF)

DEFFILE		= pdcurses.def

DLLTARGET	= pdcurses.dll
DLLFLAGS 	= -Zdll -Zcrtdll -Zomf

LINK		= gcc
EMXBIND		= emxbind 
EMXOMF		= emxomf

LIBEXE		= ar
LIBFLAGS	= rcv

ifeq ($(DLL),Y)
	CFLAGS += -Zdll -Zcrtdll -Zomf
	LDFLAGS += -Zlinker /PM:VIO -Zomf -Zcrtdll
	LIBCURSES = pdcurses.lib
	LIBDEPS = $(LIBOBJS) $(PDCOBJS) $(DEFFILE)
	PDCLIBS = $(DLLTARGET)
	EXEPOST =
	TUIPOST =
	CLEAN = *.dll *.lib $(DEFFILE)
else
	LIBCURSES = pdcurses.a
	LIBDEPS = $(LIBOBJS) $(PDCOBJS)
	PDCLIBS = $(LIBCURSES)
	EXEPOST = $(EMXBIND) $* $(BINDFLAGS)
	TUIPOST = $(EMXBIND) tuidemo $(BINDFLAGS)
	CLEAN = *.a testcurs newdemo xmas tuidemo firework ptest rain worm
endif

.PHONY: all libs clean demos dist

all:	libs demos

libs:	$(PDCLIBS)

clean:
	-del *.o
	-del *.exe
	-del $(CLEAN)

demos:	$(DEMOS)

DEMOOBJS = testcurs.o newdemo.o xmas.o tui.o tuidemo.o firework.o \
ptest.o rain.o worm.o

$(DEFFILE) : $(DEFDEPS)
	echo LIBRARY PDCURSES > $@
	echo DESCRIPTION 'PDCurses 3.4 Dynamic Linking library' >> $@
	echo PROTMODE >>$@
	echo DATA MULTIPLE READWRITE LOADONCALL >> $@
	echo CODE LOADONCALL >> $@
	echo EXPORTS >> $@
	type $(BASEDEF) >> $@

$(LIBCURSES) : $(LIBDEPS)
	$(LIBEXE) $(LIBFLAGS) $@ $?
	-copy $(LIBCURSES) panel.a

$(DLLTARGET) : $(LIBDEPS)
	$(LINK) $(DLLFLAGS) -o $(DLLTARGET) $? $(DEFFILE)
#	lxlite $(DLLTARGET)
	emximp -o $(LIBCURSES) $(DEFFILE)

$(LIBOBJS) $(PDCOBJS) $(DEMOOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_OS2_H)
$(DEMOS) : $(LIBCURSES)
panel.o ptest.o: $(PANEL_HEADER)
terminfo.o: $(TERM_HEADER)

$(LIBOBJS) : %.o: $(srcdir)/%.c
	$(CC) -c $(CFLAGS) -o$@ $<

$(PDCOBJS) : %.o: $(osdir)/%.c
	$(CC) -c $(CFLAGS) -o$@ $<

firework.exe newdemo.exe rain.exe testcurs.exe worm.exe xmas.exe \
ptest.exe: %.exe: %.o
	$(LINK) $(LDFLAGS) -o $* $< $(LIBCURSES) $(CCLIBS)
	$(EXEPOST)

tuidemo.exe:	tuidemo.o tui.o
	$(LINK) $(LDFLAGS) -o tuidemo tuidemo.o tui.o $(LIBCURSES) $(CCLIBS)
	$(TUIPOST)

firework.o newdemo.o ptest.o rain.o testcurs.o worm.o xmas.o: %.o: \
$(demodir)/%.c
	$(CC) $(CFLAGS) -o$@ $<

tui.o: $(demodir)\tui.c $(demodir)\tui.h
	$(CC) $(CFLAGS) -I$(demodir) -o $@ $<

tuidemo.o: $(demodir)\tuidemo.c
	$(CC) $(CFLAGS) -I$(demodir) -o $@ $<

PLATFORM1 = EMX OS/2
PLATFORM2 = EMX 0.9d for OS/2
ARCNAME = pdc$(VER)_emx_os2

include $(PDCURSES_SRCDIR)/makedist.mif
