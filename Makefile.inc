##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 secunet Security Networks AG
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

GIT:=$(shell [ -d "$(top)/.git" ] && command -v git)

#######################################################################
# misleadingly named, this is the coreboot version
export KERNELVERSION := $(strip $(if $(GIT),\
	$(shell git describe --dirty --always || git describe),\
	$(if $(wildcard $(top)/.coreboot-version),\
		$(shell cat $(top)/.coreboot-version),\
		coreboot-unknown$(KERNELREVISION))))

#######################################################################
# Basic component discovery
MAINBOARDDIR=$(call strip_quotes,$(CONFIG_MAINBOARD_DIR))
export MAINBOARDDIR

## Final build results, which CBFSTOOL uses to create the final
## rom image file, are placed under $(objcbfs).
## These typically have suffixes .debug .elf .bin and .map
export objcbfs := $(obj)/cbfs/$(call strip_quotes,$(CONFIG_CBFS_PREFIX))

## Based on the active configuration, Makefile conditionally collects
## the required assembly includes and saves them in a file.
## Such files that do not have a clear one-to-one relation to a source
## file under src/ are placed and built under $(objgenerated)
export objgenerated := $(obj)/generated

#######################################################################
# root rule to resolve if in build mode (ie. configuration exists)
real-target: $(obj)/config.h coreboot
coreboot: build-dirs $(obj)/coreboot.rom $(obj)/cbfstool $(obj)/rmodtool

#######################################################################
# our phony targets
PHONY+= clean-abuild coreboot lint lint-stable build-dirs

#######################################################################
# root source directories of coreboot
subdirs-y := src/lib src/commonlib/ src/console src/device src/acpi
subdirs-y += src/ec/acpi $(wildcard src/ec/*/*) $(wildcard src/southbridge/*/*)
subdirs-y += $(wildcard src/soc/*/*) $(wildcard src/northbridge/*/*)
subdirs-y += src/superio $(wildcard src/drivers/*) src/cpu src/vendorcode
subdirs-y += util/cbfstool util/sconfig util/nvramtool util/broadcom
subdirs-y += $(wildcard src/arch/*)
subdirs-y += src/mainboard/$(MAINBOARDDIR)
subdirs-y += payloads/external

subdirs-y += site-local

#######################################################################
# Add source classes and their build options
classes-y := ramstage romstage bootblock smm smmstub cpu_microcode libverstage verstage secmon

# Add dynamic classes for rmodules
$(foreach supported_arch,$(ARCH_SUPPORTED), \
	    $(eval $(call define_class,rmodules_$(supported_arch),$(supported_arch))))
# Provide a macro to determine environment for free standing rmodules.
$(foreach supported_arch,$(ARCH_SUPPORTED), \
	$(eval rmodules_$(supported_arch)-generic-ccopts += -D__RMODULE__))

#######################################################################
# Helper functions for math and various file placement matters.
# macros work on all formats understood by printf(1)
# values are space separated if using more than one value
#
# int-add:       adds an arbitrary length list of integers
# int-subtract:  subtracts the the second of two integers from the first
# int-multiply:  multiplies an arbitrary length list of integers
# int-divide:    divides the first integer by the second
# int-remainder: arithmetic remainder of the first number divided by the second
# int-lt:        1 if the first value is less than the second.  0 otherwise
# int-gt:        1 if the first values is greater than the second.  0 otherwise
# int-eq:        1 if the two values are equal.  0 otherwise
# int-align:     align $1 to $2 units
# file-size:     returns the filesize of the given file
_toint=$(shell printf "%d" $1)
_int-add2=$(shell expr $(call _toint,$1) + $(call _toint,$2))
int-add=$(if $(filter 1,$(words $1)),$(strip $1),$(call int-add,$(call _int-add2,$(word 1,$1),$(word 2,$1)) $(wordlist 3,$(words $1),$1)))
int-subtract=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) - $(call _toint,$(word 2,$1))))
_int-multiply2=$(shell expr $(call _toint,$1) \* $(call _toint,$2))
int-multiply=$(if $(filter 1,$(words $1)),$(strip $1),$(call int-multiply,$(call _int-multiply2,$(word 1,$1),$(word 2,$1)) $(wordlist 3,$(words $1),$1)))
int-divide=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) / $(call _toint,$(word 2,$1))))
int-remainder=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) % $(call _toint,$(word 2,$1))))
int-lt=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) \< $(call _toint,$(word 2,$1))))
int-gt=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) \> $(call _toint,$(word 2,$1))))
int-eq=$(if $(filter 1,$(words $1)),$(strip $1),$(shell expr $(call _toint,$(word 1,$1)) = $(call _toint,$(word 2,$1))))
int-align=$(shell A=$(call _toint,$1) B=$(call _toint,$2); expr $$A + \( \( $$B - \( $$A % $$B \) \) % $$B \) )
file-size=$(shell cat $1 | wc -c)

#######################################################################
# Helper functions for ramstage postprocess
spc :=
spc +=
$(spc) :=
$(spc) +=

# files-in-dir-recursive,dir,files
files-in-dir-recursive=$(filter $(1)%,$(2))

# parent-dir,dir/
parent-dir=$(dir $(if $(patsubst /%,,$(1)),,/)$(subst $( ),/,$(strip $(subst /, ,$(1)))))

# filters out exactly the directory specified
# filter-out-dir,dir_to_keep,dirs
filter-out-dir=$(filter-out $(1),$(2))

# filters out dir_to_keep and all its parents
# filter-out-dirs,dir_to_keep,dirs
filter-out-dirs=$(if $(filter-out ./ /,$(1)),$(call filter-out-dirs,$(call parent-dir,$(1)),$(call filter-out-dir,$(1),$(2))),$(call filter-out-dir,$(1),$(2)))

# dir-wildcards,dirs
dir-wildcards=$(addsuffix %,$(1))

# files-in-dir,dir,files
files-in-dir=$(filter-out $(call dir-wildcards,$(call filter-out-dirs,$(1),$(sort $(dir $(2))))),$(call files-in-dir-recursive,$(1),$(2)))

#######################################################################
# reduce command line length by linking the objects of each
# directory into an intermediate file
ramstage-postprocess=$$(eval DEPENDENCIES+=$$(addsuffix .d,$$(basename $(1)))) \
	$(foreach d,$(sort $(dir $(filter-out %.ld,$(1)))), \
	$(eval $(d)ramstage.a: $(call files-in-dir,$(d),$(filter-out %.ld,$(1))); rm -f $$@ && $(AR_ramstage) rcsT $$@ $$^ ) \
	$(eval ramstage-objs:=$(d)ramstage.a $(filter-out $(filter-out %.ld, $(call files-in-dir,$(d),$(1))),$(ramstage-objs))))

bootblock-generic-ccopts += -D__PRE_RAM__ -D__BOOTBLOCK__
romstage-generic-ccopts += -D__PRE_RAM__ -D__ROMSTAGE__
ramstage-generic-ccopts += -D__RAMSTAGE__
ifeq ($(CONFIG_TRACE),y)
ramstage-c-ccopts += -finstrument-functions
endif
ifeq ($(CONFIG_COVERAGE),y)
ramstage-c-ccopts += -fprofile-arcs -ftest-coverage
endif

# try to fetch non-optional submodules if the source is under git
forgetthis:=$(if $(GIT),$(shell git submodule update --init))
ifeq ($(CONFIG_USE_BLOBS),y)
# this is necessary because 3rdparty/blobs is update=none, and so is ignored
# unless explicitly requested and enabled through --checkout
forgetthis:=$(if $(GIT),$(shell git submodule update --init --checkout 3rdparty/blobs))
endif

ramstage-c-deps:=$$(OPTION_TABLE_H)
romstage-c-deps:=$$(OPTION_TABLE_H)
libverstage-c-deps:=$$(OPTION_TABLE_H)
verstage-c-deps:=$$(OPTION_TABLE_H)
bootblock-c-deps:=$$(OPTION_TABLE_H)

# Add handler to copy linker scripts
define generic-objs_ld_template_gen
de$(EMPTY)fine $(1)-objs_ld_template
$$(call src-to-obj,$1,$$(1).ld): $$(1).ld $(obj)/config.h
	@printf "    CP         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$$(CC_$(1)) -MMD $$(CPPFLAGS_$(1)) $$($(1)-ld-ccopts) $(PREPROCESS_ONLY) -include $(obj)/config.h -MT $$$$@ -o $$$$@ $$$$<
en$(EMPTY)def
endef

# Add handler to deal with archives
define generic-objs_a_template_gen
de$(EMPTY)fine $(1)-objs_a_template
$$(call src-to-obj,$1,$$(1).a): $$(1).a
	@printf "    CP         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	cp $$$$< $$$$@.tmp
	mv $$$$@.tmp $$$$@
en$(EMPTY)def
endef

# Add handler to add no rules for manual files
define generic-objs_manual_template_gen
# do nothing
endef

#######################################################################
# Add handler to compile ACPI's ASL
# arg1: base file name
# arg2: y or n for including in cbfs. defaults to y
define asl_template
$(call strip_quotes,$(CONFIG_CBFS_PREFIX))/$(1).aml-file = $(obj)/$(1).aml
$(call strip_quotes,$(CONFIG_CBFS_PREFIX))/$(1).aml-type = raw
$(call strip_quotes,$(CONFIG_CBFS_PREFIX))/$(1).aml-compression = none
cbfs-files-$(if $(2),$(2),y) += $(call strip_quotes,$(CONFIG_CBFS_PREFIX))/$(1).aml
$(obj)/$(1).aml: $(src)/mainboard/$(MAINBOARDDIR)/$(1).asl $(obj)/config.h
	@printf "    IASL       $$(subst $(top)/,,$$(@))\n"
	$(CC_ramstage) -x assembler-with-cpp -E -MMD -MT $$(@) $$(CPPFLAGS_ramstage) -D__ACPI__ -P -include $(src)/include/kconfig.h -I$(obj) -I$(src) -I$(src)/include -I$(src)/arch/$(ARCHDIR-$(ARCH-ramstage-y))/include -I$(src)/mainboard/$(MAINBOARDDIR) $$< -o $$@
	cd $$(dir $$@); $(IASL) -p $$(notdir $$@) $$(notdir $$@)
endef

#######################################################################
# Parse plaintext cmos defaults into binary format
# arg1: source file
# arg2: binary file name
cbfs-files-processor-nvramtool= \
	$(eval $(2): $(1) $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout | $(objutil)/nvramtool/nvramtool ; \
		printf "    CREATE     $(2) (from $(1))\n"; $(objutil)/nvramtool/nvramtool -y $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout -D $(2).tmp -p $(1) && mv $(2).tmp $(2))

#######################################################################
# Link VSA binary to ELF-ish stage
# arg1: source file
# arg2: binary file name
cbfs-files-processor-vsa= \
	$(eval $(2): $(1) ; \
		printf "    CREATE     $(2) (from $(1))\n";  $(OBJCOPY_ramstage) --set-start 0x20 --adjust-vma 0x60000 -I binary -O elf32-i386 -B i386 $(1) $(2).tmp && $(LD_ramstage) -m elf_i386 -e 0x60020 --section-start .data=0x60000 $(2).tmp -o $(2))

#######################################################################
# Add handler for arbitrary files in CBFS
$(call add-special-class,cbfs-files)
cbfs-files-handler= \
		$(eval tmp-cbfs-method:=$(word 2, $(subst :, ,$($(2)-file)))) \
		$(eval $(2)-file:=$(call strip_quotes,$(word 1, $(subst :, ,$($(2)-file))))) \
		$(if $(wildcard $(1)$($(2)-file)), \
			$(eval tmp-cbfs-file:= $(wildcard $(1)$($(2)-file))), \
			$(eval tmp-cbfs-file:= $($(2)-file))) \
		$(if $(strip $($(2)-required)), \
			$(if $(wildcard $(tmp-cbfs-file)),, \
				$(info This build configuration requires $($(2)-required)) \
				$(eval FAILBUILD:=1) \
			)) \
		$(if $(strip $($(2)-align)), \
			$(if $(strip $($(2)-position)), \
				$(info ERROR: It is not allowed to specify both alignment and position for $($(2)-file)) \
				$(eval FAILBUILD:=1) \
			)) \
		$(if $(tmp-cbfs-method), \
			$(eval tmp-old-cbfs-file:=$(tmp-cbfs-file)) \
			$(eval tmp-cbfs-file:=$(shell mkdir -p $(obj)/mainboard/$(MAINBOARDDIR); mktemp $(obj)/mainboard/$(MAINBOARDDIR)/cbfs-file.XXXXXX).out) \
			$(call cbfs-files-processor-$(tmp-cbfs-method),$(tmp-old-cbfs-file),$(tmp-cbfs-file))) \
		$(eval cbfs-files += $(tmp-cbfs-file)|$(2)|$($(2)-type)|$($(2)-compression)|$(strip $($(2)-position))|$($(2)-align))\
		$(eval $(2)-name:=) \
		$(eval $(2)-type:=) \
		$(eval $(2)-compression:=) \
		$(eval $(2)-position:=) \
		$(eval $(2)-required:=) \
		$(eval $(2)-align:=)

#######################################################################
# a variety of flags for our build
CBFS_COMPRESS_FLAG:=none
ifeq ($(CONFIG_COMPRESS_RAMSTAGE),y)
CBFS_COMPRESS_FLAG:=LZMA
endif

CBFS_PAYLOAD_COMPRESS_FLAG:=none
ifeq ($(CONFIG_COMPRESSED_PAYLOAD_LZMA),y)
CBFS_PAYLOAD_COMPRESS_FLAG:=LZMA
endif

ifneq ($(CONFIG_LOCALVERSION),"")
export COREBOOT_EXTRA_VERSION := -$(call strip_quotes,$(CONFIG_LOCALVERSION))
endif

CPPFLAGS_common := -Isrc -Isrc/include -Isrc/commonlib/include -I$(obj)
CPPFLAGS_common += -Isrc/device/oprom/include
CPPFLAGS_common += -include $(src)/include/kconfig.h

CFLAGS_common += -pipe -g -nostdinc
CFLAGS_common += -nostdlib -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS_common += -Wwrite-strings -Wredundant-decls -Wno-trigraphs
CFLAGS_common += -Wstrict-aliasing -Wshadow
CFLAGS_common += -fno-common -ffreestanding -fno-builtin -fomit-frame-pointer
CFLAGS_common += -ffunction-sections -fdata-sections

LDFLAGS_common := --gc-sections -nostdlib -nostartfiles -static --emit-relocs

ifeq ($(CONFIG_COMPILER_GCC),y)
# cf. commit f69a99db (coreboot: x86: enable gc-sections)
CFLAGS_common += -Wno-unused-but-set-variable
endif

ifeq ($(CONFIG_WARNINGS_ARE_ERRORS),y)
CFLAGS_common += -Werror
endif
ifneq ($(GDB_DEBUG),)
CFLAGS_common += -Og
else
CFLAGS_common += -Os
endif

additional-dirs := $(objutil)/cbfstool $(objutil)/romcc $(objutil)/ifdtool \
		   $(objutil)/ifdfake $(objutil)/options $(objutil)/fletcher \
		   $(objutil)/cbootimage $(objutil)/bimgtool

#######################################################################
# generate build support files
$(obj)/build.h: .xcompile
	@printf "    GEN        build.h\n"
	rm -f $(obj)/build.h
	util/genbuild_h/genbuild_h.sh > $(obj)/build.ht
	mv $(obj)/build.ht $(obj)/build.h

build-dirs:
	mkdir -p $(objcbfs) $(objgenerated)

#######################################################################
# Build the tools
CBFSTOOL:=$(objutil)/cbfstool/cbfstool
FMAPTOOL:=$(objutil)/cbfstool/fmaptool
RMODTOOL:=$(objutil)/cbfstool/rmodtool

$(obj)/cbfstool: $(CBFSTOOL)
	cp $< $@

$(obj)/fmaptool: $(FMAPTOOL)
	cp $< $@

$(obj)/rmodtool: $(RMODTOOL)
	cp $< $@

_WINCHECK=$(shell uname -o 2> /dev/null)
STACK=
ifeq ($(_WINCHECK),Msys)
	STACK=-Wl,--stack,16384000
endif
ifeq ($(_WINCHECK),Cygwin)
	STACK=-Wl,--stack,16384000
endif

# this allows ccache to prepend itself
# (ccache handling happens first)
ROMCC_BIN= $(objutil)/romcc/romcc
ROMCC?=$(ROMCC_BIN)
$(ROMCC_BIN): $(top)/util/romcc/romcc.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@)) (this may take a while)\n"
	@# Note: Adding -O2 here might cause problems. For details see:
	@# http://www.coreboot.org/pipermail/coreboot/2010-February/055825.html
	$(HOSTCC) -g $(STACK) -Wall -o $@ $<

IFDTOOL:=$(objutil)/ifdtool/ifdtool
$(IFDTOOL): $(top)/util/ifdtool/ifdtool.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

IFDFAKE:=$(objutil)/ifdfake/ifdfake
$(IFDFAKE): $(top)/util/ifdfake/ifdfake.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

FLETCHER:=$(objutil)/fletcher/fletcher
$(FLETCHER): $(top)/util/fletcher/fletcher.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

CBOOTIMAGE:=$(objutil)/cbootimage/cbootimage

subdirs-y += util/nvidia

BIMGTOOL:=$(objutil)/bimgtool/bimgtool
$(BIMGTOOL): $(top)/util/bimgtool/bimgtool.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -o $@ $<

#######################################################################
# needed objects that every mainboard uses
# Creation of these is architecture and mainboard independent
$(obj)/mainboard/$(MAINBOARDDIR)/static.c: $(src)/mainboard/$(MAINBOARDDIR)/devicetree.cb  $(objutil)/sconfig/sconfig
	@printf "    SCONFIG    $(subst $(src)/,,$(<))\n"
	mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(objutil)/sconfig/sconfig $(MAINBOARDDIR) $(obj)/mainboard/$(MAINBOARDDIR)

ramstage-y+=$(obj)/mainboard/$(MAINBOARDDIR)/static.c
romstage-y+=$(obj)/mainboard/$(MAINBOARDDIR)/static.c
verstage-y+=$(obj)/mainboard/$(MAINBOARDDIR)/static.c

$(obj)/%.ramstage.o $(abspath $(obj))/%.ramstage.o: $(obj)/%.c $(obj)/config.h $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_ramstage) -MMD $(CFLAGS_ramstage) $(CPPFLAGS_ramstage) $(ramstage-c-ccopts) -c -o $@ $<

$(obj)/%.romstage.o $(abspath $(obj))/%.romstage.o: $(obj)/%.c $(obj)/config.h $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_romstage) -MMD $(CFLAGS_romstage) $(CPPFLAGS_romstage) $(romstage-c-ccopts) -c -o $@ $<

$(obj)/%.bootblock.o $(abspath $(obj))/%.bootblock.o: $(obj)/%.c $(obj)/config.h $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC_bootblock) -MMD $(CFLAGS_bootblock) $(CPPFLAGS_bootblock) $(bootblock-c-ccopts) -c -o $@ $<

$(objgenerated)/libverstage.a: $$(libverstage-objs)
	rm -f $@
	$(AR_libverstage) rcsT $@ $^

#######################################################################
# Clean up rules
clean-abuild:
	rm -rf coreboot-builds

clean-for-update-target:
	rm -f $(obj)/ramstage* $(obj)/coreboot.romstage $(obj)/coreboot.pre* $(obj)/coreboot.bootblock $(obj)/coreboot.a
	rm -rf $(obj)/bootblock* $(obj)/romstage* $(obj)/location.*
	rm -f $(obj)/option_table.* $(obj)/crt0.S $(obj)/ldscript
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/static.c $(obj)/mainboard/$(MAINBOARDDIR)/config.py $(obj)/mainboard/$(MAINBOARDDIR)/static.dot
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s $(obj)/mainboard/$(MAINBOARDDIR)/crt0.disasm
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.* $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.*
	rm -f $(obj)/cpu/x86/smm/smm_bin.c $(obj)/cpu/x86/smm/smm.* $(obj)/cpu/x86/smm/smm
	$(MAKE) -C payloads/external/SeaBIOS -f Makefile.inc clean

clean-target:
	rm -f $(obj)/coreboot*

#######################################################################
# Development utilities
printcrt0s:
	@echo crt0s=$(crt0s)
	@echo ldscripts=$(ldscripts)

update:
	dongle.py -c /dev/term/1 $(obj)/coreboot.rom EOF

lint lint-stable:
	util/lint/lint $@

gitconfig:
	[ -d .git ]
	mkdir -p .git/hooks
	for hook in commit-msg pre-commit ; do                       \
		if [ util/gitconfig/$$hook -nt .git/hooks/$$hook -o  \
		! -x .git/hooks/$$hook ]; then			     \
			sed -e "s,%MAKE%,$(MAKE),g" util/gitconfig/$$hook > .git/hooks/$$hook;  \
			chmod +x .git/hooks/$$hook;		     \
		fi;						     \
	done
	# Now set up thehooks for 3rdparty/blobs
	if [ -d .git/modules/3rdparty -a \
	\( util/gitconfig/commit-msg -nt .git/modules/3rdparty/hooks/commit-msg -o \
	! -x .git/modules/3rdparty/hooks/commit-msg \) ]; then \
		sed -e "s,%MAKE%,$(MAKE),g" util/gitconfig/commit-msg > .git/modules/3rdparty/hooks/commit-msg; \
		chmod +x .git/modules/3rdparty/hooks/commit-msg; \
	fi
	[ -d 3rdparty/blobs ] && cd 3rdparty/blobs && git config remote.origin.push HEAD:refs/for/master
	git config remote.origin.push HEAD:refs/for/master
	(git config --global user.name >/dev/null && git config --global user.email >/dev/null) || (printf 'Please configure your name and email in git:\n\n git config --global user.name "Your Name Comes Here"\n git config --global user.email your.email@example.com\n'; exit 1)

crossgcc: crossgcc-i386 crossgcc-x64 crossgcc-arm crossgcc-aarch64 crossgcc-mips crossgcc-riscv

.PHONY: crossgcc-i386 crossgcc-x64 crossgcc-arm crossgcc-aarch64 crossgcc-mips crossgcc-riscv
crossgcc-i386: clean-for-update
	$(MAKE) -C util/crossgcc build-i386-without-gdb

crossgcc-x64: clean-for-update
	$(MAKE) -C util/crossgcc build-x64-without-gdb

crossgcc-arm: clean-for-update
	$(MAKE) -C util/crossgcc build-armv7a-without-gdb

crossgcc-aarch64: clean-for-update
	$(MAKE) -C util/crossgcc build-aarch64-without-gdb

crossgcc-mips: clean-for-update
	$(MAKE) -C util/crossgcc build-mips-without-gdb

crossgcc-riscv: clean-for-update
	$(MAKE) -C util/crossgcc build-riscv-without-gdb

crosstools: crosstools-i386 crosstools-x64 crosstools-arm crosstools-aarch64 crosstools-mips crosstools-riscv

.PHONY: crosstools-i386 crosstools-x64 crosstools-arm crosstools-aarch64 crosstools-mips crosstools-riscv
crosstools-i386: clean-for-update
	$(MAKE) -C util/crossgcc build-i386

crosstools-x64: clean-for-update
	$(MAKE) -C util/crossgcc build-x64

crosstools-arm: clean-for-update
	$(MAKE) -C util/crossgcc build-armv7a

crosstools-aarch64: clean-for-update
	$(MAKE) -C util/crossgcc build-aarch64

crosstools-mips: clean-for-update
	$(MAKE) -C util/crossgcc build-mips

crosstools-riscv: clean-for-update
	$(MAKE) -C util/crossgcc build-riscv

crossgcc-clean: clean-for-update
	$(MAKE) -C util/crossgcc clean

tools: $(objutil)/kconfig/conf $(CBFSTOOL) $(FMAPTOOL) $(RMODTOOL) $(objutil)/nvramtool/nvramtool $(ROMCC_BIN) $(objutil)/sconfig/sconfig $(IFDTOOL) $(IFDFAKE) $(CBOOTIMAGE)

###########################################################################
# Common recipes for all stages
###########################################################################

# loadaddr can determine the load address of a stage, which may be needed for
# platform-specific image headers (only works *after* the stage has been built)
loadaddr = $(shell $(OBJDUMP_$(1)) -p $(objcbfs)/$(1).debug | \
	     sed -ne '/LOAD/s/^.*vaddr 0x\([0-9a-fA-F]\{8\}\).*$$/0x\1/p')

# find-substr is required for stages like romstage_null and romstage_xip to
# eliminate the _* part of the string
find-substr = $(word 1,$(subst _, ,$(1)))

# find-class is used to identify the class from the name of the stage
# The input to this macro can be something like romstage.x or romstage.x.y
# find-class recursively strips off the suffixes to extract the exact class name
# e.g.: if romstage.x is provided to find-class, it will remove .x and return romstage
# if romstage.x.y is provided, it will first remove .y, call find-class with romstage.x
# and remove .x the next time and finally return romstage
find-class = $(if $(filter $(1),$(basename $(1))),$(if $(CC_$(1)), $(1), $(call find-substr,$(1))),$(call find-class,$(basename $(1))))

# Bootblocks are not CBFS stages. coreboot is currently expecting the bss to
# be cleared by the loader of the stage. For ARM SoCs that means one needs to
# include the bss section in the binary so the BootROM clears the bss on
# loading of the bootblock stage. Achieve this by marking the bss section
# loadable,allocatable, and data. Do the same for the .data section in case
# it's marked as NOBITS.
$(objcbfs)/bootblock.raw.bin: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(notdir $(@))\n"
	$(OBJCOPY_bootblock) --set-section-flags .bss=load,alloc,data --set-section-flags .data=load,alloc,data $< $<.tmp
	$(OBJCOPY_bootblock) -O binary $<.tmp $@

$(objcbfs)/%.bin: $(objcbfs)/%.raw.bin
	cp $< $@

$(objcbfs)/%.elf: $(objcbfs)/%.debug
	$(eval class := $(call find-class,$(@F)))
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	cp $< $@.tmp
	$(NM_$(class)) -n $@.tmp | sort > $(basename $@).map
	$(OBJCOPY_$(class)) --strip-debug $@.tmp
	$(OBJCOPY_$(class)) --add-gnu-debuglink=$< $@.tmp
	mv $@.tmp $@

###########################################################################
# Build the final rom image
###########################################################################

COREBOOT_ROM_DEPENDENCIES:=

extract_nth=$(patsubst -%-,%,$(word $(1), $(subst |,- -,-$(2)-)))

cbfs-add-cmd = \
	       $(CBFSTOOL) $@.tmp \
	       add$(if $(filter stage,$(call extract_nth,3,$(file))),-stage)$(if $(filter payload,$(call extract_nth,3,$(file))),-payload) \
	       -f $(call extract_nth,1,$(file)) \
	       -n $(call extract_nth,2,$(file)) \
	       $(if $(filter-out stage,$(call extract_nth,3,$(file))),-t $(call extract_nth,3,$(file))) \
	       $(if $(call extract_nth,4,$(file)),-c $(call extract_nth,4,$(file)))

ifneq ($(CONFIG_UPDATE_IMAGE),y)
prebuild-files = \
	       $(foreach file,$(cbfs-files), \
	       $(if $(call extract_nth,6,$(file)),$(cbfs-add-cmd) -a $(call extract_nth,6,$(file)) &&, \
	       $(cbfs-add-cmd) $(if $(call extract_nth,5,$(file)),-b $(call extract_nth,5,$(file))) &&))
prebuilt-files = $(foreach file,$(cbfs-files), $(call extract_nth,1,$(file)))

$(obj)/coreboot.pre1: $(objcbfs)/bootblock.bin $$(prebuilt-files) $(FMAPTOOL) $(CBFSTOOL) $$(cpu_ucode_cbfs_file)
	$(CBFSTOOL) $@.tmp create \
	-B $(objcbfs)/bootblock.bin \
	$(CBFSTOOL_PRE1_OPTS)
	$(prebuild-files) true
	mv $@.tmp $@
else
prebuilt-files = $(foreach file,$(cbfs-files), $(call extract_nth,1,$(file)))
# Add all cbfs files to image of the form: CONFIG_CBFS_PREFIX/<filename>
prebuild-files = \
	$(foreach file,$(cbfs-files), \
	       $(if $(filter $(call strip_quotes, $(CONFIG_CBFS_PREFIX))/%,\
	           $(call extract_nth,2,$(file))), \
	       $(if $(call extract_nth,6,$(file)),$(cbfs-add-cmd) -a $(call extract_nth,6,$(file)) &&,\
	       $(cbfs-add-cmd) $(if $(call extract_nth,5,$(file)),-b $(call extract_nth,5,$(file))) &&)))

.PHONY: $(obj)/coreboot.pre1
$(obj)/coreboot.pre1: $$(prebuilt-files) $(CBFSTOOL)
	mv $(obj)/coreboot.rom $@.tmp
	$(prebuild-files) true
	mv $@.tmp $@
endif

ifeq ($(CONFIG_PAYLOAD_LINUX),y)
ifneq ($(strip $(call strip_quotes,$(CONFIG_LINUX_COMMAND_LINE))),)
      ADDITIONAL_PAYLOAD_CONFIG+=-C $(CONFIG_LINUX_COMMAND_LINE)
endif
ifneq ($(strip $(call strip_quotes,$(CONFIG_LINUX_INITRD))),)
      ADDITIONAL_PAYLOAD_CONFIG+=-I $(CONFIG_LINUX_INITRD)
endif
endif

ifeq ($(CONFIG_HAVE_REFCODE_BLOB),y)
REFCODE_BLOB=$(obj)/refcode.rmod
$(REFCODE_BLOB): $(RMODTOOL)
	$(RMODTOOL) -i $(CONFIG_REFCODE_BLOB_FILE) -o $@
endif

$(obj)/coreboot.rom: $(obj)/coreboot.pre $(objcbfs)/ramstage.elf $(CBFSTOOL) $$(call strip_quotes,$$(COREBOOT_ROM_DEPENDENCIES)) $$(INTERMEDIATE) $(REFCODE_BLOB) $$(VERSTAGE_FILE)
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
# The full ROM may be larger than the CBFS part, so create an empty
# file (filled with \377 = 0xff) and copy the CBFS image over it.
	dd if=/dev/zero bs=$(call _toint,$(CONFIG_ROM_SIZE)) count=1 2> /dev/null | tr '\000' '\377' > $@.tmp
	dd if=$(obj)/coreboot.pre of=$@.tmp bs=8192 conv=notrunc 2> /dev/null
	$(CBFSTOOL) $@.tmp add-stage -f $(objcbfs)/ramstage.elf -n $(CONFIG_CBFS_PREFIX)/ramstage -c $(CBFS_COMPRESS_FLAG)
ifeq ($(CONFIG_PAYLOAD_NONE),y)
	@printf "    PAYLOAD    none (as specified by user)\n"
endif
ifneq ($(CONFIG_PAYLOAD_FILE),)
	@printf "    PAYLOAD    $(CONFIG_PAYLOAD_FILE) (compression: $(CBFS_PAYLOAD_COMPRESS_FLAG))\n"
	$(CBFSTOOL) $@.tmp add-payload -f $(CONFIG_PAYLOAD_FILE) -n $(CONFIG_CBFS_PREFIX)/payload -c $(CBFS_PAYLOAD_COMPRESS_FLAG) $(ADDITIONAL_PAYLOAD_CONFIG)
endif
ifneq ($(CONFIG_SEABIOS_PS2_TIMEOUT),)
ifneq ($(CONFIG_SEABIOS_PS2_TIMEOUT),0)
	@printf "    SeaBIOS    Wait up to $(CONFIG_SEABIOS_PS2_TIMEOUT) ms for PS/2 keyboard controller initialization\n"
	$(CBFSTOOL) $@.tmp add-int -i $(CONFIG_SEABIOS_PS2_TIMEOUT) -n etc/ps2-keyboard-spinup
endif
endif
ifeq ($(CONFIG_SEABIOS_VGA_COREBOOT),y)
	@printf "    SeaBIOS    Adding generated legacy VGA option rom.\n"
	$(CBFSTOOL) $@.tmp add -f $(CONFIG_PAYLOAD_VGABIOS_FILE) -n vgaroms/seavgabios.bin -t raw
endif
ifeq ($(CONFIG_INCLUDE_CONFIG_FILE),y)
	@printf "    CONFIG     $(DOTCONFIG)\n"
	if [ -f $(DOTCONFIG) ]; then \
	echo "# This image was built using git revision" `git rev-parse HEAD` > $(obj)/config.tmp && \
	$(MAKE) DOTCONFIG=$(DOTCONFIG) DEFCONFIG=$(obj)/config.tmp savedefconfig && \
	$(CBFSTOOL) $@.tmp add -f $(obj)/config.tmp -n config -t raw; rm -f $(obj)/config.tmp ; fi
	@printf "    REVISION   build.h\n"
	if [ -f $(obj)/build.h ]; then $(CBFSTOOL) $@.tmp add -f $(obj)/build.h -n revision -t raw; fi
endif
ifeq ($(CONFIG_HAVE_REFCODE_BLOB),y)
	$(CBFSTOOL) $@.tmp add-stage -f $(REFCODE_BLOB) -n $(CONFIG_CBFS_PREFIX)/refcode -c $(CBFS_COMPRESS_FLAG)
endif
ifeq ($(CONFIG_PXE_ROM),y)
	$(CBFSTOOL) $@.tmp add -f $(CONFIG_PXE_ROM_FILE) -n pci$(CONFIG_PXE_ROM_ID).rom -t raw
endif
ifeq ($(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE),y)
ifeq ($(CONFIG_CPU_MICROCODE_ADDED_DURING_BUILD),y)
	@printf "    UPDATE-FIT \n"
	$(CBFSTOOL) $@.tmp update-fit -n cpu_microcode_blob.bin -x $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES)
endif
endif
ifeq ($(CONFIG_INCLUDE_CONFIG_FILE),y)
	@printf "    CONFIG     $(PAYLOAD_CONFIG)\n"
	if [ -f "$(PAYLOAD_CONFIG)" ]; then \
		$(CBFSTOOL) $@.tmp add -f  "$(PAYLOAD_CONFIG)" -n payload_config -t raw; \
	fi
	@printf "    REVISION   $(PAYLOAD_VERSION)\n"
	if [ -f "$(PAYLOAD_VERSION)" ]; then \
		$(CBFSTOOL) $@.tmp add -f "$(PAYLOAD_VERSION)" -n payload_revision -t raw; \
	fi
endif
ifeq ($(CONFIG_SEPARATE_VERSTAGE),y)
	$(CBFSTOOL) $@.tmp add-stage -f $(VERSTAGE_FILE) -n $(CONFIG_CBFS_PREFIX)/verstage $(CBFSTOOL_VERSTAGE_OPTS)
endif
	mv $@.tmp $@
	@printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $@ print

cbfs-files-$(CONFIG_BOOTSPLASH_IMAGE) += bootsplash.jpg
bootsplash.jpg-file := $(call strip_quotes,$(CONFIG_BOOTSPLASH_FILE))
bootsplash.jpg-type := bootsplash

$(obj)/coreboot.pre: $(objcbfs)/romstage.elf $(obj)/coreboot.pre1 $(CBFSTOOL)
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
	cp $(obj)/coreboot.pre1 $@.tmp
	$(CBFSTOOL) $@.tmp add-stage \
		-f $(objcbfs)/romstage.elf \
		-n $(CONFIG_CBFS_PREFIX)/romstage -c none \
		$(CBFSTOOL_PRE_OPTS)
	mv $@.tmp $@

cbfs-files-$(CONFIG_BOARD_ID_MANUAL) += board_id
board_id-file := $(obj)/board_id
board_id-type := raw

$(obj)/board_id:
	printf $(CONFIG_BOARD_ID_STRING) > $@

junit.xml:
	echo "Building $(UTIL)"
	echo '<?xml version="1.0" encoding="utf-8"?><testsuite>' > $@.tmp
	echo "<testcase classname='$(UTIL)' name='$(UTIL)'>" >> $@.tmp
	-$(MAKE) -j $(CPUS) -C "util/$(UTIL)" clean distclean > $@.tmp.2 2>&1
	$(MAKE) -j $(CPUS) -C "util/$(UTIL)" >> $@.tmp.2 2>&1 && type="system-out" || type="failure"; \
	cat $@.tmp.2; \
	if [ "$$type" = "failure" ]; then \
		echo "<failure type='buildFailed'>" >> $@.tmp; \
		echo "Building $(UTIL) Failed"; \
	else \
		echo "<$$type>" >> $@.tmp; \
		echo "Building $(UTIL) Succeeded"; \
	fi; \
	echo '<![CDATA[' >> $@.tmp; \
	cat $@.tmp.2 >> $@.tmp; \
	echo "]]></$$type>" >>$@.tmp
	rm -f $@.tmp.2
	echo "</testcase>" >> $@.tmp
	echo "</testsuite>" >> $@.tmp
	mv $@.tmp "util/$(UTIL)/$@"
	echo

TOOLLIST= \
	cbmem \
	ectool \
	inteltool \
	nvramtool \
	superiotool \
	viatool
JENKINS_PAYLOAD?=none
CPUS?=4
what-jenkins-does:
	util/lint/lint lint-stable --junit
	util/abuild/abuild -B -J $(if $(JENKINS_NOCCACHE),,-y) -c $(CPUS) -z -p $(JENKINS_PAYLOAD) -x -X $(top)/abuild-chromeos.xml
	util/abuild/abuild -B -J $(if $(JENKINS_NOCCACHE),,-y) -c $(CPUS) -z -p $(JENKINS_PAYLOAD)
	(cd payloads/libpayload; unset COREBOOT_BUILD_DIR; $(MAKE) $(if $(JENKINS_NOCCACHE),,CONFIG_LP_CCACHE=y) V=$(V) Q=$(Q) junit.xml)
	$(foreach tool, $(TOOLLIST), $(MAKE) CPUS=$(CPUS) V=$(V) Q=$(Q) UTIL="$(tool)" MFLAGS= MAKEFLAGS= junit.xml; )
