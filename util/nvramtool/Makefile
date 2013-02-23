##
## Makefile for nvram utility
##
## (C) 2005-2008 coresystems GmbH
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

PROGRAM = nvramtool

CC      = gcc
STRIP	= strip
INSTALL = /usr/bin/install
PREFIX  = /usr/local
CFLAGS  = -O2 -g -Wall -W -I. -DCMOS_HAL=1
#CFLAGS  = -Os -Wall

CLI_OBJS = cli/nvramtool.o cli/opts.o

OBJS =  cmos_lowlevel.o cmos_ops.o common.o compute_ip_checksum.o \
	hexdump.o input_file.o layout.o accessors/layout-common.o accessors/layout-text.o accessors/layout-bin.o lbtable.o   \
	reg_expr.o cbfs.o accessors/cmos-hw-unix.o accessors/cmos-mem.o

OBJS += $(CLI_OBJS)

OS_ARCH        = $(shell uname)
ifeq ($(OS_ARCH), Darwin)
LDFLAGS = -framework DirectHW
endif
ifeq ($(OS_ARCH), NetBSD)
LDFLAGS = -l$(shell uname -p)
endif
ifeq ($(shell uname -o 2>/dev/null), Cygwin)
LDFLAGS = -lioperm
CFLAGS += -D__GLIBC__
endif

all: dep $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS) $(CFLAGS)
	$(STRIP) $(STRIP_ARGS) $(PROGRAM)

clean:
	rm -f $(PROGRAM) $(OBJS)

distclean: clean
	rm -f .dependencies

dep:
	@$(CC) -MM *.c > .dependencies

install: $(PROGRAM)
	mkdir -p $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin
	mkdir -p $(DESTDIR)$(PREFIX)/share/man/man8
	$(INSTALL) cli/$(PROGRAM).8 $(DESTDIR)$(PREFIX)/share/man/man8

.PHONY: all clean distclean dep

-include .dependencies
