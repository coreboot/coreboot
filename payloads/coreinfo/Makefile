##
## This file is part of the coreinfo project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
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

export src := $(shell pwd)
export srctree := $(src)
export srck := $(src)/util/kconfig
export obj := $(src)/build
export objk := $(src)/build/util/kconfig

export KERNELVERSION      := 0.1.0
export KCONFIG_AUTOHEADER := $(obj)/config.h
export KCONFIG_AUTOCONFIG := $(obj)/auto.conf

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard .config)
MAKEFLAGS += -rR --no-print-directory

# Make is silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
endif

HOSTCC = gcc
HOSTCXX = g++
HOSTCFLAGS := -I$(srck) -I$(objk)
HOSTCXXFLAGS := -I$(srck) -I$(objk)

CC = gcc
CROSS_CFLAGS = -m32
INCLUDES = -I../libpayload/include \
	   -I$(shell $(CC) $(CROSS_CFLAGS) -print-search-dirs | \
	   head -n 1 | cut -d' ' -f2)include
LIBPAYLOAD = ../libpayload/libpayload.a
LIBGCC := $(shell $(CC) $(CROSS_CFLAGS) -print-libgcc-file-name)
CFLAGS := -Wall -Werror -Os -fno-stack-protector -nostdinc $(INCLUDES)
MODULES = cpuinfo_module.o cpuid.S.o pci_module.o coreboot_module.o
OBJECTS = coreinfo.o
OBJS    = $(patsubst %,$(obj)/%,$(OBJECTS)) $(patsubst %,$(obj)/%,$(MODULES))
TARGET  = $(obj)/coreinfo.elf

ifeq ($(strip $(HAVE_DOTCONFIG)),)

all:
	$(Q)printf "Please run make config/menuconfig/xconfig/gconfig first.\n"

else

include $(src)/.config

all: prepare $(TARGET)

$(TARGET): $(OBJS)
	$(Q)printf "  LD      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)#ld --verbose -T ../libpayload/libpayload.ldscript -o $@ $(OBJS) \
		#--whole-archive $(LIBPAYLOAD) --no-whole-archive $(LIBGCC)
	$(Q)ld -T ../libpayload/libpayload.ldscript -o $@ $(OBJS) \
		../libpayload/i386/head.o $(LIBPAYLOAD) $(LIBGCC)
	$(Q)printf "  STRIP   $(subst $(shell pwd)/,,$(@))\n"
	$(Q)strip -s $@

$(obj)/%.S.o: $(src)/%.S
	$(Q)printf "  AS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AS) --32 -o $@ $<

$(obj)/%.o: $(src)/%.c
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) $(CROSS_CFLAGS) $(CFLAGS) -c -o $@ $<

endif

prepare:
	$(Q)mkdir -p $(obj)/util/kconfig/lxdialog

clean:
	$(Q)rm -rf build

distclean: clean
	$(Q)rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig*

include util/kconfig/Makefile

.PHONY: $(PHONY) prepare clean distclean

