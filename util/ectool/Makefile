## SPDX-License-Identifier: GPL-2.0-only

CC = gcc
WERROR=-Werror
CFLAGS = -O2 -Wall -Wextra -Wshadow $(WERROR)
PROGRAM = ectool
INSTALL = /usr/bin/env install
PREFIX  = /usr/local
OS_ARCH = $(shell uname)

ifeq ($(shell uname -o 2>/dev/null), Cygwin)
LDFLAGS = -lioperm
endif

ifeq ($(OS_ARCH), $(filter $(OS_ARCH), NetBSD OpenBSD))
LDFLAGS = -l$(shell uname -p)
endif

all: $(PROGRAM)

$(PROGRAM): ec.o ectool.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: $(PROGRAM)
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/sbin
	$(INSTALL) $(PROGRAM) $(DESTDIR)$(PREFIX)/sbin

clean:
	rm -f *.o $(PROGRAM) junit.xml

distclean: clean

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -I. -o $@

help:
	@echo "${PROGRAM}: Dump RAM of Embedded Controller (EC)"
	@echo "Targets: all, clean, distclean, help, install"
	@echo "To disable warnings as errors, run make as:"
	@echo "  make all WERROR=\"\""

.PHONY: all clean distclean help install
