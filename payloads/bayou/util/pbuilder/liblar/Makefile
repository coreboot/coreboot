##
## This file is part of the bayou project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License version 2 as
## published by the Free Software Foundation.
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

HOSTCC = gcc
AR?=ar
LAROBJS = self.o lib.o
CFLAGS = -g -Wall

liblar.a: $(LAROBJS)
	$(AR) rc $@ $(LAROBJS)

%.o: %.c
	$(HOSTCC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f liblar.a *.o
