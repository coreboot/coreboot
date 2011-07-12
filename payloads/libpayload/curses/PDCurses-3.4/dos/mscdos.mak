# NMAKE Makefile for PDCurses library - Microsoft C for DOS
#
# Usage: nmake -f [path\]dosmsc.mak [DEBUG=] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

# Change the memory MODEL here, if desired
MODEL	= L       # one of L, S, M, T, H, C
SIZE	= LARGE   # one of LARGE, SMALL, MEDIUM, TINY, HUGE, COMPACT

O = obj

!ifndef PDCURSES_SRCDIR
PDCURSES_SRCDIR = ..
!endif

!include $(PDCURSES_SRCDIR)\version.mif
!include $(PDCURSES_SRCDIR)\libobjs.mif

osdir		= $(PDCURSES_SRCDIR)\dos

PDCURSES_DOS_H	= $(osdir)\pdcdos.h

CC		= cl

!ifdef DEBUG
CFLAGS		= /Od /Zi /DPDCDEBUG
LDFLAGS		= /CO /NOE /SE:160 
!else
CFLAGS		= /Ox
LDFLAGS		= /NOE /SE:160
!endif

CPPFLAGS	= -I$(PDCURSES_SRCDIR)

BUILD		= $(CC) /J /nologo /c /D$(SIZE) /A$(MODEL) $(CFLAGS) $(CPPFLAGS)

LINK		= link

CCLIBS		=

LIBEXE		= lib

LIBCURSES	= pdcurses.lib

all:	$(LIBCURSES) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe

demos:	$(DEMOS)

DEMOOBJS = $(DEMOS:.exe=.obj) tui.obj

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(DEMOS) : $(LIBCURSES)
panel.obj : $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @$(osdir)\mscdos.lrf
	-copy $(LIBCURSES) panel.lib

{$(srcdir)\}.c{}.obj:
	$(BUILD) $<

{$(osdir)\}.c{}.obj:
	$(BUILD) $<

{$(demodir)\}.c{}.obj:
	$(BUILD) $<

firework.exe: firework.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

newdemo.exe: newdemo.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

ptest.exe: ptest.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

rain.exe: rain.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

testcurs.exe: testcurs.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

tuidemo.exe: tuidemo.obj tui.obj
	$(LINK) $(LDFLAGS) $*.obj+tui.obj,$*,,$(LIBCURSES);

worm.exe: worm.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

xmas.exe: xmas.obj
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

ptest.obj: $(demodir)\ptest.c $(PANEL_HEADER)
	$(BUILD) $(demodir)\ptest.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h
	$(BUILD) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c
	$(BUILD) -I$(demodir) $(demodir)\tuidemo.c

PLATFORM1 = Microsoft C
PLATFORM2 = Microsoft C for DOS
ARCNAME = pdc$(VER)msc

!include $(PDCURSES_SRCDIR)\makedist.mif
