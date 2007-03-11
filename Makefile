##
## This file is part of the LinuxBIOS project.
##
## LinuxBIOS build system Lbuild
##
## Copyright (C) 2007 coresystems GmbH
## Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
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

have_dotconfig := $(wildcard .config)
have_dotxcompile := $(wildcard .xcompile)

src := $(shell pwd)
obj := $(shell pwd)/build
export src obj

# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory

CC         := gcc
CFLAGS     := -Os -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
	      -Werror-implicit-function-declaration -Wstrict-aliasing \
	      -fno-common -ffreestanding -fno-builtin

HOSTCC     := gcc
HOSTCXX    := g++
HOSTCFLAGS := -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer \
	      -Wno-unused -Wno-sign-compare -Wno-pointer-sign

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

# make is silent per default. make V=1 will show all compiler calls.
ifneq ($(V),1)
Q := @
endif

KERNELVERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)
export KERNELVERSION

ifeq ($(strip $(have_dotconfig)),)

all:
	$(Q)printf "Please run make menuconfig, xconfig or config first.\n"

else

include $(src)/.config

ifneq ($(CONFIG_LOCALVERSION),"")
LINUXBIOS_EXTRA_VERSION := -$(shell echo $(CONFIG_LOCALVERSION))
endif

all: prepare prepare2 $(obj)/linuxbios.rom
	$(Q)printf "  DONE\n"

ARCH:=$(shell echo $(CONFIG_ARCH))
MAINBOARDDIR=$(shell echo $(CONFIG_MAINBOARD_NAME))

LINUXBIOSINCLUDE    :=  -I$(src) -Iinclude \
			-I$(src)/include \
			-I$(src)/include/cpu/generic/$(ARCH)/ \
			-include $(obj)/config.h \
			-include $(obj)/build.h

ifneq ($(strip $(have_dotxcompile)),)
	include $(src)/.xcompile
	CC := $(CC_$(ARCH))
	AS := $(AS_$(ARCH))
	LD := $(LD_$(ARCH))
endif

CPPFLAGS := $(LINUXBIOSINCLUDE)
CFLAGS += $(LINUXBIOSINCLUDE)

include lib/Makefile
include device/Makefile
include console/Makefile
include mainboard/$(MAINBOARDDIR)/Makefile
include arch/$(ARCH)/Makefile

endif

include util/Makefile

doc:
	$(DOXYGEN) util/doxygen/Doxyfile.LinuxBIOS

prepare:
	$(Q)mkdir -p $(obj)

prepare2:
	$(Q)printf "  CP      $(subst $(shell pwd)/,,$(obj)/config.h)\n"
	$(Q)cp $(src)/.tmpconfig.h $(obj)/config.h
	$(Q)printf "  GEN     $(subst $(shell pwd)/,,$(obj)/build.h)\n"
	$(Q)printf "#define LINUXBIOS_VERSION \"$(KERNELVERSION)\"\n" > $(obj)/build.h
	$(Q)printf "#define LINUXBIOS_EXTRA_VERSION \"$(LINUXBIOS_EXTRA_VERSION)\"\n" >> $(obj)/build.h
	$(Q)printf "#define LINUXBIOS_BUILD \"`LANG= date`\"\n" >> $(obj)/build.h
	$(Q)printf "\n" >> $(obj)/build.h
	$(Q)printf "#define LINUXBIOS_COMPILER \"$(shell LANG= $(CC) --version | head -n1)\"\n" >> $(obj)/build.h
	$(Q)printf "#define LINUXBIOS_ASSEMBLER \"$(shell LANG= $(AS) --version | head -n1)\"\n" >> $(obj)/build.h
	$(Q)printf "#define LINUXBIOS_LINKER \"$(shell LANG= $(LD) --version | head -n1)\"\n" >> $(obj)/build.h

clean:
	$(Q)printf "  CLEAN   $(subst $(shell pwd)/,,$(obj))\n"
	$(Q)rm -rf $(obj)
	$(Q)printf "  CLEAN   $(subst $(shell pwd)/,,$(DOXYGEN_OUTPUT_DIR))\n"
	$(Q)rm -rf $(DOXYGEN_OUTPUT_DIR)

distclean: clean
	$(Q)printf "  CLEAN   .kconfig.d .config .tmpconfig.h .config.old .xcompile\n"
	$(Q)rm -f .kconfig.d .config .tmpconfig.h .config.old .xcompile

%.o: %.c
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: doc

