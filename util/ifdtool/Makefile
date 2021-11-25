# ifdtool - dump Intel Firmware Descriptor information
#
# SPDX-License-Identifier: GPL-2.0-only

PROGRAM = ifdtool

CC     ?= gcc
INSTALL = /usr/bin/env install
PREFIX  = /usr/local
CFLAGS  = -O2 -g -Wall -Wextra -Wmissing-prototypes -Werror
CFLAGS += -I../../src/commonlib/include -I../../src/commonlib/bsd/include
CFLAGS += -I../cbfstool/flashmap
CFLAGS += -include ../../src/commonlib/bsd/include/commonlib/bsd/compiler.h
CFLAGS += -D_DEFAULT_SOURCE # for endianness converting functions
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
