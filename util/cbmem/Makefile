##
## This file is part of the coreboot project.
##
## Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

PROGRAM   = cbmem
ROOT      = ../../src
CC       ?= $(CROSS_COMPILE)gcc
INSTALL  ?= /usr/bin/install
PREFIX   ?= /usr/local
CFLAGS   ?= -O2
CFLAGS   += -Wall -Werror
CPPFLAGS += -I $(ROOT)/commonlib/include

OBJS = $(PROGRAM).o

all: $(PROGRAM)

$(PROGRAM): $(OBJS)

clean:
	rm -f $(PROGRAM) *.o *~ junit.xml

install: $(PROGRAM)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/sbin/
	$(INSTALL) cbmem $(DESTDIR)$(PREFIX)/sbin/

distclean: clean
	rm -f .dependencies

.dependencies:
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM *.c > .dependencies

.PHONY: all clean distclean

-include .dependencies

