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

PROGRAM = intelvbttool
CC ?= gcc
CFLAGS ?= -O2 -g
CFLAGS += -Wall -Werror
CFLAGS += -I../../src/commonlib/include -I ../../src/commonlib/bsd/include

all: $(PROGRAM)

$(PROGRAM): $(PROGRAM).c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(PROGRAM).c -o $(PROGRAM)

clean:
	rm -f $(PROGRAM) junit.xml

distclean: clean

.PHONY: all clean distclean
