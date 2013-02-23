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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

src := $(shell pwd)
srctree := $(src)
srck := $(src)/util/kconfig
obj := $(src)/build
objk := $(src)/build/util/kconfig

export KERNELVERSION      := 0.1.0
export KCONFIG_AUTOHEADER := $(obj)/config.h
export KCONFIG_AUTOCONFIG := $(obj)/auto.conf
export V := $(V)

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard .config)
MAKEFLAGS += -rR --no-print-directory

# Make is silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
endif

HOSTCC ?= gcc
HOSTCXX ?= g++
HOSTCFLAGS := -I$(srck) -I$(objk)
HOSTCXXFLAGS := -I$(srck) -I$(objk)

LIBCONFIG_PATH := ../libpayload
LIBPAYLOAD_DIR := ./libpayloadbin
LPCC := $(LIBPAYLOAD_DIR)/libpayload/bin/lpgcc
LPAS := $(LIBPAYLOAD_DIR)/libpayload/bin/lpas
HAVE_LIBPAYLOAD := $(wildcard $(LIBPAYLOAD_DIR)/libpayload/lib/libpayload.a)
LIB_CONFIG ?= defconfig
OBJCOPY ?= objcopy

INCLUDES = -I$(obj)
CFLAGS := -Wall -Werror -Os $(INCLUDES)
OBJECTS = cpuinfo_module.o cpuid.S.o pci_module.o coreboot_module.o \
	  nvram_module.o bootlog_module.o ramdump_module.o lar_module.o \
	  multiboot_module.o cbfs_module.o coreinfo.o
OBJS    = $(patsubst %,$(obj)/%,$(OBJECTS))
TARGET  = $(obj)/coreinfo.elf

ifneq ($(strip $(HAVE_DOTCONFIG)),)
include $(src)/.config
all: $(TARGET)

$(TARGET): $(src)/.config $(OBJS) prepare libpayload
	$(Q)printf "  LPCC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LPCC) -o $@ $(OBJS)
	$(Q)$(OBJCOPY) --only-keep-debug $@ $(TARGET).debug
	$(Q)$(OBJCOPY) --strip-debug $@
	$(Q)$(OBJCOPY) --add-gnu-debuglink=$(TARGET).debug $@

$(obj)/%.S.o: $(src)/%.S libpayload
	$(Q)printf "  LPAS      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LPAS) -o $@ $<

$(obj)/%.o: $(src)/%.c libpayload
	$(Q)printf "  LPCC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(LPCC) $(CFLAGS) -c -o $@ $<

else
all: config
endif

ifneq ($(strip $(HAVE_LIBPAYLOAD)),)
libpayload:
	$(Q)printf "Found Libpayload $(LIBPAYLOAD_DIR).\n"
else
libpayload:
	$(Q)printf "Building libpayload @ $(LIBCONFIG_PATH).\n"
	$(Q)make -C $(LIBCONFIG_PATH) distclean
	$(Q)make -C $(LIBCONFIG_PATH) $(LIB_CONFIG)
	$(Q)make -C $(LIBCONFIG_PATH) DESTDIR=$(shell pwd)/$(LIBPAYLOAD_DIR) install
endif

prepare:
	$(Q)mkdir -p $(obj)/util/kconfig/lxdialog

clean:
	$(Q)rm -rf build/*.elf build/*.o

distclean: clean
	$(Q)rm -rf build
	$(Q)rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig*

include util/kconfig/Makefile

.PHONY: $(PHONY) prepare clean distclean

