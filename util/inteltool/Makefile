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

CC      = gcc
INSTALL = /usr/bin/install
PREFIX  = /usr/local
CFLAGS  = -O2 -g -Wall -W
LDFLAGS = -lpci -lz 

OBJS = inteltool.o

all: pciutils dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(PROGRAM) .dependencies

dep:
	@$(CC) -MM *.c > .dependencies

pciutils:
	@echo; echo -n "Checking for pciutils and zlib... "
	@$(shell ( echo "#include <pci/pci.h>";		   \
		   echo "struct pci_access *pacc;";	   \
		   echo "int main(int argc, char **argv)"; \
		   echo "{ pacc = pci_alloc(); return 0; }"; ) > .test.c )
	@$(CC) $(CFLAGS) .test.c -o .test $(LDFLAGS) &>/dev/null &&	\
		echo "found." || ( echo "not found."; echo;		\
		echo "Please install pciutils-devel and zlib-devel.";	\
		echo "See README for more information."; echo;		\
		rm -f .test.c .test; exit 1)
	@rm -f .test.c .test

install: $(PROGRAM)
	$(INSTALL) $(PROGRAM) $(PREFIX)/sbin
	mkdir -p $(PREFIX)/share/man/man8
	$(INSTALL) $(PROGRAM).8 $(PREFIX)/share/man/man8

.PHONY: all clean distclean dep pciutils

-include .dependencies

