##
## This file is part of the LinuxBIOS project.
##
## LinuxBIOS build system Lbuild
##
## Copyright (C) 2007 coresystems GmbH
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
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

VERSION = 3
PATCHLEVEL = 0
SUBLEVEL = 0


ARCH := $(shell uname -m | sed -e s/i.86/x86/ -e s/sun4u/sparc64/ \
				-e s/arm.*/arm/ -e s/sa110/arm/ \
				-e s/s390x/s390/ -e s/parisc64/parisc/ \
				-e s/ppc.*/powerpc/ -e s/mips.*/mips/ )

KERNELVERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)

have_dotconfig := $(wildcard .config)
 
srcdir:=$(shell pwd)
src:=$(srcdir)
objdir:=$(shell pwd)/lbobj
obj:=$(objdir)


# Do not print "Entering directory ..."
MAKEFLAGS += --no-print-directory

CC         := gcc
CFLAGS     := -Os -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
	      -Werror-implicit-function-declaration \
	      -fno-strict-aliasing -fno-common \
	      -ffreestanding -fno-builtin

HOSTCC     := gcc
HOSTCFLAGS := -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer

LINUXBIOSINCLUDE    :=  -I$(src) -Iinclude \
			-I$(src)/include \
			-I$(src)/include/cpu/generic/$(ARCH)/ \
			-include $(obj)/config.h

CPPFLAGS   := $(LINUXBIOSINCLUDE)

export srcdir src objdir obj KERNELVERSION

ifeq ($(strip $(have_dotconfig)),) 
all:
	@echo "run make menuconfig first"
else
include $(src)/.config
all: prepare prepare2 $(obj)/linuxbios.rom
	@echo "build process finished."
endif


include util/Makefile
#include mainboards/Makefile
include arch/$(ARCH)/Makefile

MAINBOARDDIR=$(shell echo $(CONFIG_MAINBOARD_NAME))
-include mainboard/$(MAINBOARDDIR)/Makefile


prepare:
	@mkdir -p $(objdir)

prepare2:
	@cp $(src)/.tmpconfig.h $(obj)/config.h

clean:
	@echo "Cleaning up..."
	rm -rf $(objdir)


%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 
