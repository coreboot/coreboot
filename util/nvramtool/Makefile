#
# Makefile for nvram utility
# 
# (C) 2005-2008 coresystems GmbH
# written by Stefan Reinauer <stepan@coresystems.de>
#

PROGRAM = nvramtool

CC      = gcc
STRIP	= strip
INSTALL = /usr/bin/install
PREFIX  = /usr/local
CFLAGS  = -O2 -g -Wall -W
#CFLAGS  = -Os -Wall

OBJS =  cmos_lowlevel.o cmos_ops.o common.o compute_ip_checksum.o \
	hexdump.o input_file.o layout.o layout_file.o lbtable.o   \
	nvramtool.o opts.o reg_expr.o

OS_ARCH        = $(shell uname)
ifeq ($(OS_ARCH), Darwin)
LDFLAGS = -framework DirectIO
endif

all: dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)
	$(STRIP) $(STRIP_ARGS) $(PROGRAM)

clean:
	rm -f $(PROGRAM) *.o

distclean: clean
	rm -f .dependencies

dep:
	@$(CC) -MM *.c > .dependencies

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man8
	$(INSTALL) $(PROGRAM).8 $(DESTDIR)$(PREFIX)/share/man/man8

.PHONY: all clean distclean dep

-include .dependencies
