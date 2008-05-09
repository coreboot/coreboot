#
# Makefile for lxregs utility
# 

PROGRAM = lxregs

CC      = gcc
STRIP	= strip
INSTALL = /usr/bin/install
PREFIX  = /usr/local
#CFLAGS  = -O2 -g -Wall -Werror
CFLAGS  = -Os -Wall -Werror -DDISABLE_DOC
OS_ARCH	= $(shell uname)
ifeq ($(OS_ARCH), SunOS)
LDFLAGS = -lpci -lz
else
LDFLAGS = -lpci -lz
STRIP_ARGS = -s
endif

OBJS = lxregs.o

all: dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
	$(STRIP) $(STRIP_ARGS) $(PROGRAM)

lxregs.o: lxregs.c
	$(CC) -c $(CFLAGS) $(SVNDEF) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(PROGRAM) .dependencies
	
dep:
	@$(CC) -MM *.c > .dependencies

install: $(PROGRAM)
	$(INSTALL) flashrom $(PREFIX)/sbin
	mkdir -p $(PREFIX)/share/man/man8
	$(INSTALL) $(PROGRAM).8 $(PREFIX)/share/man/man8

.PHONY: all clean distclean dep pciutils

-include .dependencies

