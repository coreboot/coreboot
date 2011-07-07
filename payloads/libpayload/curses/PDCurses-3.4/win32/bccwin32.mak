# Borland MAKE Makefile for PDCurses library - Win32 BC++ 4.0+
#
# Usage: make -f [path\]bccwin32.mak [DEBUG=] [WIDE=] [UTF8=] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

O = obj

!ifndef PDCURSES_SRCDIR
PDCURSES_SRCDIR = ..
!endif

!include $(PDCURSES_SRCDIR)\version.mif
!include $(PDCURSES_SRCDIR)\libobjs.mif

osdir		= $(PDCURSES_SRCDIR)\win32

CC		= bcc32 -q

!ifdef DEBUG
CFLAGS		= -N -v -y -DPDCDEBUG
!else
CFLAGS		= -O
!endif

!ifdef WIDE
WIDEOPT		= -DPDC_WIDE
!endif

!ifdef UTF8
UTF8OPT		= -DPDC_FORCE_UTF8
!endif

BUILD		= $(CC) -I$(PDCURSES_SRCDIR) -c -Tpe -w32 $(CFLAGS) -w-par \
$(WIDEOPT) $(UTF8OPT)

LIBEXE		= tlib /C /E /0 /a

LIBCURSES	= pdcurses.lib

all:	$(LIBCURSES) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.tds
	-del *.exe

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	-del $@
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)
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

PLATFORM1 = Borland C++ Win32
PLATFORM2 = Borland C/C++ 5.5 for Win32
ARCNAME = pdc$(VER)_bcc_w32

!include $(PDCURSES_SRCDIR)\makedist.mif
