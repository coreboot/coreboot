##
## Makefile for pmh7tool
##
## Copyright (C) 2018 Evgeny Zinoviev <me@ch1p.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

CC = gcc
CFLAGS = -O2 -Wall -W -Werror
PROGRAM = pmh7tool
INSTALL = /usr/bin/install
PREFIX	= /usr/local

all: $(PROGRAM)

$(PROGRAM): pmh7tool.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(PROGRAM)
	$(INSTALL) $(PROGRAM) $(PREFIX)/sbin

clean:
	rm -f *.o $(PROGRAM)

distclean: clean

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -I. -o $@

.PHONY: all install clean distclean
