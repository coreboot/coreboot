##
## SPDX-License-Identifier: GPL-2.0-only

PROGRAM   = cbmem
TOP      ?= $(abspath ../..)
ROOT      = $(TOP)/src
CC       ?= $(CROSS_COMPILE)gcc
INSTALL  ?= /usr/bin/env install
PREFIX   ?= /usr/local
CFLAGS   ?= -O2
WERROR=-Werror
CFLAGS   += -Wall -Wextra -Wmissing-prototypes -Wshadow $(WERROR)
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

help:
	@echo "${PROGRAM}: View machine's cbmem contents"
	@echo "Targets: all, clean, distclean, help, install"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean install help

-include .dependencies
