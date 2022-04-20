# ifdtool - dump Intel Firmware Descriptor information
#
# SPDX-License-Identifier: GPL-2.0-only

CC      ?= gcc
INSTALL ?= /usr/bin/env install
PREFIX  ?= /usr/local

HOSTCC ?= $(CC)
HOSTCFLAGS ?= $(CFLAGS)
top := ../..
objutil := ..
include Makefile.inc

PROGRAM=$(objutil)/ifdtool/ifdtool

all: $(PROGRAM)

clean:
	rm -f $(PROGRAM) *.o *~ .dependencies
distclean: clean

install: $(PROGRAM)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/bin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/bin

.PHONY: all clean distclean install
