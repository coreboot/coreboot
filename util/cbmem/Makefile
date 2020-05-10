##
## SPDX-License-Identifier: GPL-2.0-only

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
