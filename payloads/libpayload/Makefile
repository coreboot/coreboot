##
## This file is part of the libpayload project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##

export src := $(shell pwd)
export srctree := $(src)
export srck := $(src)/util/kconfig
export obj := $(src)/build
export objk := $(src)/build/util/kconfig

export KERNELVERSION      := 0.2.0
export KCONFIG_AUTOHEADER := $(src)/include/libpayload-config.h
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

CC = gcc
HOSTCC = gcc
HOSTCXX = g++
HOSTCFLAGS := -I$(srck) -I$(objk)
HOSTCXXFLAGS := -I$(srck) -I$(objk)

DESTDIR = /opt

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

ifeq ($(strip $(HAVE_DOTCONFIG)),)

all: config

else

include $(src)/.config

ARCHDIR-$(CONFIG_TARGET_I386)    := i386
ARCHDIR-$(CONFIG_TARGET_POWERPC) := powerpc

PLATFORM-y += arch/$(ARCHDIR-y)/Makefile.inc
TARGETS-y :=

BUILD-y := crypto/Makefile.inc libc/Makefile.inc drivers/Makefile.inc
BUILD-$(CONFIG_TINYCURSES) += curses/Makefile.inc

# The primary target needs to be here before we include the
# other files

all: lib

include $(PLATFORM-y) $(BUILD-y)

OBJS     := $(patsubst %,$(obj)/%,$(TARGETS-y))
INCLUDES := -Iinclude -Ibuild -Iinclude/$(ARCHDIR-y)
INCLUDES += -I$(shell $(CC) -print-search-dirs | head -n 1 | cut -d' ' -f2)include

try-run= $(shell set -e; \
TMP=".$$$$.tmp"; \
if ($(1)) > /dev/null 2>&1; \
then echo "$(2)"; \
else echo "$(3)"; \
fi; rm -rf "$$TMP")

cc-option= $(call try-run,\
$(CC) $(1) -S -xc /dev/null -o "$$TMP", $(1), $(2))

STACKPROTECT += $(call cc-option, -fno-stack-protector,)

# TODO: Re-add -Os as soon as we find out why it caused problems.
CFLAGS := -Wall -Werror $(STACKPROTECT) -nostdinc $(INCLUDES) -ffreestanding

all: lib

lib: $(obj)/lib/libpayload.a copystuff

# Copy libpayload.a and head.o into $(src)/lib where lpgcc et al expect them.
copystuff: $(obj)/arch/$(ARCHDIR-y)/head.S.o $(obj)/lib/libpayload.a
	$(Q)cp $(obj)/arch/$(ARCHDIR-y)/head.S.o $(src)/lib/$(ARCHDIR-y)/head.o
	$(Q)cp $(obj)/lib/libpayload.a $(src)/lib

$(obj)/lib/libpayload.a: prepare $(OBJS)
	$(Q)printf "  AR      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(AR) rc $@ $(OBJS)

endif

install: lib
	$(Q)printf "  INSTALL $(DESTDIR)/libpayload/lib\n"
	$(Q)install -m 755 -d $(DESTDIR)/libpayload/lib
	$(Q)cp -r lib/* $(DESTDIR)/libpayload/lib/
	$(Q)printf "  INSTALL $(DESTDIR)/libpayload/include\n"
	$(Q)install -m 755 -d $(DESTDIR)/libpayload/include
	$(Q)for file in `find include -name *.h -type f`; do \
		install -m 755 -d $(DESTDIR)/libpayload/`dirname $$file`; \
		install -m 644 $$file $(DESTDIR)/libpayload/$$file; \
	done
	$(Q)printf "  INSTALL $(DESTDIR)/libpayload/bin\n"
	$(Q)install -m 755 -d $(DESTDIR)/libpayload/bin
	$(Q)install -m 755 bin/lpgcc $(DESTDIR)/libpayload/bin
	$(Q)install -m 755 bin/lpas $(DESTDIR)/libpayload/bin
	$(Q)install -m 644 bin/lp.functions $(DESTDIR)/libpayload/bin
	$(Q)install -m 644 $(KCONFIG_AUTOCONFIG) $(DESTDIR)/libpayload/libpayload.config

prepare:
	$(Q)mkdir -p $(obj)/util/kconfig/lxdialog
	$(Q)mkdir -p $(obj)/crypto $(obj)/curses $(obj)/drivers/video
	$(Q)mkdir -p $(obj)/drivers/usb
	$(Q)mkdir -p $(obj)/arch/$(ARCHDIR-y) $(obj)/lib/$(ARCHDIR-y) $(obj)/libc 
	$(Q)mkdir -p $(src)/lib/$(ARCHDIR-y)

doxy: doxygen
doxygen:
	$(Q)$(DOXYGEN) Doxyfile

doxyclean: doxygen-clean
doxygen-clean:
	$(Q)rm -rf $(DOXYGEN_OUTPUT_DIR)

clean: doxygen-clean
	$(Q)rm -rf $(obj)/crypto $(obj)/curses $(obj)/drivers
	$(Q)rm -rf $(obj)/i386 $(obj)/powerpc $(obj)/lib $(obj)/libc 
	$(Q)rm -rf $(src)/lib/i386 $(src)/lib/powerpc $(src)/lib/libpayload.a

distclean: clean
	$(Q)rm -rf build
	$(Q)rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig*
	$(Q)rm -f $(src)/include/libpayload-config.h

# This include must come _before_ the pattern rules below!
# Order _does_ matter for pattern rules.
include util/kconfig/Makefile

ifeq ($(strip $(HAVE_DOTCONFIG)),)
else

$(obj)/%.o: $(src)/%.c
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -m32 $(CFLAGS) -c -o $@ $<

$(obj)/%.S.o: $(src)/%.S
	$(Q)printf "  CC      $(subst $(shell pwd)/,,$(@))\n"
	$(Q)$(CC) -m32 $(CFLAGS) -c -o $@ $<

endif

.PHONY: $(PHONY) prepare clean distclean doxygen doxy

