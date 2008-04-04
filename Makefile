##
## This file is part of the coreboot project.
##
## coreboot build system Lbuild
##
## Copyright (C) 2007 coresystems GmbH
## (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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
# Set the sublevel version string from the highest revision number
# of the checked out files.
SUBLEVEL := "$(shell svnversion -cn . \ | sed -e "s/.*://" -e "s/\([0-9]*\).*/\1/")"'

have_dotconfig := $(wildcard .config)

src := $(shell pwd)
obj := $(shell pwd)/build
export src obj

# Do not print "Entering directory ...".
MAKEFLAGS += --no-print-directory

CC         := gcc
CFLAGS     := -Os -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs \
	      -Werror-implicit-function-declaration -Wstrict-aliasing \
	      -fno-common -ffreestanding -fno-builtin -fomit-frame-pointer \
	      -mpreferred-stack-boundary=2 -mregparm=3 -pipe
# FIXME: Does stack boundary or regparm break the code on real hardware?

HOSTCC     := gcc
HOSTCXX    := g++
HOSTCFLAGS := -Wall -Wstrict-prototypes -g -fomit-frame-pointer \
	      -Wno-unused -Wno-sign-compare

LEX        := flex
LYX        := lyx
DOXYGEN    := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

# Make is silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
endif

KERNELVERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)
export KERNELVERSION

include $(shell $(src)/util/xcompile/xcompile > $(src)/.xcompile || \
	{ echo "complete\\ toolchain" && rm -f $(src)/.xcompile && exit 1; }; echo $(src)/.xcompile)

ifeq ($(strip $(have_dotconfig)),)

all:
	$(Q)printf "Please run make menuconfig, xconfig or config first.\n"

else

include $(src)/.config

ifneq ($(CONFIG_LOCALVERSION),"")
COREBOOT_EXTRA_VERSION := -$(shell echo $(CONFIG_LOCALVERSION))
endif

all: prepare prepare2 $(obj)/coreboot.rom
	$(Q)printf "  DONE\n"

ARCH:=$(shell echo $(CONFIG_ARCH))
MAINBOARDDIR=$(shell echo $(CONFIG_MAINBOARD_NAME))
export MAINBOARDDIR

COREBOOTINCLUDE    :=   -I$(src) -Iinclude \
			-I$(src)/include \
			-I$(src)/include/arch/$(ARCH)/ \
			-I$(src)/mainboard/$(MAINBOARDDIR)/ \
			-include $(obj)/config.h \
			-include $(obj)/build.h

CC := $(CC_$(ARCH))
AS := $(AS_$(ARCH))
LD := $(LD_$(ARCH))
NM := $(NM_$(ARCH))
OBJCOPY := $(OBJCOPY_$(ARCH))
AR := $(AR_$(ARCH))

CFLAGS += $(CFLAGS_$(ARCH))

CPPFLAGS := $(COREBOOTINCLUDE)
CFLAGS += $(COREBOOTINCLUDE)

# Note: This _must_ come after 'CC' is set for the second time in this
# Makefile (see above), otherwise the build would break if 'gcc' isn't
# the compiler actually used for the build (e.g. on cross compiler setups).
CFLAGS += -nostdinc -isystem `$(CC) -print-file-name=include`

include lib/Makefile
include device/Makefile
include mainboard/$(MAINBOARDDIR)/Makefile
include Rules.make
include northbridge/*/*/Makefile
include southbridge/*/*/Makefile
include superio/*/*/Makefile
include arch/$(ARCH)/Makefile

endif

include util/Makefile

doc:
	$(Q)$(LYX) -e pdf doc/design/newboot.lyx

doxy: doxygen
doxygen:
	$(Q)$(DOXYGEN) util/doxygen/Doxyfile.coreboot

prepare:
	$(Q)mkdir -p $(obj)

# FIXME: If .config changed, a rebuild is required.

prepare2:
	$(Q)printf "  CP      $(subst $(shell pwd)/,,$(obj)/config.h)\n"
	$(Q)cp $(src)/.tmpconfig.h $(obj)/config.h
	$(Q)printf "  GEN     $(subst $(shell pwd)/,,$(obj)/build.h)\n"
	$(Q)printf "#define COREBOOT_VERSION \"$(KERNELVERSION)\"\n" > $(obj)/build.h
	$(Q)printf "#define COREBOOT_EXTRA_VERSION \"$(COREBOOT_EXTRA_VERSION)\"\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_BUILD \"`LANG= date`\"\n" >> $(obj)/build.h
	$(Q)printf "\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_COMPILER \"$(shell LANG= $(CC) --version | head -n1)\"\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_ASSEMBLER \"$(shell LANG= $(AS) --version | head -n1)\"\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_LINKER \"$(shell LANG= $(LD) --version | head -n1)\"\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_COMPILE_TIME \"`LANG= date +%T`\"\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_COMPILE_BY \"$(shell PATH=$$PATH:/usr/ucb whoami)\"\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_COMPILE_HOST \"$(shell hostname)\"\n" >> $(obj)/build.h
	$(Q)printf "#define COREBOOT_COMPILE_DOMAIN \"$(shell test `uname -s` = "Linux" && dnsdomainname || domainname)\"\n" >> $(obj)/build.h

clean:
	$(Q)printf "  CLEAN   $(subst $(shell pwd)/,,$(obj))\n"
	$(Q)rm -rf $(obj)
	$(Q)printf "  CLEAN   $(subst $(shell pwd)/,,$(DOXYGEN_OUTPUT_DIR))\n"
	$(Q)rm -rf $(DOXYGEN_OUTPUT_DIR)
	$(Q)printf "  CLEAN   doc/design/newboot.pdf\n"
	$(Q)rm -f doc/design/newboot.pdf

distclean: clean
	$(Q)printf "  CLEAN   .kconfig.d .config .tmpconfig.h .config.old .xcompile\n"
	$(Q)rm -f .kconfig.d .config .tmpconfig.h .config.old .xcompile

%.o: %.c
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: doc doxygen depends prepare prepare2 clean distclean

