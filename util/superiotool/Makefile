##
## This file is part of the superiotool project.
##
## Copyright (C) 2007-2010 Uwe Hermann <uwe@hermann-uwe.de>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

PROGRAM = superiotool

CC      = gcc
INSTALL = /usr/bin/install
PREFIX  = /usr/local

# Set the superiotool version string from the highest revision number
# of the checked out superiotool files.
SVNDEF := -D'SUPERIOTOOL_VERSION="$(shell svnversion -cn . \
          | sed -e "s/.*://" -e "s/\([0-9]*\).*/\1/")"'

CFLAGS += -O2 -Wall -Werror -Wstrict-prototypes -Wundef -Wstrict-aliasing \
         -Werror-implicit-function-declaration -ansi -pedantic $(SVNDEF)
LDFLAGS += -lz

OBJS = superiotool.o ali.o fintek.o ite.o nsc.o nuvoton.o smsc.o winbond.o

OS_ARCH = $(shell uname)
ifeq ($(OS_ARCH), Darwin)
LIBS = -framework IOKit -framework DirectHW -lpci -lz
endif
ifeq ($(OS_ARCH), FreeBSD)
CFLAGS = -O2 -Wall -Werror -Wstrict-prototypes -Wundef -Wstrict-aliasing \
         -Werror-implicit-function-declaration -ansi $(SVNDEF) \
         -I/usr/local/include
LDFLAGS += -L/usr/local/lib
LIBS = -lz
endif
ifeq ($(OS_ARCH), NetBSD)
LDFLAGS = -l$(shell uname -p)
endif

# Support for PCI-attached "Super I/Os" (e.g. in VIA VT82686A/B).
CONFIG_PCI = yes

ifeq ($(CONFIG_PCI), yes)
CFLAGS += -DPCI_SUPPORT
LIBS += -lpci
OBJS += pci.o via.o
endif

all: $(PROGRAM)

superiotool.o: *.c superiotool.h

$(PROGRAM): $(OBJS) superiotool.h
	$(CC) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LIBS)

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man8
	$(INSTALL) $(PROGRAM).8 $(DESTDIR)$(PREFIX)/share/man/man8

clean:
	rm -f $(PROGRAM) *.o

.PHONY: all install clean

