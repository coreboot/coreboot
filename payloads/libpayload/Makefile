##
## This file is part of the libpayload project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
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

BASE_DIR=$(shell pwd)
KCONFIG_DIR=util/kconfig
DESTDIR=/opt

ifeq (.config, $(wildcard .config))
dot-config := 1
else
dot-config := 0
config-targets := 1
endif

ifneq ($(filter textconfig oldconfig defconfig menuconfig,$(MAKECMDGOALS)),)
config-targets := 1
dot-config := 0
endif

ifeq ($(dot-config),0)
all: .config

.config: oldconfig
	@echo "Configuration completed - type make to build libpayload"
else
-include .config

ARCHDIR-$(CONFIG_TARGET_I386) := i386

PLATFORM-y += $(ARCHDIR-y)/Makefile.inc
TARGETS-y :=

BUILD-y := crypto/Makefile.inc libc/Makefile.inc drivers/Makefile.inc
BUILD-$(CONFIG_TINYCURSES) += curses/Makefile.inc

include $(PLATFORM-y) $(BUILD-y)

INCLUDES := -I./include
INCLUDES += -I$(shell $(CC) -print-search-dirs | head -n 1 | cut -d' ' -f2)include
# TODO: Re-add -Os as soon as we find out why it caused problems.
CFLAGS := -Wall -Werror -fno-stack-protector -nostdinc $(INCLUDES)

lib: lib/libpayload.a lib/$(ARCHDIR-y)/head.o

lib/libpayload.a: $(TARGETS-y)
	@ $(AR) rc $@ $(TARGETS-y)

lib/$(ARCHDIR-y)/head.o: $(ARCHDIR-y)/head.o
	@ mkdir -p lib/$(ARCHDIR-y)
	@ cp $< $@

%.o: %.c
	$(CC) -m32 $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(AS) --32 -o $@ $<

install: lib
	install -m 755 -d $(DESTDIR)/libpayload/lib
	cp -r lib/* $(DESTDIR)/libpayload/lib/
	install -m 755 -d $(DESTDIR)/libpayload/include
	for file in `find include -name *.h -type f`; do \
		install -m 644 -D $$file $(DESTDIR)/libpayload/$$file; \
	done
	install -m 755 -d $(DESTDIR)/libpayload/bin
	install -m 755 bin/lpgcc $(DESTDIR)/libpayload/bin
	install -m 755 bin/lpas $(DESTDIR)/libpayload/bin
	install -m 644 bin/lp.functions $(DESTDIR)/libpayload/bin

clean:
	@ rm -f $(TARGETS-y)
	@ rm -f lib/libpayload.a lib/$(ARCHDIR-y)/head.o

distclean: clean
	@ make -C $(KCONFIG_DIR) clean
	@ rm -f $(KCONFIG_DIR)/lxdialog/lxdialog
	@ rm -f .config .kconfig.d  include/autoconf.h
endif

ifeq ($(config-targets),1)

$(KCONFIG_DIR)/conf:
	make -C $(KCONFIG_DIR) conf

$(KCONFIG_DIR)/mconf:
	make -C $(KCONFIG_DIR) mconf

$(KCONFIG_DIR)/lxdialog/lxdialog:
	make -C $(KCONFIG_DIR)/lxdialog lxdialog

textconfig: $(KCONFIG_DIR)/conf
	@$(KCONFIG_DIR)/conf $(BASE_DIR)/Config.in

oldconfig: $(KCONFIG_DIR)/conf
	@$(KCONFIG_DIR)/conf -o $(BASE_DIR)/Config.in

defconfig: $(KCONFIG_DIR)/conf
	@$(KCONFIG_DIR)/conf -d $(BASE_DIR)/Config.in

menuconfig: $(KCONFIG_DIR)/lxdialog/lxdialog $(KCONFIG_DIR)/mconf
	@$(KCONFIG_DIR)/mconf $(BASE_DIR)/Config.in

endif
