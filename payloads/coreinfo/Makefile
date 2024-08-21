## SPDX-License-Identifier: GPL-2.0-only

unexport $(COREBOOT_EXPORTS)

src := $(CURDIR)
obj := build

srctree := $(src)
srck := $(src)/../../util/kconfig
objk := $(src)/build/util/kconfig

LIBPAYLOAD_DEFCONFIG ?= $(src)/../libpayload/configs/defconfig-tinycurses

PAYLOAD_DEPS := $(obj)/config.h

OBJECTS = cpuinfo_module.o cpuid.S.o pci_module.o coreboot_module.o \
	  nvram_module.o bootlog_module.o ramdump_module.o \
	  multiboot_module.o cbfs_module.o timestamps_module.o coreinfo.o
OBJS    = $(patsubst %,$(obj)/%,$(OBJECTS))
TARGET  = $(obj)/coreinfo.elf

all: real-all

include ../libpayload/Makefile.payload

ifeq ($(filter %clean,$(MAKECMDGOALS)),)
export KERNELVERSION		:= 0.1.0
export KCONFIG_AUTOHEADER	:= $(obj)/config.h
export KCONFIG_AUTOCONFIG	:= $(obj)/auto.conf
export KCONFIG_DEPENDENCIES	:= $(obj)/auto.conf.cmd
export KCONFIG_SPLITCONFIG	:= $(obj)/config
export KCONFIG_TRISTATE		:= $(obj)/tristate.conf
export KCONFIG_CONFIG		:= $(CURDIR)/.config
export KCONFIG_NEGATIVES	:= 1
export KBUILD_KCONFIG		:= Kconfig

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard .config)
MAKEFLAGS += -rR --no-print-directory

HOSTCC ?= gcc
HOSTCXX ?= g++
HOSTCFLAGS := -I$(srck) -I$(objk)
HOSTCXXFLAGS := -I$(srck) -I$(objk)

CFLAGS += -I$(obj) -I../../src/commonlib/include

ifneq ($(strip $(HAVE_DOTCONFIG)),)
include $(src)/.config
real-all: $(TARGET)

ifeq ($(CONFIG_LTO),y)
CFLAGS += -flto
endif

else
real-all: config
endif

defaultbuild:
	$(MAKE) olddefconfig
	$(MAKE) all

$(obj)/config.h:
	$(MAKE) oldconfig

$(shell mkdir -p $(objk)/lxdialog $(KCONFIG_SPLITCONFIG))

include $(srck)/Makefile.mk

.PHONY: $(PHONY) prepare all real-all defaultbuild

else

distclean: clean
	rm -f .config*

.PHONY: distclean
endif
