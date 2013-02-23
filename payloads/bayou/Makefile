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

CONFIG_LZMA=y
CONFIG_NRV2B=y
CONFIG_BUILTIN_LAR=y

PBUILDER_CONFIG=bayou.xml
BUILTIN_LAR=builtin.lar

export src := $(shell pwd)
export obj := $(src)/build

LIBPAYLOAD_DIR := $(obj)/libpayload

CC?=gcc
STRIP?=strip
OBJCOPY?=objcopy

FFLAGS-y=
FFLAGS-$(CONFIG_BUILTIN_LAR) += -DCONFIG_BUILTIN_LAR
FFLAGS-$(CONFIG_LZMA) += -DCONFIG_LZMA
FFLAGS-$(CONFIG_NRV2B) += -DCONFIG_NRV2B

OBJECTS-y=main.o payload.o config.o menu.o self.o
OBJECTS-$(CONFIG_LZMA) += lzma.o
OBJECTS-$(CONFIG_NRV2B) += nrv2b.o
OBJECTS-$(CONFIG_BUILTIN_LAR) += builtin-lar.o

CFLAGS= -Wall -Werror -Os $(FFLAGS-y)
LDFLAGS=-Wl,-T,bayou.ldscript -static
LIBGCC=$(shell $(CC) -m32 -print-libgcc-file-name)

LPCC=$(LIBPAYLOAD_DIR)/bin/lpgcc

bayou.elf: $(OBJECTS-y)
	$(LPCC) $(LDFLAGS) -m32 -o $@ $(OBJECTS-y)
	@$(STRIP) $@

builtin-lar.o: $(BUILTIN_LAR)
	@$(OBJCOPY) -I binary -B i386 -O elf32-i386 $(BUILTIN_LAR) $@

builtin.lar: util/pbuilder/pbuilder
	@rm -f $@
	util/pbuilder/pbuilder -c $(PBUILDER_CONFIG) create $@

util/pbuilder/pbuilder:
	$(MAKE) -C util/pbuilder

%.o: %.c
	$(LPCC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o bayou.elf builtin.lar
	$(MAKE) -C util/pbuilder clean

