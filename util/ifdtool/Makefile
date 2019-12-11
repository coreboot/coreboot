#
# ifdtool - dump Intel Firmware Descriptor information
#
# Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

PROGRAM = ifdtool

CC      = gcc
INSTALL = /usr/bin/env install
PREFIX  = /usr/local
CFLAGS  = -O2 -g -Wall -Wextra -Wmissing-prototypes -Werror
CFLAGS += -I../../src/commonlib/include -I../../src/commonlib/bsd/include
CFLAGS += -I../cbfstool/flashmap
CFLAGS += -include ../../src/commonlib/bsd/include/commonlib/bsd/compiler.h
LDFLAGS =

OBJS = ifdtool.o
OBJS += fmap.o
OBJS += kv_pair.o
OBJS += valstr.o

all: dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(PROGRAM) *.o *~ .dependencies
distclean: clean

dep:
	@$(CC) $(CFLAGS) -MM *.c > .dependencies

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: ../cbfstool/flashmap/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/bin

.PHONY: all clean distclean dep
