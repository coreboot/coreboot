##
## This file is part of the coreinfo project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
##

CC = gcc
CROSS_CFLAGS = -m32

INCLUDES = -I../libpayload/include 
INCLUDES += -I$(shell $(CC) $(CROSS_CFLAGS) -print-search-dirs | head -n 1 | cut -d' ' -f2)include

LIBPAYLOAD = ../libpayload/libpayload.a
LIBGCC := $(shell $(CC) $(CROSS_CFLAGS) -print-libgcc-file-name)
CFLAGS := -Werror -fno-stack-protector -nostdinc $(INCLUDES)

MODULES = cpuinfo_module.o cpuid.o pci_module.o coreboot_module.o
OBJECTS = coreinfo.o
TARGET  = coreinfo.elf

all: $(TARGET)

clean:
	@rm -f $(TARGET) $(MODULES) $(OBJECTS)

$(TARGET): $(OBJECTS) $(MODULES)
	@#ld --verbose -T ../libpayload/libpayload.ldscript -o $@ $(OBJECTS) --whole-archive $(LIBPAYLOAD) --no-whole-archive $(LIBGCC)
	ld -T ../libpayload/libpayload.ldscript -o $@ $(OBJECTS) $(MODULES) ../libpayload/i386/head.o $(LIBPAYLOAD) $(LIBGCC)
	@strip $@

.S.o:
	$(AS) --32 -o $@ $<

.c.o:
	$(CC) $(CROSS_CFLAGS) $(CFLAGS) -c -o $@ $<
