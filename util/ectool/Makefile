#
# Makefile for ectool
#
# Copyright (C) 2009 by coresystems GmbH
# written by Stefan Reinauer <stepan@coresystems.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

CC=gcc
CFLAGS=-O2 -Wall -W

all: ectool

ectool: ec.o ectool.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o ectool

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -I. -o $@
