# NMAKE Makefile for PDCurses library - OS/2 C Set/2
#
# Usage: nmake -f [path\]iccos2.mak [DEBUG=] [target]
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

PDCURSES_OS2_H	= $(osdir)\pdcos2.h

CC		= icc

!ifdef DEBUG
CFLAGS  	= /Sm /Ti+ /O- /Q+ /dPDCDEBUG
LDFLAGS 	= /NOLOGO /NOE /SE:160 /DEBUG /PMTYPE:VIO
!else
CFLAGS  	= /Sm /Ti- /O+ /Q+
LDFLAGS 	= /NOLOGO /NOE /EXEPACK /PACKCODE /PACKDATA /PMTYPE:VIO
!endif

CPPFLAGS	= -I$(PDCURSES_SRCDIR)

BUILD		= $(CC) -c $(CFLAGS) $(CPPFLAGS)

LINK		= link386

LIBEXE		= lib

LIBCURSES	= pdcurses.lib

all:	$(LIBCURSES) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe

demos:	$(DEMOS)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @$(osdir)\iccos2.lrf
	-copy $(LIBCURSES) panel.lib

addch.obj: $(srcdir)\addch.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\addch.c

addchstr.obj: $(srcdir)\addchstr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\addchstr.c

addstr.obj: $(srcdir)\addstr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\addstr.c

attr.obj: $(srcdir)\attr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\attr.c

beep.obj: $(srcdir)\beep.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\beep.c

bkgd.obj: $(srcdir)\bkgd.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\bkgd.c

border.obj: $(srcdir)\border.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\border.c

clear.obj: $(srcdir)\clear.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\clear.c

color.obj: $(srcdir)\color.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\color.c

delch.obj: $(srcdir)\delch.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\delch.c

deleteln.obj: $(srcdir)\deleteln.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\deleteln.c

deprec.obj: $(srcdir)\deprec.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\deprec.c

getch.obj: $(srcdir)\getch.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\getch.c

getstr.obj: $(srcdir)\getstr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\getstr.c

getyx.obj: $(srcdir)\getyx.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\getyx.c

inch.obj: $(srcdir)\inch.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\inch.c

inchstr.obj: $(srcdir)\inchstr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\inchstr.c

initscr.obj: $(srcdir)\initscr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\initscr.c

inopts.obj: $(srcdir)\inopts.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\inopts.c

insch.obj: $(srcdir)\insch.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\insch.c

insstr.obj: $(srcdir)\insstr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\insstr.c

instr.obj: $(srcdir)\instr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\instr.c

kernel.obj: $(srcdir)\kernel.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\kernel.c

keyname.obj: $(srcdir)\keyname.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\keyname.c

mouse.obj: $(srcdir)\mouse.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\mouse.c

move.obj: $(srcdir)\move.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\move.c

outopts.obj: $(srcdir)\outopts.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\outopts.c

overlay.obj: $(srcdir)\overlay.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\overlay.c

pad.obj: $(srcdir)\pad.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\pad.c

panel.obj: $(srcdir)\panel.c $(PDCURSES_HEADERS) $(PANEL_HEADER)
	$(BUILD) $(srcdir)\panel.c

printw.obj: $(srcdir)\printw.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\printw.c

refresh.obj: $(srcdir)\refresh.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\refresh.c

scanw.obj: $(srcdir)\scanw.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\scanw.c

scr_dump.obj: $(srcdir)\scr_dump.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\scr_dump.c

scroll.obj: $(srcdir)\scroll.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\scroll.c

slk.obj: $(srcdir)\slk.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\slk.c

termattr.obj: $(srcdir)\termattr.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\termattr.c

terminfo.obj: $(srcdir)\terminfo.c $(PDCURSES_HEADERS) $(TERM_HEADER)
	$(BUILD) $(srcdir)\terminfo.c

touch.obj: $(srcdir)\touch.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\touch.c

util.obj: $(srcdir)\util.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\util.c

window.obj: $(srcdir)\window.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\window.c

debug.obj: $(srcdir)\debug.c $(PDCURSES_HEADERS)
	$(BUILD) $(srcdir)\debug.c

pdcclip.obj: $(osdir)\pdcclip.c $(PDCURSES_HEADERS) $(PDCURSES_OS2_H)
	$(BUILD) $(osdir)\pdcclip.c

pdcdisp.obj: $(osdir)\pdcdisp.c $(PDCURSES_HEADERS) $(PDCURSES_OS2_H)
	$(BUILD) $(osdir)\pdcdisp.c

pdcgetsc.obj: $(osdir)\pdcgetsc.c $(PDCURSES_HEADERS) $(PDCURSES_OS2_H)
	$(BUILD) $(osdir)\pdcgetsc.c

pdckbd.obj: $(osdir)\pdckbd.c $(PDCURSES_HEADERS) $(PDCURSES_OS2_H)
	$(BUILD) $(osdir)\pdckbd.c

pdcscrn.obj: $(osdir)\pdcscrn.c $(PDCURSES_HEADERS) $(PDCURSES_OS2_H)
	$(BUILD) $(osdir)\pdcscrn.c

pdcsetsc.obj: $(osdir)\pdcsetsc.c $(PDCURSES_HEADERS) $(PDCURSES_OS2_H)
	$(BUILD) $(osdir)\pdcsetsc.c

pdcutil.obj: $(osdir)\pdcutil.c $(PDCURSES_HEADERS) $(PDCURSES_OS2_H)
	$(BUILD) $(osdir)\pdcutil.c

firework.exe: firework.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

newdemo.exe: newdemo.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

ptest.exe: ptest.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

rain.exe: rain.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

testcurs.exe: testcurs.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

tuidemo.exe: tuidemo.obj tui.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj+tui.obj,$*,,$(LIBCURSES);

worm.exe: worm.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

xmas.exe: xmas.obj $(LIBCURSES)
	$(LINK) $(LDFLAGS) $*.obj,$*,,$(LIBCURSES);

firework.obj: $(demodir)\firework.c $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\firework.c

newdemo.obj: $(demodir)\newdemo.c $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\newdemo.c

ptest.obj: $(demodir)\ptest.c $(PANEL_HEADER) $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\ptest.c

rain.obj: $(demodir)\rain.c $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\rain.c

testcurs.obj: $(demodir)\testcurs.c $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\testcurs.c

tui.obj: $(demodir)\tui.c $(demodir)\tui.h $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\tui.c

tuidemo.obj: $(demodir)\tuidemo.c $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\tuidemo.c

worm.obj: $(demodir)\worm.c $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\worm.c

xmas.obj: $(demodir)\xmas.c $(PDCURSES_CURSES_H)
	$(BUILD) $(demodir)\xmas.c

PLATFORM1 = C Set/2 OS/2
PLATFORM2 = C Set/2 for OS/2
ARCNAME = pdc$(VER)_icc_os2

!include $(PDCURSES_SRCDIR)\makedist.mif
