##
## This file is part of the TianoCoreBoot project.
##
## Copyright (C) 2013 Google Inc.
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
obj ?= $(src)/build

export V := $(V)

CONFIG_SHELL := sh
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard .config)
MAKEFLAGS += -rR --no-print-directory

# Make is silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q := @
endif

LIBCONFIG_PATH := ../libpayload
LIBPAYLOAD_DIR := $(obj)/libpayload
LPCC := $(LIBPAYLOAD_DIR)/libpayload/bin/lpgcc
LPAS := $(LIBPAYLOAD_DIR)/libpayload/bin/lpas
HAVE_LIBPAYLOAD := $(wildcard $(LIBPAYLOAD_DIR)/libpayload/lib/libpayload.a)
OBJCOPY ?= objcopy

INCLUDES = -Iinclude
CFLAGS := -Wall -Werror -Os $(INCLUDES)
OBJECTS = tianocoreboot.o
OBJS    = $(patsubst %,$(obj)/%,$(OBJECTS))
TARGET  = $(obj)/tianocoreboot.elf

all: $(TARGET)

$(TARGET): prepare $(OBJS) libpayload
	$(Q)printf "    LINK       $(subst $(shell pwd)/,,$(@))\n"
	$(Q)CC="$(CC)" $(LPCC) -o $@ $(OBJS)
	$(Q)$(OBJCOPY) --only-keep-debug $@ $(TARGET).debug
	$(Q)$(OBJCOPY) --strip-debug $@
	$(Q)$(OBJCOPY) --add-gnu-debuglink=$(TARGET).debug $@

$(obj)/%.o: $(src)/%.c libpayload
	$(Q)printf "    CC         $(subst $(shell pwd)/,,$(@))\n"
	$(Q)CC="$(CC)" $(LPCC) $(CFLAGS) -c -o $@ $<

ifneq ($(strip $(HAVE_LIBPAYLOAD)),)
libpayload:
	$(Q)printf "Found Libpayload $(LIBPAYLOAD_DIR).\n"
else
libpayload:
	$(Q)printf "Building libpayload @ $(LIBCONFIG_PATH).\n"
	$(Q)cp libpayload.config .config
	$(Q)make -C $(LIBCONFIG_PATH) distclean
	$(Q)make -C $(LIBCONFIG_PATH) DESTDIR=$(LIBPAYLOAD_DIR) install DOTCONFIG=$(shell pwd)/.config
endif

prepare:
	$(Q)mkdir -p $(obj)

clean:
	$(Q)rm -rf $(obj) $(LIBPAYLOAD_DIR) .xcompile .config .config.old

.PHONY: $(PHONY) clean

