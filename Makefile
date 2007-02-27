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
 
src:=$(shell pwd)
obj:=$(shell pwd)/lbobj


# Do not print "Entering directory ..."
MAKEFLAGS += --no-print-directory

CC         := gcc
CFLAGS     := -Os -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
	      -Werror-implicit-function-declaration \
	      -fno-strict-aliasing -fno-common \
	      -ffreestanding -fno-builtin

HOSTCC     := gcc
HOSTCXX    := g++
HOSTCFLAGS := -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer \
	      -Wno-unused -Wno-sign-compare -Wno-pointer-sign

LINUXBIOSINCLUDE    :=  -I$(src) -Iinclude \
			-I$(src)/include \
			-I$(src)/include/cpu/generic/$(ARCH)/ \
			-include $(obj)/config.h
# make silent per default
Q := @

CPPFLAGS   := $(LINUXBIOSINCLUDE)

CFLAGS += $(LINUXBIOSINCLUDE)

export src obj KERNELVERSION

ifeq ($(strip $(have_dotconfig)),) 

all:
	$(Q)echo "Please run make menuconfig, xconfig or config first."

else

include $(src)/.config
all: prepare prepare2 $(obj)/linuxbios.rom
	$(Q)echo "Build process finished."

MAINBOARDDIR=$(shell echo $(CONFIG_MAINBOARD_NAME))

include lib/Makefile
include device/Makefile
include console/Makefile
include mainboard/$(MAINBOARDDIR)/Makefile
include arch/$(ARCH)/Makefile
endif

include util/Makefile



prepare:
	$(Q)mkdir -p $(obj)

prepare2:
	$(Q)cp $(src)/.tmpconfig.h $(obj)/config.h

clean:
	$(Q)echo -n "Cleaning up... "
	$(Q)rm -rf $(obj)
	$(Q)echo "done"

distclean: clean
	$(Q)echo -n "Deleting config files... "
	$(Q)rm -f .kconfig.d .config .tmpconfig.h .config.old
	$(Q)echo "done"

%.o: %.c
	$(Q)echo "Compiling $<"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $< 
