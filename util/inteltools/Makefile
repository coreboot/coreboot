#
# Makefile for ich_gpio utility
# 
# (C) 2008 by coresystems GmbH
# written by Stefan Reinauer <stepan@coresystems.de>
# Copyright (C) 2008 Joseph Smith <joe@smittys.pointclark.net>
#

PROGRAM = ich_gpio

CC = gcc
STRIP	= strip
INSTALL = /usr/bin/install
PREFIX = /usr/local
CFLAGS = -O2 -g -Wall
OS_ARCH = $(shell uname)
ifeq ($(OS_ARCH), SunOS)
LDFLAGS = -lpci -lz
else
LDFLAGS = -lpci -lz -static 
STRIP_ARGS = -s
endif

OBJS = ich_gpio.o

all: pciutils dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
	$(STRIP) $(STRIP_ARGS) $(PROGRAM)

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

