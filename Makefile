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

export top := $(CURDIR)
export src := src
export srck := $(top)/util/kconfig
export obj ?= build
export objutil ?= $(obj)/util
export objk := $(objutil)/kconfig


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
ROMCC:= $(objutil)/romcc/romcc
HOSTCC = gcc
HOSTCXX = g++
HOSTCFLAGS := -I$(srck) -I$(objk) -g
HOSTCXXFLAGS := -I$(srck) -I$(objk)
LIBGCC_FILE_NAME := $(shell test -r `$(CC) -print-libgcc-file-name` && $(CC) -print-libgcc-file-name)

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

# Three cases where we don't need fully populated $(obj) lists:
# 1. when no .config exists
# 2. when make config (in any flavour) is run
# 3. when make distclean is run
# Don't waste time on reading all Makefile.incs in these cases
ifeq ($(strip $(HAVE_DOTCONFIG)),)
NOCOMPILE:=1
endif
ifneq ($(MAKECMDGOALS),)
ifneq ($(filter %config distclean,$(MAKECMDGOALS)),)
NOCOMPILE:=1
endif
endif

ifeq ($(NOCOMPILE),1)
all: config

else

include $(HAVE_DOTCONFIG)

ifneq ($(INNER_SCANBUILD),y)
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
CC:=clang -m32
HOSTCC:=clang
endif
endif

ifeq ($(CONFIG_CCACHE),y)
CCACHE:=$(wildcard $(addsuffix /ccache,$(subst :, ,$(PATH))))
ifeq ($(CCACHE),)
$(error ccache selected, but not found in PATH)
endif
CCACHE:=CCACHE_COMPILERCHECK=content CCACHE_BASEDIR=$(top) $(CCACHE)
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

CBFSTOOL:=$(objutil)/cbfstool/cbfstool

# needed objects that every mainboard uses
# Creation of these is architecture and mainboard independent
$(obj)/mainboard/$(MAINBOARDDIR)/static.c: $(src)/mainboard/$(MAINBOARDDIR)/devicetree.cb  $(objutil)/sconfig/sconfig
	@printf "    SCONFIG    $(subst $(src)/,,$(<))\n"
	mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(objutil)/sconfig/sconfig $(MAINBOARDDIR) $(obj)/mainboard/$(MAINBOARDDIR)

$(objutil)/%.o: $(objutil)/%.c
	@printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) -MMD -I$(subst $(objutil)/,util/,$(dir $<)) -I$(dir $<) $(HOSTCFLAGS) -c -o $@ $<

$(obj)/%.ramstage.o: $(obj)/%.c $(obj)/config.h
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD $(CFLAGS) -c -o $@ $<

ramstage-srcs:=$(obj)/mainboard/$(MAINBOARDDIR)/static.c
romstage-srcs:=
driver-srcs:=
smm-srcs:=

ramstage-objs:=
romstage-objs:=
driver-objs:=
smm-objs:=
types:=ramstage romstage driver smm

# Clean -y variables, include Makefile.inc
# Add paths to files in X-y to X-srcs
# Add subdirs-y to subdirs
includemakefiles= \
	$(foreach type,$(2), $(eval $(type)-y:=)) \
	$(eval subdirs-y:=) \
	$(eval -include $(1)) \
	$(foreach type,$(2), \
		$(eval $(type)-srcs+= \
			$$(subst $(top)/,, \
			$$(abspath $$(addprefix $(dir $(1)),$$($(type)-y)))))) \
	$(eval subdirs+=$$(subst $(CURDIR)/,,$$(abspath $$(addprefix $(dir $(1)),$$(subdirs-y)))))

# For each path in $(subdirs) call includemakefiles
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

src-to-obj=$(addsuffix .$(1).o, $(basename $(patsubst src/%, $(obj)/%, $($(1)-srcs))))

ramstage-objs:=$(call src-to-obj,ramstage)
romstage-objs:=$(call src-to-obj,romstage)
driver-objs:=$(call src-to-obj,driver)
smm-objs:=$(call src-to-obj,smm)

allsrcs:=$(foreach var, $(addsuffix -srcs,$(types)), $($(var)))
allobjs:=$(foreach var, $(addsuffix -objs,$(types)), $($(var)))
alldirs:=$(sort $(abspath $(dir $(allobjs))))

define ramstage-objs_asl_template
$(obj)/$(1).ramstage.o: src/$(1).asl
	@printf "    IASL       $$(subst $(top)/,,$$(@))\n"
	$(CPP) -D__ACPI__ -P -include $(abspath $(obj)/config.h) -I$(src) -I$(src)/mainboard/$(MAINBOARDDIR) $$< -o $$(basename $$@).asl
	iasl -p $$(obj)/$(1) -tc $$(basename $$@).asl
	mv $$(obj)/$(1).hex $$(basename $$@).c
	$(CC) $$(CFLAGS) $$(if $$(subst dsdt,,$$(basename $$(notdir $(1)))), -DAmlCode=AmlCode_$$(basename $$(notdir $(1)))) -c -o $$@ $$(basename $$@).c
	# keep %.o: %.c rule from catching the temporary .c file after a make clean
	mv $$(basename $$@).c $$(basename $$@).hex
endef

# macro to define template macros that are used by use_template macro
define create_cc_template
# $1 obj class (ramstage, romstage, driver, smm)
# $2 source suffix (c, S)
# $3 additional compiler flags
de$(EMPTY)fine $(1)-objs_$(2)_template
$(obj)/$$(1).$(1).o: src/$$(1).$(2) $(obj)/config.h
	@printf "    CC         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(CC) $(3) -MMD $$$$(CFLAGS) -c -o $$$$@ $$$$<
en$(EMPTY)def
endef

$(eval $(call create_cc_template,ramstage,c))
$(eval $(call create_cc_template,ramstage,S,-DASSEMBLY))
$(eval $(call create_cc_template,romstage,c,-D__PRE_RAM__))
$(eval $(call create_cc_template,romstage,S,-DASSEMBLY -D__PRE_RAM__))
$(eval $(call create_cc_template,driver,c))
$(eval $(call create_cc_template,driver,S,-DASSEMBLY))
$(eval $(call create_cc_template,smm,c))
$(eval $(call create_cc_template,smm,S))

foreach-src=$(foreach file,$($(1)-srcs),$(eval $(call $(1)-objs_$(subst .,,$(suffix $(file)))_template,$(subst src/,,$(basename $(file))))))
$(eval $(foreach type,$(types),$(call foreach-src,$(type))))

DEPENDENCIES = $(ramstage-objs:.o=.d) $(romstage-objs:.o=.d) $(driver-objs:.o=.d) $(smm-objs:.o=.d)
-include $(DEPENDENCIES)

printall:
	@echo ramstage-objs:=$(ramstage-objs)
	@echo romstage-objs:=$(romstage-objs)
	@echo driver-objs:=$(driver-objs)
	@echo smm-objs:=$(smm-objs)
	@echo alldirs:=$(alldirs)
	@echo allsrcs=$(allsrcs)
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

CFLAGS = $(INCLUDES) -Os -nostdinc -pipe -g
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

coreboot: $(obj)/coreboot.rom

endif

$(shell mkdir -p $(obj) $(objutil)/kconfig/lxdialog $(objutil)/cbfstool $(objutil)/romcc $(objutil)/options $(alldirs))

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

cscope:
	cscope -bR

doxy: doxygen
doxygen:
	$(DOXYGEN) documentation/Doxyfile.coreboot

doxyclean: doxygen-clean
doxygen-clean:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

clean-for-update: doxygen-clean
	rm -f $(ramstage-objs) $(romstage-objs) $(driver-objs) $(smm-objs) .xcompile
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

clean-abuild:
	rm -rf coreboot-builds

clean-cscope:
	rm -f cscope.out

distclean: clean-cscope
	rm -rf $(obj)
	rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig* .ccwrap .xcompile

update:
	dongle.py -c /dev/term/1 $(obj)/coreboot.rom EOF

lint:
	FAILED=0; LINTLOG=`mktemp`; \
	for script in util/lint/lint-*; do \
		echo; echo `basename $$script`; \
		grep "^# DESCR:" $$script | sed "s,.*DESCR: *,," ; \
		echo ========; \
		$$script > $$LINTLOG; \
		if [ `wc -l $$LINTLOG | cut -d' ' -f1` -eq 0 ]; then \
			printf "success\n\n"; \
		else \
			echo test failed: ; \
			cat $$LINTLOG; \
			rm -f $$LINTLOG; \
			FAILED=$$(( $$FAILED + 1 )); \
		fi; \
		echo ========; \
	done; \
	test $$FAILED -eq 0 || { echo "ERROR: $$FAILED test(s) failed." &&  exit 1; }; \
	rm -f $$LINTLOG

# This include must come _before_ the pattern rules below!
# Order _does_ matter for pattern rules.
include util/kconfig/Makefile

$(obj)/ldoptions: $(obj)/config.h
	awk '/^#define ([^"])* ([^"])*$$/ {gsub("\\r","",$$3); print $$2 " = " $$3 ";";}' $< > $@

_WINCHECK=$(shell uname -o 2> /dev/null)
STACK=
ifeq ($(_WINCHECK),Msys)
	STACK=-Wl,--stack,16384000
endif
ifeq ($(_WINCHECK),Cygwin)
	STACK=-Wl,--stack,16384000
endif

$(objutil)/romcc/romcc: $(top)/util/romcc/romcc.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@)) (this may take a while)\n"
	@# Note: Adding -O2 here might cause problems. For details see:
	@# http://www.coreboot.org/pipermail/coreboot/2010-February/055825.html
	$(HOSTCC) -g $(STACK) -Wall -o $@ $<

.PHONY: $(PHONY) clean clean-abuild clean-cscope cscope distclean doxygen doxy coreboot .xcompile

