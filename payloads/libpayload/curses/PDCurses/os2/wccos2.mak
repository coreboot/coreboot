# Watcom WMAKE Makefile for PDCurses library - OS/2 Open Watcom 1.1+
#
# Usage: wmake -f [path\]wccos2.mak [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

!ifdef %PDCURSES_SRCDIR
PDCURSES_SRCDIR	= $(%PDCURSES_SRCDIR)
!else
PDCURSES_SRCDIR	= ..
!endif

!include $(PDCURSES_SRCDIR)\version.mif

osdir		= $(PDCURSES_SRCDIR)\os2

CC		= wcc386
TARGET		= os2v2

CFLAGS		= /bt=$(TARGET) /wx /s /zq /i=$(PDCURSES_SRCDIR)

!ifeq DEBUG Y
CFLAGS  	+= /d2 /DPDCDEBUG
LDFLAGS 	= D A op q sys $(TARGET)
!else
CFLAGS  	+= /oneatx
LDFLAGS 	= op q sys $(TARGET)
!endif

LIBEXE		= wlib /q /n /b /c /t

!include $(PDCURSES_SRCDIR)\watcom.mif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ $(LIBOBJS) $(PDCOBJS)
	-copy $(LIBCURSES) panel.lib

PLATFORM1	= Watcom C++ OS/2
PLATFORM2	= Open Watcom 1.6 for OS/2
ARCNAME		= pdc$(VER)_wcc_os2

!include $(PDCURSES_SRCDIR)\makedist.mif
