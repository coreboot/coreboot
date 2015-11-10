# Watcom WMAKE Makefile for PDCurses library - DOS (16 bit) Watcom C/C++ 10.6+
#
# Usage: wmake -f [path\]wccdos16.mak [DEBUG=Y] [target]
#
# where target can be any of:
# [all|demos|pdcurses.lib|testcurs.exe...]

# Change the memory MODEL here, if desired
MODEL		= l

!ifdef %PDCURSES_SRCDIR
PDCURSES_SRCDIR	= $(%PDCURSES_SRCDIR)
!else
PDCURSES_SRCDIR	= ..
!endif

!include $(PDCURSES_SRCDIR)\version.mif

osdir		= $(PDCURSES_SRCDIR)\dos

CC		= wcc
TARGET		= dos

CFLAGS		= /bt=$(TARGET) /zq /wx /m$(MODEL) /i=$(PDCURSES_SRCDIR)

!ifeq DEBUG Y
CFLAGS  	+= /d2 /DPDCDEBUG
LDFLAGS 	= D W A op q sys $(TARGET)
!else
CFLAGS  	+= /oneatx
LDFLAGS 	= op q sys $(TARGET)
!endif

LIBEXE		= wlib /q /n /t

!include $(PDCURSES_SRCDIR)\watcom.mif

$(LIBCURSES) : $(LIBOBJS) $(PDCOBJS)
	%write wccdos.lrf $(LIBOBJS) $(PDCOBJS)
	$(LIBEXE) $@ @wccdos.lrf
	-del wccdos.lrf
	-copy $(LIBCURSES) panel.lib

PLATFORM1	= Watcom C++ 16-bit DOS
PLATFORM2	= Open Watcom 1.6 for 16-bit DOS
ARCNAME		= pdc$(VER)16w

!include $(PDCURSES_SRCDIR)\makedist.mif
