##
## SPDX-License-Identifier: GPL-2.0-only

PROGRAM = intelvbttool
CC ?= gcc
INSTALL ?= /usr/bin/env install
PREFIX ?= /usr/local
CFLAGS ?= -O2 -g
CFLAGS += -Wall -Werror
CFLAGS += -I../../src/commonlib/include -I ../../src/commonlib/bsd/include

all: $(PROGRAM)

$(PROGRAM): $(PROGRAM).c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(PROGRAM).c -o $(PROGRAM)

clean:
	rm -f $(PROGRAM) junit.xml

distclean: clean

install: $(PROGRAM)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin

.PHONY: all clean distclean
