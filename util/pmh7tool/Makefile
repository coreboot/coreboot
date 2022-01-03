## SPDX-License-Identifier: GPL-2.0-only

CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror
PROGRAM = pmh7tool
INSTALL = /usr/bin/env install
PREFIX	= /usr/local

all: $(PROGRAM)

$(PROGRAM): pmh7tool.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(PROGRAM)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin

clean:
	rm -f *.o $(PROGRAM)

distclean: clean

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -I. -o $@

.PHONY: all install clean distclean
