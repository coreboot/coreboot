# SPDX-License-Identifier: GPL-2.0-or-later
PREFIX  ?= /usr/local
INSTALL ?= install

spkmodem-recv:
	$(CC) -o $@ $@.c
install: spkmodem-recv
	$(INSTALL) -d $(DESTDIR)$(PREFIX)/bin/
	$(INSTALL) $< -t $(DESTDIR)$(PREFIX)/bin/
