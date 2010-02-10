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

$(if $(wildcard .xcompile),,$(eval $(shell bash util/xcompile/xcompile > .xcompile)))
include .xcompile

export top := $(shell pwd)
export src := $(top)/src
export srck := $(top)/util/kconfig
export obj ?= $(top)/build
export objk := $(obj)/util/kconfig
export sconfig := $(top)/util/sconfig
export yapps2_py := $(sconfig)/yapps2.py
export config_g := $(sconfig)/config.g


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
HOSTCC = gcc
HOSTCXX = g++
HOSTCFLAGS := -I$(srck) -I$(objk) -g
HOSTCXXFLAGS := -I$(srck) -I$(objk)
LIBGCC_FILE_NAME := $(shell $(CC) -print-libgcc-file-name)

DESTDIR = /opt

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

ifeq ($(strip $(HAVE_DOTCONFIG)),)

all: config

else

include $(top)/.config

ARCHDIR-$(CONFIG_ARCH_X86)    := i386
ARCHDIR-$(CONFIG_ARCH_POWERPC) := ppc

MAINBOARDDIR=$(shell echo $(CONFIG_MAINBOARD_DIR))
export MAINBOARDDIR

PLATFORM-y += src/arch/$(ARCHDIR-y) src/cpu src/mainboard/$(MAINBOARDDIR)
TARGETS-y :=

BUILD-y := src/lib src/boot src/console src/devices src/southbridge src/northbridge src/superio src/drivers util/x86emu
BUILD-y += util/cbfstool
BUILD-$(CONFIG_ARCH_X86) += src/pc80

ifneq ($(CONFIG_LOCALVERSION),"")
COREBOOT_EXTRA_VERSION := -$(shell echo $(CONFIG_LOCALVERSION))
endif

# The primary target needs to be here before we include the
# other files

all: coreboot


#######################################################################
# Build the tools

CBFSTOOL:=$(obj)/util/cbfstool/cbfstool

$(obj)/mainboard/$(MAINBOARDDIR)/config.py: $(yapps2_py) $(config_g) 
	mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	python $(yapps2_py) $(config_g) $(obj)/mainboard/$(MAINBOARDDIR)/config.py


# needed objects that every mainboard uses 
# Creation of these is architecture and mainboard independent
$(obj)/mainboard/$(MAINBOARDDIR)/static.c: $(src)/mainboard/$(MAINBOARDDIR)/devicetree.cb  $(obj)/mainboard/$(MAINBOARDDIR)/config.py
	mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	(cd $(obj)/mainboard/$(MAINBOARDDIR) ; PYTHONPATH=$(top)/util/sconfig export PYTHONPATH; python config.py  $(MAINBOARDDIR) $(top) $(obj)/mainboard/$(MAINBOARDDIR))

$(obj)/mainboard/$(MAINBOARDDIR)/static.o: $(obj)/mainboard/$(MAINBOARDDIR)/static.c
#

objs:=$(obj)/mainboard/$(MAINBOARDDIR)/static.o
initobjs:=
drivers:=
smmobjs:=
crt0s:=
ldscripts:=
types:=obj initobj driver smmobj
includemakefiles=$(foreach type,$(2), $(eval $(type)-y:=)) $(eval subdirs-y:=) $(eval include $(1)) $(if $(strip $(3)),$(foreach type,$(2),$(eval $(type)s+=$$(patsubst src/%,$(obj)/%,$$(addprefix $(dir $(1)),$$($(type)-y)))))) $(eval subdirs+=$$(subst $(PWD)/,,$$(abspath $$(addprefix $(dir $(1)),$$(subdirs-y)))))
evaluate_subdirs=$(eval cursubdirs:=$(subdirs)) $(eval subdirs:=) $(foreach dir,$(cursubdirs),$(eval $(call includemakefiles,$(dir)/Makefile.inc,$(types),$(1)))) $(if $(subdirs),$(eval $(call evaluate_subdirs, $(1))))

# collect all object files eligible for building
subdirs:=$(PLATFORM-y) $(BUILD-y)
$(eval $(call evaluate_subdirs, modify))

allobjs:=$(foreach var, $(addsuffix s,$(types)), $($(var)))
alldirs:=$(sort $(abspath $(dir $(allobjs))))
source_with_ext=$(patsubst $(obj)/%.o,src/%.$(1),$(allobjs))
allsrc=$(wildcard $(call source_with_ext,c) $(call source_with_ext,S))

POST_EVALUATION:=y

# fetch rules (protected in POST_EVALUATION) that rely on the variables filled above
subdirs:=$(PLATFORM-y) $(BUILD-y)
$(eval $(call evaluate_subdirs))


define objs_dsl_template
$(obj)/$(1)%.o: src/$(1)%.asl
	@printf "    IASL       $$(subst $$(shell pwd)/,,$$(@))\n"
	$(CPP) -D__ACPI__ -P $(CPPFLAGS) -include $(obj)/config.h -I$(src) -I$(src)/mainboard/$(MAINBOARDDIR) $$< -o $$(basename $$@).asl
	iasl -p $$(basename $$@) -tc $$(basename $$@).asl
	mv $$(basename $$@).hex $$(basename $$@).c
	$(CC) -m32 $$(CFLAGS) $$(if $$(subst dsdt,,$$(basename $$(notdir $$@))), -DAmlCode=AmlCode_$$(basename $$(notdir $$@))) -c -o $$@ $$(basename $$@).c
endef

define objs_c_template
$(obj)/$(1)%.o: src/$(1)%.c $(obj)/build.h
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 $$(CFLAGS) -c -o $$@ $$<
endef

define objs_S_template
$(obj)/$(1)%.o: src/$(1)%.S $(obj)/build.h
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 -DASSEMBLY $$(CFLAGS) -c -o $$@ $$<
endef

define initobjs_c_template
$(obj)/$(1)%.o: src/$(1)%.c $(obj)/build.h
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 $$(CFLAGS) -c -o $$@ $$<
endef

define initobjs_S_template
$(obj)/$(1)%.o: src/$(1)%.S $(obj)/build.h
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 -DASSEMBLY $$(CFLAGS) -c -o $$@ $$<
endef

define drivers_c_template
$(obj)/$(1)%.o: src/$(1)%.c $(obj)/build.h
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 $$(CFLAGS) -c -o $$@ $$<
endef

define drivers_S_template
$(obj)/$(1)%.o: src/$(1)%.S
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 -DASSEMBLY $$(CFLAGS) -c -o $$@ $$<
endef

define smmobjs_c_template
$(obj)/$(1)%.o: src/$(1)%.c
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 $$(CFLAGS) -c -o $$@ $$<
endef

define smmobjs_S_template
$(obj)/$(1)%.o: src/$(1)%.S
	@printf "    CC         $$(subst $$(obj)/,,$$(@))\n"
	$(CC) -m32 $$(CFLAGS) -c -o $$@ $$<
endef

usetemplate=$(foreach d,$(sort $(dir $($(1)))),$(eval $(call $(1)_$(2)_template,$(subst $(obj)/,,$(d)))))
usetemplate=$(foreach d,$(sort $(dir $($(1)))),$(eval $(call $(1)_$(2)_template,$(subst $(obj)/,,$(d)))))
$(eval $(call usetemplate,objs,dsl))
$(eval $(call usetemplate,objs,c))
$(eval $(call usetemplate,objs,S))
$(eval $(call usetemplate,initobjs,c))
$(eval $(call usetemplate,initobjs,S))
$(eval $(call usetemplate,drivers,c))
$(eval $(call usetemplate,drivers,S))
$(eval $(call usetemplate,smmobjs,c))
$(eval $(call usetemplate,smmobjs,S))

printall:
	@echo objs:=$(objs)
	@echo initobjs:=$(initobjs)
	@echo drivers:=$(drivers)
	@echo smmobjs:=$(smmobjs)
	@echo alldirs:=$(alldirs)
	@echo allsrc=$(allsrc)

OBJS     := $(patsubst %,$(obj)/%,$(TARGETS-y))
INCLUDES := -I$(top)/src -I$(top)/src/include -I$(obj) -I$(top)/src/arch/$(ARCHDIR-y)/include 
INCLUDES += -I$(shell $(CC) -print-search-dirs | head -n 1 | cut -d' ' -f2)include
INCLUDES += -I$(top)/util/x86emu/include
INCLUDES += -include $(obj)/build.h

try-run= $(shell set -e; \
TMP=".$$$$.tmp"; \
if ($(1)) > /dev/null 2>&1; \
then echo "$(2)"; \
else echo "$(3)"; \
fi; rm -rf "$$TMP")

cc-option= $(call try-run,\
$(CC) $(1) -S -xc /dev/null -o "$$TMP", $(1), $(2))

STACKPROTECT += $(call cc-option, -fno-stack-protector,)

CFLAGS = $(STACKPROTECT) $(INCLUDES) -Os -nostdinc
CFLAGS += -nostdlib -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Wwrite-strings -Wredundant-decls -Wno-trigraphs 
CFLAGS += -Wstrict-aliasing -Wshadow 
ifeq ($(CONFIG_WARNINGS_ARE_ERRORS),y)
CFLAGS += -Werror
endif
CFLAGS += -fno-common -ffreestanding -fno-builtin -fomit-frame-pointer

CBFS_COMPRESS_FLAG:=l
CBFS_PAYLOAD_COMPRESS_FLAG:=
ifeq ($(CONFIG_COMPRESSED_PAYLOAD_LZMA),y)
CBFS_PAYLOAD_COMPRESS_FLAG:=l
endif

coreboot: prepare prepare2 $(obj)/coreboot.rom

endif

prepare:
	mkdir -p $(obj)
	mkdir -p $(obj)/util/kconfig/lxdialog
	test -n "$(alldirs)" && mkdir -p $(alldirs) || true

prepare2: $(obj)/build.h

$(obj)/build.h:
	@printf "    GEN        build.h\n"
	rm -f $(obj)/build.h
	printf "#define COREBOOT_VERSION \"$(KERNELVERSION)\"\n" > $(obj)/build.ht
	printf "#define COREBOOT_EXTRA_VERSION \"$(COREBOOT_EXTRA_VERSION)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_BUILD \"`LANG= date`\"\n" >> $(obj)/build.ht
	printf "\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILER \"$(shell LANG= $(CC) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_ASSEMBLER \"$(shell LANG= $(AS) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_LINKER \"$(shell LANG= $(LD) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_TIME \"`LANG= date +%T`\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_BY \"$(subst \,@,$(shell PATH=$$PATH:/usr/ucb whoami))\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_HOST \"$(shell hostname -s)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_DOMAIN \"$(shell test `uname -s` = "Linux" && dnsdomainname || domainname)\"\n" >> $(obj)/build.ht
	printf "#include \"config.h\"\n" >> $(obj)/build.ht
	mv $(obj)/build.ht $(obj)/build.h

doxy: doxygen
doxygen:
	$(DOXYGEN) documentation/Doxyfile.coreboot

doxyclean: doxygen-clean
doxygen-clean:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

clean-for-update: doxygen-clean
	rm -f $(objs) $(initobjs) $(drivers) $(smmobjs) .xcompile
	rm -f $(obj)/coreboot_ram* $(obj)/coreboot.romstage $(obj)/coreboot.pre* $(obj)/coreboot.bootblock $(obj)/coreboot.a
	rm -rf $(obj)/bootblock* $(obj)/romstage* $(obj)/location.*
	rm -f $(obj)/option_table.* $(obj)/crt0_includes.h $(obj)/ldscript $(obj)/config.h
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/static.c $(obj)/mainboard/$(MAINBOARDDIR)/config.py $(obj)/mainboard/$(MAINBOARDDIR)/static.dot
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/auto.inc $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s $(obj)/mainboard/$(MAINBOARDDIR)/crt0.disasm
	rmdir -p $(alldirs) 2>/dev/null >/dev/null || true
	$(MAKE) -C util/sconfig clean

clean: clean-for-update
	rm -f $(obj)/coreboot*

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
	$(HOSTCC) -g -O2 $(STACK) -Wall -o $@ $<

.PHONY: $(PHONY) prepare prepare2 clean distclean doxygen doxy coreboot

