#
# Makefile for inteltool utility
#
# Copyright (C) 2008 by coresystems GmbH
# written by Stefan Reinauer <stepan@coresystems.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

PROGRAM = inteltool

CC      ?= gcc
INSTALL ?= /usr/bin/install
PREFIX  ?= /usr/local
CFLAGS  ?= -O2 -g -Wall -W
LDFLAGS += -lpci -lz

OBJS = inteltool.o cpu.o gpio.o rootcmplx.o powermgt.o memory.o pcie.o amb.o

OS_ARCH	= $(shell uname)
ifeq ($(OS_ARCH), Darwin)
LDFLAGS += -framework DirectHW
endif
ifeq ($(OS_ARCH), FreeBSD)
CFLAGS += -I/usr/local/include
LDFLAGS += -L/usr/local/lib
LIBS = -lz
endif

all: pciutils dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(PROGRAM) *.o *~

distclean: clean
	rm -f .dependencies

dep:
	@$(CC) $(CFLAGS) -MM *.c > .dependencies

define LIBPCI_TEST
/* Avoid a failing test due to libpci header symbol shadowing breakage */
#define index shadow_workaround_index
#include <pci/pci.h>
struct pci_access *pacc;
int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;
	pacc = pci_alloc();
	return 0;
}
endef
export LIBPCI_TEST

pciutils:
	@printf "\nChecking for pciutils and zlib... "
	@echo "$$LIBPCI_TEST" > .test.c
	@$(CC) $(CFLAGS) .test.c -o .test $(LDFLAGS) >/dev/null 2>&1 &&	  \
		printf "found.\n" || ( printf "not found.\n\n"; 	  \
		printf "Please install pciutils-devel and zlib-devel.\n"; \
		printf "See README for more information.\n\n";		  \
		rm -f .test.c .test; exit 1)
	@rm -rf .test.c .test .test.dSYM

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man8
	$(INSTALL) $(PROGRAM).8 $(DESTDIR)$(PREFIX)/share/man/man8

.PHONY: all clean distclean dep pciutils

-include .dependencies

