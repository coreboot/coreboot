# SPDX-License-Identifier: GPL-2.0-or-later
PREFIX  ?= /usr/local
INSTALL ?= install

spkmodem-recv:
	$(CC) -o $@ $@.c
install: spkmodem-recv
	$(INSTALL) $< -t $(PREFIX)/bin/
