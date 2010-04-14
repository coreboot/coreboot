##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2009-2010 coresystems GmbH
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

ifeq ($(INNER_SCANBUILD),y)
CC_real:=$(CC)
endif

$(if $(wildcard .xcompile),,$(eval $(shell bash util/xcompile/xcompile > .xcompile)))
include .xcompile

ifeq ($(INNER_SCANBUILD),y)
CC:=$(CC_real)
HOSTCC:=$(CC_real) --hostcc
HOSTCXX:=$(CC_real) --hostcxx
endif

export top := $(PWD)
export src := src
export srck := $(top)/util/kconfig
export obj ?= build
export objk := $(obj)/util/kconfig


export KERNELVERSION      := 4.0
export KCONFIG_AUTOHEADER := $(obj)/config.h
export KCONFIG_AUTOCONFIG := $(obj)/auto.conf

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard .config)
MAKEFLAGS += -rR --no-print-directory

# Make is silent per default, but 'make V=1' will show all compiler calls.
Q:=@
ifneq ($(V),1)
ifneq ($(Q),)
.SILENT:
endif
endif

CPP:= $(CC) -x assembler-with-cpp -DASSEMBLY -E
ROMCC:= $(obj)/romcc
HOSTCC = gcc
HOSTCXX = g++
HOSTCFLAGS := -I$(srck) -I$(objk) -g
HOSTCXXFLAGS := -I$(srck) -I$(objk)
LIBGCC_FILE_NAME := $(shell test -r `$(CC) -print-libgcc-file-name` && $(CC) -print-libgcc-file-name)

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

ifeq ($(strip $(HAVE_DOTCONFIG)),)

all: config

else

include $(top)/.config

ifneq ($(INNER_SCANBUILD),y)
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
CC:=clang -m32
HOSTCC:=clang
endif
endif

ifeq ($(CONFIG_CCACHE),y)
CCACHE:=CCACHE_COMPILERCHECK=content $(wildcard $(addsuffix /ccache,$(subst :, ,$(PATH))))
ifeq ($(CCACHE),)
$(error ccache selected, but not found in PATH)
endif
CC := $(CCACHE) $(CC)
HOSTCC := $(CCACHE) $(HOSTCC)
HOSTCXX := $(CCACHE) $(HOSTCXX)
ROMCC := $(CCACHE) $(ROMCC)
endif

strip_quotes = $(subst ",,$(subst \",,$(1)))

ARCHDIR-$(CONFIG_ARCH_X86)    := i386
ARCHDIR-$(CONFIG_ARCH_POWERPC) := ppc

MAINBOARDDIR=$(call strip_quotes,$(CONFIG_MAINBOARD_DIR))
export MAINBOARDDIR

PLATFORM-y += src/arch/$(ARCHDIR-y) src/cpu src/mainboard/$(MAINBOARDDIR)
TARGETS-y :=

BUILD-y := src/lib src/boot src/console src/devices src/southbridge src/northbridge src/superio src/drivers
BUILD-y += util/cbfstool util/sconfig
BUILD-$(CONFIG_ARCH_X86) += src/pc80

ifneq ($(CONFIG_LOCALVERSION),"")
COREBOOT_EXTRA_VERSION := -$(call strip_quotes,$(CONFIG_LOCALVERSION))
endif

# The primary target needs to be here before we include the
# other files

ifeq ($(INNER_SCANBUILD),y)
CONFIG_SCANBUILD_ENABLE:=
endif

ifeq ($(CONFIG_SCANBUILD_ENABLE),y)
ifneq ($(CONFIG_SCANBUILD_REPORT_LOCATION),)
CONFIG_SCANBUILD_REPORT_LOCATION:=-o $(CONFIG_SCANBUILD_REPORT_LOCATION)
endif
all:
	echo '#!/bin/sh' > .ccwrap
	echo 'CC="$(CC)"' >> .ccwrap
	echo 'if [ "$$1" = "--hostcc" ]; then shift; CC="$(HOSTCC)"; fi' >> .ccwrap
	echo 'if [ "$$1" = "--hostcxx" ]; then shift; CC="$(HOSTCXX)"; fi' >> .ccwrap
	echo 'eval $$CC $$*' >> .ccwrap
	chmod +x .ccwrap
	scan-build $(CONFIG_SCANBUILD_REPORT_LOCATION) -analyze-headers --use-cc=$(top)/.ccwrap --use-c++=$(top)/.ccwrap $(MAKE) INNER_SCANBUILD=y
else
all: $(obj)/config.h coreboot
endif

# must come rather early
.SECONDEXPANSION:

$(obj)/config.h:
	$(MAKE) oldconfig

#######################################################################
# Build the tools

CBFSTOOL:=$(obj)/util/cbfstool/cbfstool

# needed objects that every mainboard uses 
# Creation of these is architecture and mainboard independent
$(obj)/mainboard/$(MAINBOARDDIR)/static.c: $(src)/mainboard/$(MAINBOARDDIR)/devicetree.cb  $(obj)/util/sconfig/sconfig
	@printf "    SCONFIG    $(subst $(src)/,,$(<))\n"
	mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(obj)/util/sconfig/sconfig $(MAINBOARDDIR) $(obj)/mainboard/$(MAINBOARDDIR)

objs:=$(obj)/mainboard/$(MAINBOARDDIR)/static.o
initobjs:=
drivers:=
smmobjs:=
types:=obj initobj driver smmobj

# Clean -y variables, include Makefile.inc
# If $(3) is non-empty, add paths to files in X-y, and add them to Xs
# Add subdirs-y to subdirs
includemakefiles= \
	$(foreach type,$(2), $(eval $(type)-y:=)) \
	$(eval subdirs-y:=) \
	$(eval -include $(1)) \
	$(foreach type,$(2), \
		$(eval $(type)s+= \
			$$(subst $(top)/,, \
			$$(abspath $$(patsubst src/%, \
					$(obj)/%, \
					$$(addprefix $(dir $(1)),$$($(type)-y))))))) \
	$(eval subdirs+=$$(subst $(PWD)/,,$$(abspath $$(addprefix $(dir $(1)),$$(subdirs-y)))))

# For each path in $(subdirs) call includemakefiles, passing $(1) as $(3)
# Repeat until subdirs is empty
evaluate_subdirs= \
	$(eval cursubdirs:=$(subdirs)) \
	$(eval subdirs:=) \
	$(foreach dir,$(cursubdirs), \
		$(eval $(call includemakefiles,$(dir)/Makefile.inc,$(types)))) \
	$(if $(subdirs),$(eval $(call evaluate_subdirs)))

# collect all object files eligible for building
subdirs:=$(PLATFORM-y) $(BUILD-y)
$(eval $(call evaluate_subdirs))

initobjs:=$(addsuffix .initobj.o, $(basename $(initobjs)))
drivers:=$(addsuffix .driver.o, $(basename $(drivers)))
smmobjs:=$(addsuffix .smmobj.o, $(basename $(smmobjs)))

allobjs:=$(foreach var, $(addsuffix s,$(types)), $($(var)))
alldirs:=$(sort $(abspath $(dir $(allobjs))))
source_with_ext=$(patsubst $(obj)/%.o,src/%.$(1),$(allobjs))
allsrc=$(wildcard $(call source_with_ext,c) $(call source_with_ext,S))

define objs_asl_template
$(obj)/$(1)%.o: src/$(1)%.asl
	@printf "    IASL       $$(subst $(top)/,,$$(@))\n"
	$(CPP) -D__ACPI__ -P -include $(abspath $(obj)/config.h) -I$(src) -I$(src)/mainboard/$(MAINBOARDDIR) $$< -o $$(basename $$@).asl
	iasl -p $$(basename $$@) -tc $$(basename $$@).asl
	mv $$(basename $$@).hex $$(basename $$@).c
	$(CC) $$(CFLAGS) $$(if $$(subst dsdt,,$$(basename $$(notdir $$@))), -DAmlCode=AmlCode_$$(basename $$(notdir $$@))) -c -o $$@ $$(basename $$@).c
endef

# macro to define template macros that are used by use_template macro
define create_cc_template
# $1 obj class (objs, initobjs, ...)
# $2 source suffix (c, S)
# $3 .o infix ("" ".initobj", ...)
# $4 additional compiler flags
de$(EMPTY)fine $(1)_$(2)_template
$(obj)/$$(1)%$(3).o: src/$$(1)%.$(2) $(obj)/config.h
	printf "    CC         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(CC) $(4) -MMD $$$$(CFLAGS) -c -o $$$$@ $$$$<
en$(EMPTY)def
endef

$(eval $(call create_cc_template,objs,c))
$(eval $(call create_cc_template,objs,S,,-DASSEMBLY))
$(eval $(call create_cc_template,initobjs,c,.initobj,-D__PRE_RAM__))
$(eval $(call create_cc_template,initobjs,S,.initobj,-DASSEMBLY -D__PRE_RAM__))
$(eval $(call create_cc_template,drivers,c,.driver))
$(eval $(call create_cc_template,drivers,S,.driver,-DASSEMBLY))
$(eval $(call create_cc_template,smmobjs,c,.smmobj))
$(eval $(call create_cc_template,smmobjs,S,.smmobj))

usetemplate=$(foreach d,$(sort $(dir $($(1)))),$(eval $(call $(1)_$(2)_template,$(subst $(obj)/,,$(d)))))
usetemplate=$(foreach d,$(sort $(dir $($(1)))),$(eval $(call $(1)_$(2)_template,$(subst $(obj)/,,$(d)))))
$(eval $(call usetemplate,objs,asl))
$(eval $(call usetemplate,objs,c))
$(eval $(call usetemplate,objs,S))
$(eval $(call usetemplate,initobjs,c))
$(eval $(call usetemplate,initobjs,S))
$(eval $(call usetemplate,drivers,c))
$(eval $(call usetemplate,drivers,S))
$(eval $(call usetemplate,smmobjs,c))
$(eval $(call usetemplate,smmobjs,S))

DEPENDENCIES = $(objs:.o=.d) $(initobjs:.o=.d) $(drivers:.o=.d) $(smmobjs:.o=.d)
-include $(DEPENDENCIES)

printall:
	@echo objs:=$(objs)
	@echo initobjs:=$(initobjs)
	@echo drivers:=$(drivers)
	@echo smmobjs:=$(smmobjs)
	@echo alldirs:=$(alldirs)
	@echo allsrc=$(allsrc)
	@echo DEPENDENCIES=$(DEPENDENCIES)
	@echo LIBGCC_FILE_NAME=$(LIBGCC_FILE_NAME)

printcrt0s:
	@echo crt0s=$(crt0s)
	@echo ldscripts=$(ldscripts)

OBJS     := $(patsubst %,$(obj)/%,$(TARGETS-y))
INCLUDES := -Isrc -Isrc/include -I$(obj) -Isrc/arch/$(ARCHDIR-y)/include 
INCLUDES += -Isrc/devices/oprom/include
# abspath is a workaround for romcc
INCLUDES += -include $(abspath $(obj)/config.h)

CFLAGS = $(INCLUDES) -Os -nostdinc -pipe
CFLAGS += -nostdlib -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Wwrite-strings -Wredundant-decls -Wno-trigraphs 
CFLAGS += -Wstrict-aliasing -Wshadow
ifeq ($(CONFIG_WARNINGS_ARE_ERRORS),y)
CFLAGS += -Werror
endif
CFLAGS += -fno-common -ffreestanding -fno-builtin -fomit-frame-pointer

CBFS_COMPRESS_FLAG:=l
CBFS_PAYLOAD_COMPRESS_FLAG:=
CBFS_PAYLOAD_COMPRESS_NAME:=none
ifeq ($(CONFIG_COMPRESSED_PAYLOAD_LZMA),y)
CBFS_PAYLOAD_COMPRESS_FLAG:=l
CBFS_PAYLOAD_COMPRESS_NAME:=LZMA
endif

coreboot: prepare $(obj)/coreboot.rom

endif

prepare:
	mkdir -p $(obj)
	mkdir -p $(obj)/util/kconfig/lxdialog $(obj)/util/cbfstool
	test -n "$(alldirs)" && mkdir -p $(alldirs) || true

$(obj)/build.h: .xcompile
	@printf "    GEN        build.h\n"
	rm -f $(obj)/build.h
	printf "/* build system definitions (autogenerated) */\n" > $(obj)/build.ht
	printf "#ifndef __BUILD_H\n" >> $(obj)/build.ht
	printf "#define __BUILD_H\n\n" >> $(obj)/build.ht
	printf "#define COREBOOT_VERSION \"$(KERNELVERSION)-r$(shell if [ -d $(top)/.svn -a -f "`which svnversion`" ]; then svnversion $(top); else if [ -d $(top)/.git -a -f "`which git`" ]; then git --git-dir=/$(top)/.git log|grep git-svn-id|cut -f 2 -d@|cut -f 1 -d' '|sort -g|tail -1; fi; fi)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_EXTRA_VERSION \"$(COREBOOT_EXTRA_VERSION)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_BUILD \"`LANG= date`\"\n" >> $(obj)/build.ht
	printf "\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILER \"$(shell LANG= $(CC) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_ASSEMBLER \"$(shell LANG= $(AS) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_LINKER \"$(shell LANG= $(LD) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_TIME \"`LANG= date +%T`\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_BY \"$(subst \,@,$(shell PATH=$$PATH:/usr/ucb whoami))\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_HOST \"$(shell hostname -s 2>/dev/null)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_DOMAIN \"$(shell test `uname -s` = "Linux" && dnsdomainname || domainname 2>/dev/null)\"\n" >> $(obj)/build.ht
	printf "#endif\n" >> $(obj)/build.ht
	mv $(obj)/build.ht $(obj)/build.h

doxy: doxygen
doxygen:
	$(DOXYGEN) documentation/Doxyfile.coreboot

doxyclean: doxygen-clean
doxygen-clean:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

clean-for-update: doxygen-clean
	rm -f $(objs) $(initobjs) $(drivers) $(smmobjs) .xcompile
	rm -f $(DEPENDENCIES)
	rm -f $(obj)/coreboot_ram* $(obj)/coreboot.romstage $(obj)/coreboot.pre* $(obj)/coreboot.bootblock $(obj)/coreboot.a
	rm -rf $(obj)/bootblock* $(obj)/romstage* $(obj)/location.*
	rm -f $(obj)/option_table.* $(obj)/crt0.S $(obj)/ldscript
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/static.c $(obj)/mainboard/$(MAINBOARDDIR)/config.py $(obj)/mainboard/$(MAINBOARDDIR)/static.dot
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s $(obj)/mainboard/$(MAINBOARDDIR)/crt0.disasm
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.* $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.*
	rm -f $(obj)/cpu/x86/smm/smm_bin.c $(obj)/cpu/x86/smm/smm.* $(obj)/cpu/x86/smm/smm
	rmdir -p $(alldirs) 2>/dev/null >/dev/null || true

clean: clean-for-update
	rm -f $(obj)/coreboot* .ccwrap

distclean: clean
	rm -rf $(obj)
	rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig*

update:
	dongle.py -c /dev/term/1 $(obj)/coreboot.rom EOF

# This include must come _before_ the pattern rules below!
# Order _does_ matter for pattern rules.
include util/kconfig/Makefile

$(obj)/ldoptions: $(obj)/config.h
	awk '/^#define ([^"])* ([^"])*$$/ {gsub("\\r","",$$3); print $$2 " = " $$3 ";";}' $< > $@

_OS=$(shell uname -s |cut -c-7)
STACK=
ifeq ($(_OS),MINGW32)
	STACK=-Wl,--stack,16384000
endif
ifeq ($(_OS),CYGWIN_)
	STACK=-Wl,--stack,16384000
endif
$(obj)/romcc: $(top)/util/romcc/romcc.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@)) (this may take a while)\n"
	@# Note: Adding -O2 here might cause problems. For details see:
	@# http://www.coreboot.org/pipermail/coreboot/2010-February/055825.html
	$(HOSTCC) -g $(STACK) -Wall -o $@ $<

.PHONY: $(PHONY) prepare clean distclean doxygen doxy coreboot .xcompile

