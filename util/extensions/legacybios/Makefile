# tag: Makefile for the legacy bios emulation
#
# Copyright (C) 2003 Stefan Reinauer
#
# See the file "COPYING" for further information about
# the copyright and warranty status of this work.
#

# FIXME: limitations of this makefile:
#   - needs vgabios.rom even if its not used
#   - builds x86emu even if it's not used.


ARCH     := $(shell uname -m | sed -e s/i.86/x86/ -e s/sun4u/sparc64/ -e s/arm.*/arm/ -e s/sa110/arm/ -e s/x86_64/amd64/)
TOPDIR   := $(shell /bin/pwd)
BUILDDIR ?= $(TOPDIR)/obj-$(ARCH)
VPATH    := $(TOPDIR)/include:$(TOPDIR)/util:$(BUILDDIR)

include $(TOPDIR)/Rules.make

CFLAGS_DEBUG := -Wall -Wstrict-prototypes -Wshadow

CFLAGS       ?= -Os -fcaller-saves -fomit-frame-pointer $(CFLAGS_ARCH) $(CFLAGS_DEBUG)

export ARCH TOPDIR VPATH CFLAGS BUILDDIR

all: main $(ARCH)
	@echo -e "\nlegacy bios plugin build finished for platform $(ARCH).\n"

.PHONY: kernel $(ARCH)

types.h: types.sh
	@echo -n "Checking types..."
	@cd $(BUILDDIR) && $(TOPDIR)/util/types.sh

bin2hex: bin2hex.c
	@echo -n "Building binary converter..."
	@cd $(BUILDDIR) && $(CC) -o bin2hex $(TOPDIR)/util/bin2hex.c
	@echo -e "\t\tok"

kernel:
	@make -s -C kernel 

main:
	@echo -e "\nWelcome to LegacyBIOS..."
	@test -r $(BUILDDIR) || ( mkdir -p $(BUILDDIR); \
	  echo -e "\nCreating build directory $(BUILDDIR)" )

libx86emu.a:
	make -s -C arch/x86emu/src/x86emu

$(ARCH): types.h bin2hex kernel vgabios.rom libx86emu.a
	@make -s -C arch/$(ARCH)

vgabios.rom:
	@cp $(TOPDIR)/roms/vgabios.rom $(BUILDDIR)

clean:
	@test ! -d $(BUILDDIR) && \
		echo "Architecture $(ARCH) is already clean." || \
		( 						 \
			echo "Cleaning up architecture $(ARCH)"; \
			rm -rf $(BUILDDIR)			 \
			rm forth.dict.core			 \
		)

indent:
	find -path './{arch}' -prune -o -name '*.[ch]' \
				-exec indent -kr -i8 {} \;
	find -name '*~' -exec rm {} \;

