# Makefile for PDCurses library - WIN32 Digital Mars
#
# Usage: make -f dmcwin32.mak [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

PDCURSES_SRCDIR	= ..

PDCURSES_CURSES_H	= $(PDCURSES_SRCDIR)\curses.h
PDCURSES_CURSPRIV_H	= $(PDCURSES_SRCDIR)\curspriv.h
PDCURSES_HEADERS	= $(PDCURSES_CURSES_H) $(PDCURSES_CURSPRIV_H)

PANEL_HEADER	= $(PDCURSES_SRCDIR)\panel.h
TERM_HEADER	= $(PDCURSES_SRCDIR)\term.h

srcdir		= $(PDCURSES_SRCDIR)\pdcurses
osdir		= $(PDCURSES_SRCDIR)\win32
demodir		= $(PDCURSES_SRCDIR)\demos

PDCURSES_WIN_H	= $(osdir)\pdcwin.h

CC		= dmc

CFLAGS		= -c -o+space -Nc

CPPFLAGS	= -I$(PDCURSES_SRCDIR) #-DPDC_WIDE -DPDC_FORCE_UTF8

LINK		= dmc
LIBEXE		= lib

LIBCURSES	= pdcurses.lib

BUILD		= $(CC) $(CFLAGS) $(CPPFLAGS)

DEMOS		= testcurs.exe newdemo.exe xmas.exe tuidemo.exe \
firework.exe ptest.exe rain.exe worm.exe

all:    $(LIBCURSES) $(DEMOS)

clean:
	-del *.obj
	-del *.lib
	-del *.exe
	-del *.map
	-del advapi32.def

LIBOBJS = addch.obj addchstr.obj addstr.obj attr.obj beep.obj bkgd.obj \
border.obj clear.obj color.obj delch.obj deleteln.obj deprec.obj getch.obj \
getstr.obj getyx.obj inch.obj inchstr.obj initscr.obj inopts.obj \
insch.obj insstr.obj instr.obj kernel.obj keyname.obj mouse.obj move.obj \
outopts.obj overlay.obj pad.obj panel.obj printw.obj refresh.obj \
scanw.obj scr_dump.obj scroll.obj slk.obj termattr.obj terminfo.obj \
touch.obj util.obj window.obj debug.obj

PDCOBJS = pdcclip.obj pdcdisp.obj pdcgetsc.obj pdckbd.obj pdcscrn.obj \
pdcsetsc.obj pdcutil.obj

DEMOOBJS = testcurs.obj newdemo.obj xmas.obj tuidemo.obj tui.obj \
firework.obj ptest.obj rain.obj worm.obj

$(LIBOBJS) $(PDCOBJS) : $(PDCURSES_HEADERS)
$(PDCOBJS) : $(PDCURSES_WIN_H)
panel.obj ptest.obj: $(PANEL_HEADER)
terminfo.obj: $(TERM_HEADER)

$(DEMOOBJS) : $(PDCURSES_CURSES_H)
$(DEMOS) : $(LIBCURSES)

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) -c $@ $(LIBOBJS) $(PDCOBJS)
	-copy $(LIBCURSES) panel.lib

SRCBUILD = $(BUILD) $(srcdir)\$*.c
OSBUILD = $(BUILD) $(osdir)\$*.c
DEMOBUILD = $(LINK) $(CPPFLAGS) -o+space $@ $**

addch.obj: $(srcdir)\addch.c
	$(SRCBUILD)

addchstr.obj: $(srcdir)\addchstr.c
	$(SRCBUILD)

addstr.obj: $(srcdir)\addstr.c
	$(SRCBUILD)

attr.obj: $(srcdir)\attr.c
	$(SRCBUILD)

beep.obj: $(srcdir)\beep.c
	$(SRCBUILD)

bkgd.obj: $(srcdir)\bkgd.c
	$(SRCBUILD)

border.obj: $(srcdir)\border.c
	$(SRCBUILD)

clear.obj: $(srcdir)\clear.c
	$(SRCBUILD)

color.obj: $(srcdir)\color.c
	$(SRCBUILD)

delch.obj: $(srcdir)\delch.c
	$(SRCBUILD)

deleteln.obj: $(srcdir)\deleteln.c
	$(SRCBUILD)

deprec.obj: $(srcdir)\deprec.c
	$(SRCBUILD)

getch.obj: $(srcdir)\getch.c
	$(SRCBUILD)

getstr.obj: $(srcdir)\getstr.c
	$(SRCBUILD)

getyx.obj: $(srcdir)\getyx.c
	$(SRCBUILD)

inch.obj: $(srcdir)\inch.c
	$(SRCBUILD)

inchstr.obj: $(srcdir)\inchstr.c
	$(SRCBUILD)

initscr.obj: $(srcdir)\initscr.c
	$(SRCBUILD)

inopts.obj: $(srcdir)\inopts.c
	$(SRCBUILD)

insch.obj: $(srcdir)\insch.c
	$(SRCBUILD)

insstr.obj: $(srcdir)\insstr.c
	$(SRCBUILD)

instr.obj: $(srcdir)\instr.c
	$(SRCBUILD)

kernel.obj: $(srcdir)\kernel.c
	$(SRCBUILD)

keyname.obj: $(srcdir)\keyname.c
	$(SRCBUILD)

mouse.obj: $(srcdir)\mouse.c
	$(SRCBUILD)

move.obj: $(srcdir)\move.c
	$(SRCBUILD)

outopts.obj: $(srcdir)\outopts.c
	$(SRCBUILD)

overlay.obj: $(srcdir)\overlay.c
	$(SRCBUILD)

pad.obj: $(srcdir)\pad.c
	$(SRCBUILD)

panel.obj: $(srcdir)\panel.c
	$(SRCBUILD)

printw.obj: $(srcdir)\printw.c
	$(SRCBUILD)

refresh.obj: $(srcdir)\refresh.c
	$(SRCBUILD)

scanw.obj: $(srcdir)\scanw.c
	$(SRCBUILD)

scr_dump.obj: $(srcdir)\scr_dump.c
	$(SRCBUILD)

scroll.obj: $(srcdir)\scroll.c
	$(SRCBUILD)

slk.obj: $(srcdir)\slk.c
	$(SRCBUILD)

termattr.obj: $(srcdir)\termattr.c
	$(SRCBUILD)

terminfo.obj: $(srcdir)\terminfo.c
	$(SRCBUILD)

touch.obj: $(srcdir)\touch.c
	$(SRCBUILD)

util.obj: $(srcdir)\util.c
	$(SRCBUILD)

window.obj: $(srcdir)\window.c
	$(SRCBUILD)

debug.obj: $(srcdir)\debug.c
	$(SRCBUILD)

pdcclip.obj: $(osdir)\pdcclip.c
	$(OSBUILD)

pdcdisp.obj: $(osdir)\pdcdisp.c
	$(OSBUILD)

pdcgetsc.obj: $(osdir)\pdcgetsc.c
	$(OSBUILD)

pdckbd.obj: $(osdir)\pdckbd.c
	$(OSBUILD)

pdcscrn.obj: $(osdir)\pdcscrn.c
	$(OSBUILD)

pdcsetsc.obj: $(osdir)\pdcsetsc.c
	$(OSBUILD)

pdcutil.obj: $(osdir)\pdcutil.c
	$(OSBUILD)

firework.exe:   $(demodir)\firework.c
	$(DEMOBUILD)

newdemo.exe:    $(demodir)\newdemo.c
	$(DEMOBUILD)

ptest.exe:      $(demodir)\ptest.c
	$(DEMOBUILD)

rain.exe:       $(demodir)\rain.c
	$(DEMOBUILD)

testcurs.exe:   $(demodir)\testcurs.c
	$(DEMOBUILD) advapi32.lib

tuidemo.exe:    tuidemo.obj tui.obj
	$(DEMOBUILD)

worm.exe:       $(demodir)\worm.c
	$(DEMOBUILD)

xmas.exe:       $(demodir)\xmas.c
	$(DEMOBUILD)

tui.obj: $(demodir)\tui.c $(demodir)\tui.h
	$(BUILD) -I$(demodir) $(demodir)\$*.c

tuidemo.obj: $(demodir)\tuidemo.c
	$(BUILD) -I$(demodir) $(demodir)\$*.c
