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

GIT:=$(shell [ -d "$(top)/.git" ] && command -v git)

#######################################################################
# normalize Kconfig variables in a central place
CONFIG_CBFS_PREFIX:=$(call strip_quotes,$(CONFIG_CBFS_PREFIX))
CONFIG_FMDFILE:=$(call strip_quotes,$(CONFIG_FMDFILE))

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
export objcbfs := $(obj)/cbfs/$(CONFIG_CBFS_PREFIX)

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
classes-y := ramstage romstage bootblock smm smmstub cpu_microcode libverstage verstage

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
$(CONFIG_CBFS_PREFIX)/$(1).aml-file = $(obj)/$(1).aml
$(CONFIG_CBFS_PREFIX)/$(1).aml-type = raw
$(CONFIG_CBFS_PREFIX)/$(1).aml-compression = none
cbfs-files-$(if $(2),$(2),y) += $(CONFIG_CBFS_PREFIX)/$(1).aml
-include $(obj)/$(1).d
$(obj)/$(1).aml: $(src)/mainboard/$(MAINBOARDDIR)/$(1).asl $(obj)/config.h
	@printf "    IASL       $$(subst $(top)/,,$$(@))\n"
	$(CC_ramstage) -x assembler-with-cpp -E -MMD -MT $$(@) $$(CPPFLAGS_ramstage) -D__ACPI__ -P -include $(src)/include/kconfig.h -I$(obj) -I$(src) -I$(src)/include -I$(src)/arch/$(ARCHDIR-$(ARCH-ramstage-y))/include -I$(src)/mainboard/$(MAINBOARDDIR) $$< -o $$@
ifeq ($(CONFIG_IASL_WARNINGS_ARE_ERRORS),y)
	cd $$(dir $$@); $(IASL) -we -p $$(notdir $$@) $$(notdir $$@)
else
	cd $$(dir $$@); $(IASL) -p $$(notdir $$@) $$(notdir $$@)
endif
endef

#######################################################################
# Parse plaintext cmos defaults into binary format
# arg1: source file
# arg2: binary file name
cbfs-files-processor-nvramtool= \
	$(eval $(2): $(1) $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout | $(objutil)/nvramtool/nvramtool ; \
		printf "    CREATE     $(2) (from $(1))\n"; \
		$(objutil)/nvramtool/nvramtool -y $(src)/mainboard/$(MAINBOARDDIR)/cmos.layout -D $(2).tmp -p $(1) && \
		mv $(2).tmp $(2))

#######################################################################
# Link VSA binary to ELF-ish stage
# arg1: source file
# arg2: binary file name
cbfs-files-processor-vsa= \
	$(eval $(2): $(1) ; \
		printf "    CREATE     $(2) (from $(1))\n"; \
		$(OBJCOPY_ramstage) --set-start 0x20 --adjust-vma 0x60000 -I binary -O elf32-i386 -B i386 $(1) $(2).tmp && \
		$(LD_ramstage) -m elf_i386 -e 0x60020 --section-start .data=0x60000 $(2).tmp -o $(2))

#######################################################################
# Reduce a .config file to its minimal representation
# arg1: input
# arg2: output
cbfs-files-processor-defconfig= \
	$(eval $(2): $(1) $(obj)/build.h; \
		printf "    CREATE     $(2) (from $(1))\n"; \
		printf "\# This image was built using coreboot " > $(2).tmp && \
		grep "\<COREBOOT_VERSION\>" $(obj)/build.h |cut -d\" -f2 >> $(2).tmp && \
		$(MAKE) DOTCONFIG=$(1) DEFCONFIG=$(2).tmp2 savedefconfig && \
		cat $(2).tmp2 >> $(2).tmp && \
		rm -f $(2).tmp2 && \
		\mv -f $(2).tmp $(2))

#######################################################################
# Add handler for arbitrary files in CBFS
$(call add-special-class,cbfs-files)
cbfs-files-handler= \
		$(eval tmp-cbfs-method:=$(word 2, $(subst :, ,$($(2)-file)))) \
		$(eval $(2)-file:=$(call strip_quotes,$(word 1, $(subst :, ,$($(2)-file))))) \
		$(eval tmp-cbfs-file:= ) \
		$(if $($(2)-file), \
			$(if $(wildcard $(1)$($(2)-file)), \
				$(eval tmp-cbfs-file:= $(wildcard $(1)$($(2)-file))), \
				$(eval tmp-cbfs-file:= $($(2)-file)))) \
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
		$(eval _cbfs-bucket:=regular ) \
		$(if $(strip $($(2)-position)), \
			$(eval _cbfs-bucket:=fixed)) \
		$(if $(strip $($(2)-align)), \
			$(eval _cbfs-bucket:=aligned)) \
		$(if $(tmp-cbfs-method), \
			$(eval tmp-old-cbfs-file:=$(tmp-cbfs-file)) \
			$(eval tmp-cbfs-file:=$(shell mkdir -p $(obj)/mainboard/$(MAINBOARDDIR); mktemp $(obj)/mainboard/$(MAINBOARDDIR)/cbfs-file.XXXXXX).out) \
			$(call cbfs-files-processor-$(tmp-cbfs-method),$(tmp-old-cbfs-file),$(tmp-cbfs-file))) \
		$(if $(tmp-cbfs-file), \
			$(eval cbfs-files-$(_cbfs-bucket) += $(subst $(spc),*,$(tmp-cbfs-file)|$(2)|$($(2)-type)|$($(2)-compression)|$(strip $($(2)-position))|$($(2)-align)|$($(2)-options)))) \
		$(eval $(2)-name:=) \
		$(eval $(2)-type:=) \
		$(eval $(2)-compression:=) \
		$(eval $(2)-position:=) \
		$(eval $(2)-required:=) \
		$(eval $(2)-options:=) \
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
VB_SOURCE ?= 3rdparty/vboot
CPPFLAGS_common += -I$(VB_SOURCE)/firmware/include
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
		   $(objutil)/ifdfake $(objutil)/options $(objutil)/amdfwtool \
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

AMDFWTOOL:=$(objutil)/amdfwtool/amdfwtool
$(AMDFWTOOL): $(top)/util/amdfwtool/amdfwtool.c
	@printf "    HOSTCC     $(subst $(obj)/,,$(@))\n"
	$(HOSTCC) $(HOSTCFLAGS) -DCONFIG_ROM_SIZE=$(CONFIG_ROM_SIZE) -o $@ $<

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
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.* $(obj)/dsdt.*
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

help_toolchain help::
	@echo  '*** Toolchain targets ***'
	@echo  '  crossgcc        - Build coreboot cross-compilers for all platforms'
	@echo  '  crosstools      - Build coreboot cross-compiler and GDB for all platforms'
	@echo  '  crossgcc-clean  - Remove all built coreboot cross-compilers'
	@echo  '  iasl            - Build coreboot IASL compiler (built by all cross targets)'
	@echo  '  clang           - Build coreboot clang compiler'
	@echo  '  test-toolchain  - Reports if toolchain components are out of date'
	@echo  '  crossgcc-ARCH   - Build cross-compiler for specific architecture'
	@echo  '  crosstools-ARCH - Build cross-compiler with GDB for specific architecture'
	@echo  '  ARCH can be "i386", "x64", "arm", "aarch64", "mips", "riscv", or "power8"'
	@echo  '  Use "make [target] CPUS=#" to build toolchain using multiple cores'
	@echo

# For the toolchain builds, use CPUS=x to use multiple processors to build
# use BUILDGCC_OPTIONS= to set any crossgcc command line options
# Example: BUILDGCC_OPTIONS='-t' will keep temporary files after build
crossgcc: clean-for-update
	$(MAKE) -C util/crossgcc all_without_gdb SKIP_CLANG=1

.PHONY: crossgcc crossgcc-i386 crossgcc-x64 crossgcc-arm crossgcc-aarch64 \
	crossgcc-mips crossgcc-riscv crossgcc-power8 crossgcc-clean iasl \
	clang tools crosstools-i386 crosstools-x64 crosstools-arm \
	crosstools-aarch64 crosstools-mips crosstools-riscv crosstools-power8

crossgcc-i386: clean-for-update
	$(MAKE) -C util/crossgcc build-i386 SKIP_GDB=1

crossgcc-x64: clean-for-update
	$(MAKE) -C util/crossgcc build-x64 SKIP_GDB=1

crossgcc-arm: clean-for-update
	$(MAKE) -C util/crossgcc build-armv7a SKIP_GDB=1

crossgcc-aarch64: clean-for-update
	$(MAKE) -C util/crossgcc build-aarch64 SKIP_GDB=1

crossgcc-mips: clean-for-update
	$(MAKE) -C util/crossgcc build-mips SKIP_GDB=1

crossgcc-riscv: clean-for-update
	$(MAKE) -C util/crossgcc build-riscv SKIP_GDB=1

crossgcc-power8: clean-for-update
	$(MAKE) -C util/crossgcc build-power8 SKIP_GDB=1

crosstools: clean-for-update
	$(MAKE) -C util/crossgcc all_with_gdb SKIP_CLANG=1

iasl: clean-for-update
	$(MAKE) -C util/crossgcc build_iasl

clang: clean-for-update
	$(MAKE) -C util/crossgcc build_clang

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

crosstools-power8: clean-for-update
	$(MAKE) -C util/crossgcc build-power8

crossgcc-clean: clean-for-update
	$(MAKE) -C util/crossgcc clean

tools: $(objutil)/kconfig/conf $(CBFSTOOL) $(FMAPTOOL) $(RMODTOOL) $(objutil)/nvramtool/nvramtool $(ROMCC_BIN) $(objutil)/sconfig/sconfig $(IFDTOOL) $(IFDFAKE) $(CBOOTIMAGE) $(AMDFWTOOL)

test-toolchain:
ifeq ($(COMPILER_OUT_OF_DATE),1)
	echo "The coreboot toolchain is not the current version."
	$(error )
else
	echo "The coreboot toolchain is the current version."
endif # ifeq ($(COMPILER_OUT_OF_DATE),1)

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

# extract_nth - Return a subsection of the $file string
#
# the input string looks like this:
# ./build/cbfs/fallback/romstage.elf|fallback/romstage|stage|none||64|--xip*-S*.car.data*-P*0x10000
#
# Sections:
# 1 - Path and name of file [FILENAME: Added to cbfs-files-y list variable]
# 2 - Name of file in cbfs  [$(FILENAME)-file]
# 3 - File type:            [$(FILENAME)-type]
#                bootblock, cbfs header, stage, payload, optionrom, bootsplash, raw, vsa,
#                mbi, microcode, fsp, mrc, cmos_default, cmos_layout, spd, mrc_cache,
#                mma, efi, deleted, null
# 4 - Compression type      [$(FILENAME)-compression]
#                      none, LZMA
# 5 - Base address          [$(FILANAME)-position]
# 6 - Alignment             [$(FILENAME)-align]
# 7 - cbfstool flags        [$(FILENAME)-options]
#
# Input:
#  $(1) = Section to extract
#  $(2) = Input string
#
# Steps:
# 1) replace all '|' characters with the sequence '- -' within the full string, prepended and appended with the character '-'
# 2) extract the specified section from the string - this gets us the section surrounded by '-' characters
# 3) remove the leading and trailing '-' characters
# 4) replace all '*' characters with spaces
extract_nth=$(subst *,$(spc),$(patsubst -%-,%,$(word $(1), $(subst |,- -,-$(2)-))))

cbfs-add-cmd = \
               printf "    CBFS       $(call extract_nth,2,$(file))\n"; \
	       $(CBFSTOOL) $@.tmp \
	       add$(if $(filter stage,$(call extract_nth,3,$(file))),-stage)$(if $(filter payload,$(call extract_nth,3,$(file))),-payload) \
	       -f $(call extract_nth,1,$(file)) \
	       -n $(call extract_nth,2,$(file)) \
	       $(if $(filter-out stage,$(call extract_nth,3,$(file))),-t $(call extract_nth,3,$(file))) \
	       $(if $(call extract_nth,4,$(file)),-c $(call extract_nth,4,$(file))) \
	       $(call extract_nth,7,$(file))

cbfs-files=$(cbfs-files-fixed) $(cbfs-files-aligned) $(cbfs-files-regular)
ifneq ($(CONFIG_UPDATE_IMAGE),y)
prebuild-files = \
	       $(foreach file,$(cbfs-files), \
	       $(if $(call extract_nth,6,$(file)),$(cbfs-add-cmd) -a $(call extract_nth,6,$(file)) &&, \
	       $(cbfs-add-cmd) $(if $(call extract_nth,5,$(file)),-b $(call extract_nth,5,$(file))) &&))
prebuilt-files = $(foreach file,$(cbfs-files), $(call extract_nth,1,$(file)))

ifeq ($(CONFIG_FMDFILE),)
# For a description of the flash layout described by these variables, check
# the $(DEFAULT_FLASHMAP) .fmd files.
ifeq ($(CONFIG_ARCH_X86),y)
DEFAULT_FLASHMAP:=$(top)/util/cbfstool/default-x86.fmd
# entire flash
FMAP_ROM_ADDR := $(call int-subtract, 0x100000000 $(CONFIG_ROM_SIZE))
FMAP_ROM_SIZE := $(CONFIG_ROM_SIZE)
# entire "BIOS" region (everything directly of concern to the host system)
# relative to ROM_BASE
FMAP_BIOS_BASE := $(call int-subtract, $(CONFIG_ROM_SIZE) $(CONFIG_CBFS_SIZE))
FMAP_BIOS_SIZE := $(shell echo $(CONFIG_CBFS_SIZE) | tr A-F a-f)
# position and size of flashmap, relative to BIOS_BASE
FMAP_FMAP_BASE := 0
FMAP_FMAP_SIZE := 0x100
# position and size of CBFS, relative to BIOS_BASE
FMAP_CBFS_BASE := $(FMAP_FMAP_SIZE)
FMAP_CBFS_SIZE := $(call int-subtract, $(FMAP_BIOS_SIZE) $(FMAP_FMAP_SIZE))
else
DEFAULT_FLASHMAP:=$(top)/util/cbfstool/default.fmd
# entire flash
FMAP_ROM_ADDR := 0
FMAP_ROM_SIZE := $(CONFIG_ROM_SIZE)
# entire "BIOS" region (everything directly of concern to the host system)
# relative to ROM_BASE
FMAP_BIOS_BASE := 0
FMAP_BIOS_SIZE := $(CONFIG_CBFS_SIZE)
# position and size of flashmap, relative to BIOS_BASE
FMAP_FMAP_BASE := 0x20000
FMAP_FMAP_SIZE := 0x100
# position and size of CBFS, relative to BIOS_BASE
FMAP_CBFS_BASE := $(call int-add,$(FMAP_FMAP_BASE) $(FMAP_FMAP_SIZE))
FMAP_CBFS_SIZE := $(call int-subtract,$(FMAP_BIOS_SIZE) $(FMAP_CBFS_BASE))
endif

$(obj)/fmap.fmd: $(top)/Makefile.inc $(DEFAULT_FLASHMAP) $(obj)/config.h
	sed -e "s,##ROM_BASE##,$(FMAP_ROM_ADDR)," \
	    -e "s,##ROM_SIZE##,$(FMAP_ROM_SIZE)," \
	    -e "s,##BIOS_BASE##,$(FMAP_BIOS_BASE)," \
	    -e "s,##BIOS_SIZE##,$(FMAP_BIOS_SIZE)," \
	    -e "s,##FMAP_BASE##,$(FMAP_FMAP_BASE)," \
	    -e "s,##FMAP_SIZE##,$(FMAP_FMAP_SIZE)," \
	    -e "s,##CBFS_BASE##,$(FMAP_CBFS_BASE)," \
	    -e "s,##CBFS_SIZE##,$(FMAP_CBFS_SIZE)," \
		$(DEFAULT_FLASHMAP) > $@.tmp
	mv $@.tmp $@
else
$(obj)/fmap.fmd: $(CONFIG_FMDFILE) $(obj)/config.h
	cp $< $@
endif

# generated at the same time as fmap.fmap
$(obj)/fmap.h: $(obj)/fmap.fmap

$(obj)/fmap.fmap: $(obj)/fmap.fmd $(FMAPTOOL)
	echo "    FMAP       $(FMAPTOOL) -h $(obj)/fmap.h $< $@"
	$(FMAPTOOL) -h $(obj)/fmap.h $< $@

$(obj)/coreboot.pre: $(objcbfs)/bootblock.bin $$(prebuilt-files) $(CBFSTOOL) $$(cpu_ucode_cbfs_file) $(obj)/fmap.fmap
	$(CBFSTOOL) $@.tmp create -M $(obj)/fmap.fmap
ifeq ($(CONFIG_ARCH_X86),y)
	$(CBFSTOOL) $@.tmp add \
		-f $(objcbfs)/bootblock.bin \
		-n bootblock \
		-t bootblock \
		-b -$(call file-size,$(objcbfs)/bootblock.bin)
else
	# don't add bootblock to cbfs yet, it's just a waste of space
	true $(CBFSTOOL) $@.tmp add \
		-f $(objcbfs)/bootblock.bin \
		-n bootblock \
		-t bootblock \
		-b 0
	$(CBFSTOOL) $@.tmp write -u \
		-r BOOTBLOCK \
		-f $(objcbfs)/bootblock.bin
	# make space for the CBFS master header pointer. "ptr_" is just
	# arbitrary 4 bytes that will be overwritten by add-master-header.
	printf "ptr_" > $@.tmp.2
	$(CBFSTOOL) $@.tmp add \
		-f $@.tmp.2 \
		-n "header pointer" \
		-t "cbfs header" \
		-b -4
	rm -f $@.tmp.2
endif
	$(CBFSTOOL) $@.tmp add-master-header
	$(prebuild-files) true
	mv $@.tmp $@
else
prebuilt-files = $(foreach file,$(cbfs-files), $(call extract_nth,1,$(file)))
# Add all cbfs files to image of the form: CONFIG_CBFS_PREFIX/<filename>
prebuild-files = \
	$(foreach file,$(cbfs-files), \
	$(CBFSTOOL) $@.tmp remove -n $(call extract_nth,2,$(file)) 2>/dev/null ; \
	       $(if $(call extract_nth,6,$(file)),$(cbfs-add-cmd) -a $(call extract_nth,6,$(file)) &&,\
	       $(cbfs-add-cmd) $(if $(call extract_nth,5,$(file)),-b $(call extract_nth,5,$(file))) &&))

.PHONY: $(obj)/coreboot.pre
$(obj)/coreboot.pre: $$(prebuilt-files) $(CBFSTOOL)
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

$(obj)/coreboot.rom: $(obj)/coreboot.pre $(objcbfs)/ramstage.elf $(CBFSTOOL) $$(INTERMEDIATE)
	@printf "    CBFS       $(subst $(obj)/,,$(@))\n"
# The full ROM may be larger than the CBFS part, so create an empty
# file (filled with \377 = 0xff) and copy the CBFS image over it.
	dd if=/dev/zero bs=$(call _toint,$(CONFIG_ROM_SIZE)) count=1 2> /dev/null | tr '\000' '\377' > $@.tmp
	dd if=$(obj)/coreboot.pre of=$@.tmp bs=8192 conv=notrunc 2> /dev/null
ifneq ($(CONFIG_SEABIOS_PS2_TIMEOUT),)
ifneq ($(CONFIG_SEABIOS_PS2_TIMEOUT),0)
	@printf "    SeaBIOS    Wait up to $(CONFIG_SEABIOS_PS2_TIMEOUT) ms for PS/2 keyboard controller initialization\n"
	$(CBFSTOOL) $@.tmp add-int -i $(CONFIG_SEABIOS_PS2_TIMEOUT) -n etc/ps2-keyboard-spinup
endif
endif
ifeq ($(CONFIG_CPU_INTEL_FIRMWARE_INTERFACE_TABLE),y)
ifeq ($(CONFIG_CPU_MICROCODE_CBFS_GENERATE),y)
	@printf "    UPDATE-FIT \n"
	$(CBFSTOOL) $@.tmp update-fit -n cpu_microcode_blob.bin -x $(CONFIG_CPU_INTEL_NUM_FIT_ENTRIES)
endif
endif
	mv $@.tmp $@
	@printf "    CBFSPRINT  $(subst $(obj)/,,$(@))\n\n"
	$(CBFSTOOL) $@ print
ifeq ($(CONFIG_IASL_WARNINGS_ARE_ERRORS),)
	@printf "\n***** WARNING: IASL warnings as errors is disabled!  *****\n"
	@printf "*****          Please fix the ASL for this platform. *****\n\n"
endif

cbfs-files-y += $(CONFIG_CBFS_PREFIX)/romstage
$(CONFIG_CBFS_PREFIX)/romstage-file := $(objcbfs)/romstage.elf
$(CONFIG_CBFS_PREFIX)/romstage-type := stage
$(CONFIG_CBFS_PREFIX)/romstage-compression := none
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)
$(CONFIG_CBFS_PREFIX)/romstage-options := -b 0
endif
ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32)$(CONFIG_ARCH_ROMSTAGE_X86_64),y)
# Use a 64 byte alignment to provide a minimum alignment
# requirement for the overall romstage. While the first object within
# romstage could have a 4 byte minimum alignment that doesn't mean the linker
# won't decide the entire section should be aligned to a larger value. In the
# future cbfstool should add XIP files proper and honor the alignment
# requirements of the program segment.
#
# Make sure that segment for .car.data is ignored while adding romstage.
$(CONFIG_CBFS_PREFIX)/romstage-align := 64
$(CONFIG_CBFS_PREFIX)/romstage-options := --xip -S .car.data -P $(CONFIG_XIP_ROM_SIZE)
endif

cbfs-files-y += $(CONFIG_CBFS_PREFIX)/ramstage
$(CONFIG_CBFS_PREFIX)/ramstage-file := $(objcbfs)/ramstage.elf
$(CONFIG_CBFS_PREFIX)/ramstage-type := stage
$(CONFIG_CBFS_PREFIX)/ramstage-compression := $(CBFS_COMPRESS_FLAG)

cbfs-files-y += $(CONFIG_CBFS_PREFIX)/payload
$(CONFIG_CBFS_PREFIX)/payload-file := $(CONFIG_PAYLOAD_FILE)
$(CONFIG_CBFS_PREFIX)/payload-type := payload
$(CONFIG_CBFS_PREFIX)/payload-compression := $(CBFS_PAYLOAD_COMPRESS_FLAG)
$(CONFIG_CBFS_PREFIX)/payload-options := $(ADDITIONAL_PAYLOAD_CONFIG)

cbfs-files-$(CONFIG_HAVE_REFCODE_BLOB) += $(CONFIG_CBFS_PREFIX)/refcode
$(CONFIG_CBFS_PREFIX)/refcode-file := $(REFCODE_BLOB)
$(CONFIG_CBFS_PREFIX)/refcode-type := stage
$(CONFIG_CBFS_PREFIX)/refcode-compression := $(CBFS_COMPRESS_FLAG)

cbfs-files-$(CONFIG_SEABIOS_VGA_COREBOOT) += vgaroms/seavgabios.bin
vgaroms/seavgabios.bin-file := $(CONFIG_PAYLOAD_VGABIOS_FILE)
vgaroms/seavgabios.bin-type := raw

cbfs-files-$(CONFIG_PXE_ROM) += pci$(CONFIG_PXE_ROM_ID).rom
pci$(CONFIG_PXE_ROM_ID).rom-file := $(CONFIG_PXE_ROM_FILE)
pci$(CONFIG_PXE_ROM_ID).rom-type := raw

cbfs-files-$(CONFIG_INCLUDE_CONFIG_FILE) += config
config-file := $(DOTCONFIG):defconfig
config-type := raw

cbfs-files-$(CONFIG_INCLUDE_CONFIG_FILE) += revision
revision-file := $(obj)/build.h
revision-type := raw

cbfs-files-$(CONFIG_BOOTSPLASH_IMAGE) += bootsplash.jpg
bootsplash.jpg-file := $(call strip_quotes,$(CONFIG_BOOTSPLASH_FILE))
bootsplash.jpg-type := bootsplash

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
	intelvbttool \
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
