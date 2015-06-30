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
## Foundation, Inc.
##

src := $(shell pwd)
srctree := $(src)
srck := $(src)/../../util/kconfig
obj := $(src)/build
objk := $(src)/build/util/kconfig

export KERNELVERSION      := 0.1.0
export KCONFIG_AUTOHEADER := $(obj)/config.h
export KCONFIG_AUTOCONFIG := $(obj)/auto.conf
export KCONFIG_DEPENDENCIES := $(obj)/auto.conf.cmd
export KCONFIG_SPLITCONFIG := $(obj)/config
export KCONFIG_TRISTATE := $(obj)/tristate.conf
export KCONFIG_NEGATIVES := 1
export Kconfig := Kconfig

export V := $(V)

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard .config)
MAKEFLAGS += -rR --no-print-directory

# Make is silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
.SILENT:
endif

HOSTCC ?= gcc
HOSTCXX ?= g++
HOSTCFLAGS := -I$(srck) -I$(objk)
HOSTCXXFLAGS := -I$(srck) -I$(objk)

LIBCONFIG_PATH := ../libpayload
LIBPAYLOAD_DIR := build/libpayload
LPCC := $(LIBPAYLOAD_DIR)/libpayload/bin/lpgcc
LPAS := $(LIBPAYLOAD_DIR)/libpayload/bin/lpas
HAVE_LIBPAYLOAD := $(wildcard $(LIBPAYLOAD_DIR)/libpayload/lib/libpayload.a)
LIB_CONFIG ?= defconfig
OBJCOPY ?= objcopy

INCLUDES = -I$(obj) -include $(LIBPAYLOAD_DIR)/libpayload/include/kconfig.h
CFLAGS := -Wall -Werror -Os -fno-builtin $(INCLUDES)
OBJECTS = cpuinfo_module.o cpuid.S.o pci_module.o coreboot_module.o \
	  nvram_module.o bootlog_module.o ramdump_module.o lar_module.o \
	  multiboot_module.o cbfs_module.o coreinfo.o
OBJS    = $(patsubst %,$(obj)/%,$(OBJECTS))
TARGET  = $(obj)/coreinfo.elf

ifneq ($(strip $(HAVE_DOTCONFIG)),)
include $(src)/.config
all: $(TARGET)

$(TARGET): $(src)/.config $(obj)/config.h $(OBJS) libpayload
	printf "    LPCC       $(subst $(shell pwd)/,,$(@)) (LINK)\n"
	$(LPCC) -o $@ $(OBJS)
	$(OBJCOPY) --only-keep-debug $@ $(TARGET).debug
	$(OBJCOPY) --strip-debug $@
	$(OBJCOPY) --add-gnu-debuglink=$(TARGET).debug $@

$(obj)/%.S.o: $(src)/%.S libpayload
	printf "    LPAS       $(subst $(shell pwd)/,,$(@))\n"
	$(LPAS) -o $@ $<

$(obj)/%.o: $(src)/%.c libpayload
	printf "    LPCC       $(subst $(shell pwd)/,,$(@))\n"
	$(LPCC) $(CFLAGS) -c -o $@ $<

else
all: config
endif

ifneq ($(strip $(HAVE_LIBPAYLOAD)),)
libpayload:
	printf "Found Libpayload $(LIBPAYLOAD_DIR).\n"
else
libpayload:
	printf "Building libpayload @ $(LIBCONFIG_PATH).\n"
	make -C $(LIBCONFIG_PATH) distclean
	make -C $(LIBCONFIG_PATH) $(LIB_CONFIG)
	make -C $(LIBCONFIG_PATH) DESTDIR=$(shell pwd)/$(LIBPAYLOAD_DIR) install
endif

$(obj)/config.h:
	$(MAKE) oldconfig

$(shell mkdir -p $(obj) $(objk)/lxdialog $(KCONFIG_SPLITCONFIG))

clean:
	rm -rf build/*.elf build/*.o

distclean: clean
	rm -rf build
	rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig*

include $(srck)/Makefile

.PHONY: $(PHONY) prepare clean distclean
