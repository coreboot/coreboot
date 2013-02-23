##
## Makefile for ectool
##
## Copyright (C) 2009 by coresystems GmbH
## written by Stefan Reinauer <stepan@coresystems.de>
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

CC = gcc
CFLAGS = -O2 -Wall -W
PROGRAM = ectool
INSTALL = /usr/bin/install
PREFIX  = /usr/local

OS_ARCH = $(shell uname -o)
ifeq ($(OS_ARCH), Cygwin)
LDFLAGS = -lioperm
endif

all: $(PROGRAM)

$(PROGRAM): ec.o ectool.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(PROGRAM)
	$(INSTALL) $(PROGRAM) $(PREFIX)/sbin

clean:
	rm *.o $(PROGRAM)

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -I. -o $@
