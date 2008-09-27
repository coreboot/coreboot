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
	$(INSTALL) $(PROGRAM) $(PREFIX)/sbin
	mkdir -p $(PREFIX)/share/man/man1
	$(INSTALL) $(PROGRAM).1 $(PREFIX)/share/man/man1

.PHONY: all clean distclean dep

-include .dependencies
