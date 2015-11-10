# Borland MAKE Makefile for PDCurses library - OS/2 BC++ 1.0+
#
# Usage: make -f [path\]bccos2.mak [DEBUG=] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

O = obj

!ifndef PDCURSES_SRCDIR
PDCURSES_SRCDIR = ..
!endif

!include $(PDCURSES_SRCDIR)\version.mif
!include $(PDCURSES_SRCDIR)\libobjs.mif

osdir		= $(PDCURSES_SRCDIR)\os2

CC		= bcc

!ifdef DEBUG
CFLAGS		= -N -v -y -DPDCDEBUG 
!else
CFLAGS		= -O 
!endif

CPPFLAGS	= -I$(PDCURSES_SRCDIR)

BUILD		= $(CC) -c $(CFLAGS) $(CPPFLAGS)

LINK		= tlink

LIBEXE		= tlib /C /E

LIBCURSES	= pdcurses.lib

all:	$(LIBCURSES) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe

demos:	$(DEMOS)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	-del $@
	$(LIBEXE) $@ \
+addch.obj +addchstr.obj +addstr.obj +attr.obj +beep.obj +bkgd.obj \
+border.obj +clear.obj +color.obj +delch.obj +deleteln.obj +deprec.obj \
+getch.obj +getstr.obj +getyx.obj +inch.obj +inchstr.obj +initscr.obj \
+inopts.obj +insch.obj +insstr.obj +instr.obj +kernel.obj +keyname.obj \
+mouse.obj +move.obj +outopts.obj +overlay.obj +pad.obj +panel.obj \
+printw.obj +refresh.obj +scanw.obj +scr_dump.obj +scroll.obj +slk.obj \
+termattr.obj +terminfo.obj +touch.obj +util.obj +window.obj +debug.obj \
+pdcclip.obj +pdcdisp.obj +pdcgetsc.obj +pdckbd.obj +pdcscrn.obj \
+pdcsetsc.obj +pdcutil.obj ,lib.map
	-copy $(LIBCURSES) panel.lib

.autodepend

{$(srcdir)\}.c.obj:
	$(BUILD) $<

{$(osdir)\}.c.obj:
	$(BUILD) $<

{$(demodir)\}.c.obj:
	$(BUILD) $<

.c.obj:
	$(BUILD) $<

.obj.exe:
	$(CC) -e$@ $** $(LIBCURSES)

tuidemo.exe:	tuidemo.obj tui.obj $(LIBCURSES)
	$(CC) -e$@ $**

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(BUILD) -I$(demodir) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(BUILD) -I$(demodir) $(demodir)\tuidemo.c

PLATFORM1 = Borland C++ OS/2 1.0
PLATFORM2 = Borland C/C++ OS/2 1.0
ARCNAME = pdc$(VER)bcos2

!include $(PDCURSES_SRCDIR)\makedist.mif
