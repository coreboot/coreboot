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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

PROGRAM = superiotool

CC      ?= gcc
INSTALL ?= /usr/bin/install
PREFIX  ?= /usr/local

# Set the superiotool version string to the output of 'git describe'.

VERSION := -D'SUPERIOTOOL_VERSION="$(shell git describe 2>/dev/null)"'

CFLAGS += -O2 -Wall -Wstrict-prototypes -Wundef -Wstrict-aliasing \
         -Werror-implicit-function-declaration -ansi -pedantic $(VERSION)
LDFLAGS += -lz

OBJS = superiotool.o serverengines.o ali.o fintek.o ite.o nsc.o nuvoton.o \
       smsc.o winbond.o infineon.o

OS_ARCH = $(shell uname)
ifeq ($(OS_ARCH), Darwin)
LIBS = -framework IOKit -framework DirectHW -lpci -lz
endif
ifeq ($(OS_ARCH), FreeBSD)
CFLAGS = -O2 -Wall -Werror -Wstrict-prototypes -Wundef -Wstrict-aliasing \
         -Werror-implicit-function-declaration -ansi $(VERSION) \
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
OBJS += pci.o via.o amd.o
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

