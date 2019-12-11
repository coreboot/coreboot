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
INSTALL  ?= /usr/bin/env install
PREFIX   ?= /usr/local
CFLAGS   ?= -O2
CFLAGS   += -Wall -Wextra -Wmissing-prototypes -Werror
CPPFLAGS += -I . -I $(ROOT)/commonlib/include -I $(ROOT)/commonlib/bsd/include
CPPFLAGS += -include $(ROOT)/commonlib/bsd/include/commonlib/bsd/compiler.h

OBJS = $(PROGRAM).o

all: $(PROGRAM)

$(PROGRAM): $(OBJS)

clean:
	rm -f $(PROGRAM) *.o .dependencies *~ junit.xml

install: $(PROGRAM)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/sbin/
	$(INSTALL) cbmem $(DESTDIR)$(PREFIX)/sbin/

distclean: clean

.dependencies:
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM *.c > .dependencies

.PHONY: all clean distclean

-include .dependencies
