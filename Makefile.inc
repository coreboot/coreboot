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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

#######################################################################
# misleadingly named, this is the coreboot version
export KERNELVERSION := $(shell if [ -d "$(top)/.git" -a -f "`which git`" ]; \
       then git describe --dirty --always || git describe; \
       else echo 4.0$(KERNELREVISION); fi)

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
coreboot: build-dirs $(obj)/coreboot.rom

#######################################################################
# our phony targets
PHONY+= clean-abuild coreboot lint lint-stable build-dirs

#######################################################################
# root source directories of coreboot
subdirs-y := src/lib src/console src/device src/ec src/southbridge
subdirs-y += src/northbridge src/superio src/drivers src/cpu src/vendorcode
subdirs-y += util/cbfstool util/sconfig util/nvramtool
subdirs-y += src/arch/$(ARCHDIR-y)
subdirs-y += src/mainboard/$(MAINBOARDDIR)

subdirs-y += site-local

#######################################################################
# Add source classes and their build options
classes-y := ramstage romstage bootblock smm smmstub cpu_microcode rmodules

#######################################################################
# Helper functions for ramstage postprocess
spc :=
spc +=
$(spc) :=
$(spc) +=

# files-in-dir-recursive,dir,files
files-in-dir-recursive=$(filter $(1)%,$(2))

# parent-dir,dir/
parent-dir=$(dir $(subst $( ),/,$(strip $(subst /, ,$(1)))))

# filters out exactly the directory specified
# filter-out-dir,dir_to_keep,dirs
filter-out-dir=$(filter-out $(1),$(2))

# filters out dir_to_keep and all its parents
# filter-out-dirs,dir_to_keep,dirs
filter-out-dirs=$(if $(filter-out ./,$(1)),$(call filter-out-dirs,$(call parent-dir,$(1)),$(call filter-out-dir,$(1),$(2))),$(call filter-out-dir,$(1),$(2)))

# dir-wildcards,dirs
dir-wildcards=$(addsuffix %,$(1))

# files-in-dir,dir,files
files-in-dir=$(filter-out $(call dir-wildcards,$(call filter-out-dirs,$(1),$(dir $(2)))),$(call files-in-dir-recursive,$(1),$(2)))

#######################################################################
# reduce command line length by linking the objects of each
# directory into an intermediate file
ramstage-postprocess=$(foreach d,$(sort $(dir $(1))), \
	$(eval $(d)ramstage.o: $(call files-in-dir,$(d),$(1)); $$(LD) -o $$@ -r $$^ ) \
	$(eval ramstage-objs:=$(d)ramstage.o $(filter-out $(call files-in-dir,$(d),$(1)),$(ramstage-objs))))

romstage-c-ccopts:=-D__PRE_RAM__
romstage-S-ccopts:=-D__PRE_RAM__
ifeq ($(CONFIG_TRACE),y)
ramstage-c-ccopts:= -finstrument-functions
endif
ifeq ($(CONFIG_COVERAGE),y)
ramstage-c-ccopts+=-fprofile-arcs -ftest-coverage
endif

ifeq ($(CONFIG_USE_BLOBS),y)
forgetthis:=$(shell git submodule update --init --checkout 3rdparty)
endif

bootblock-c-ccopts:=-D__BOOT_BLOCK__ -D__PRE_RAM__
bootblock-S-ccopts:=-D__BOOT_BLOCK__ -D__PRE_RAM__

smmstub-c-ccopts:=-D__SMM__
smmstub-S-ccopts:=-D__SMM__
smm-c-ccopts:=-D__SMM__
smm-S-ccopts:=-D__SMM__

# SMM TSEG base is dynamic
ifneq ($(CONFIG_SMM_MODULES),y)
ifeq ($(CONFIG_SMM_TSEG),y)
smm-c-ccopts += -fpic
endif
endif

ramstage-c-deps:=$$(OPTION_TABLE_H)
romstage-c-deps:=$$(OPTION_TABLE_H)
bootblock-c-deps:=$$(OPTION_TABLE_H)

#######################################################################
# Add handler to compile ACPI's ASL
define ramstage-objs_asl_template
$(obj)/$(1).ramstage.o: src/$(1).asl $(obj)/config.h
	@printf "    IASL       $$(subst $(top)/,,$$(@))\n"
	$(CC) -x assembler-with-cpp -E -MMD -MT $$(@) -D__ACPI__ -P -include $(src)/include/kconfig.h -I$(obj) -I$(src) -I$(src)/include -I$(src)/arch/$(ARCHDIR-y)/include -I$(src)/mainboard/$(MAINBOARDDIR) $$< -o $$(basename $$@).asl
	cd $$(dir $$@); $(IASL) -p $$(notdir $$@) -tc $$(notdir $$(basename $$@)).asl
	mv $$(basename $$@).hex $$(basename $$@).c
	$(CC) $$(CFLAGS) $$(if $$(subst dsdt,,$$(basename $$(notdir $(1)))), -DAmlCode=AmlCode_$$(basename $$(notdir $(1)))) -c -o $$@ $$(basename $$@).c
	# keep %.o: %.c rule from catching the temporary .c file after a make clean
	mv $$(basename $$@).c $$(basename $$@).hex
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
		printf "    CREATE     $(2) (from $(1))\n";  $(OBJCOPY) --set-start 0x20 --adjust-vma 0x60000 -I binary -O elf32-i386 -B i386 $(1) $(2).tmp && $(LD) -m elf_i386 -e 0x60020 --section-start .data=0x60000 $(2).tmp -o $(2))

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
		$(if $(tmp-cbfs-method), \
			$(eval tmp-old-cbfs-file:=$(tmp-cbfs-file)) \
			$(eval tmp-cbfs-file:=$(shell mkdir -p $(obj)/mainboard/$(MAINBOARDDIR); mktemp $(obj)/mainboard/$(MAINBOARDDIR)/cbfs-file.XXXXXX).out) \
			$(call cbfs-files-processor-$(tmp-cbfs-method),$(tmp-old-cbfs-file),$(tmp-cbfs-file))) \
		$(eval cbfs-files += $(tmp-cbfs-file)|$(2)|$($(2)-type)|$($(2)-compression)|$($(2)-position)) \
		$(eval $(2)-name:=) \
		$(eval $(2)-type:=) \
		$(eval $(2)-compression:=) \
		$(eval $(2)-position:=) \
		$(eval $(2)-required:=)

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
COREBOOT_EXTRA_VERSION := -$(call strip_quotes,$(CONFIG_LOCALVERSION))
endif

INCLUDES := -Isrc -Isrc/include -I$(obj) -Isrc/arch/$(ARCHDIR-y)/include
INCLUDES += -Isrc/device/oprom/include
# abspath is a workaround for romcc
INCLUDES += -include $(src)/include/kconfig.h

CFLAGS = $(INCLUDES) -Os -pipe -g -nostdinc
CFLAGS += -nostdlib -Wall -Wundef -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Wwrite-strings -Wredundant-decls -Wno-trigraphs
CFLAGS += -Wstrict-aliasing -Wshadow
ifeq ($(CONFIG_WARNINGS_ARE_ERRORS),y)
CFLAGS += -Werror
endif
CFLAGS += -fno-common -ffreestanding -fno-builtin -fomit-frame-pointer

additional-dirs := $(objutil)/cbfstool $(objutil)/romcc $(objutil)/ifdtool \
		   $(objutil)/ifdfake $(objutil)/options

#######################################################################
# generate build support files
$(obj)/build.h: .xcompile
	@printf "    GEN        build.h\n"
	rm -f $(obj)/build.h
	printf "/* build system definitions (autogenerated) */\n" > $(obj)/build.ht
	printf "#ifndef __BUILD_H\n" >> $(obj)/build.ht
	printf "#define __BUILD_H\n\n" >> $(obj)/build.ht
	printf "#define COREBOOT_VERSION \"$(KERNELVERSION)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_EXTRA_VERSION \"$(COREBOOT_EXTRA_VERSION)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_BUILD \"`LANG= date`\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_BUILD_YEAR_BCD 0x`LANG= date +"%y"`\n" >> $(obj)/build.ht
	printf "#define COREBOOT_BUILD_MONTH_BCD 0x`LANG= date +"%m"`\n" >> $(obj)/build.ht
	printf "#define COREBOOT_BUILD_DAY_BCD 0x`LANG= date +"%d"`\n" >> $(obj)/build.ht
	printf "#define COREBOOT_BUILD_WEEKDAY_BCD 0x`LANG= date +"%w"`\n" >> $(obj)/build.ht
	printf "#define COREBOOT_DMI_DATE \"`LANG= date +"%m/%d/%Y"`\"\n" >> $(obj)/build.ht
	printf "\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILER \"$(shell LANG= $(CC) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_ASSEMBLER \"$(shell LANG= $(AS) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_LINKER \"$(shell LANG= $(LD) --version | head -n1)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_TIME \"`LANG= date +%T`\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_BY \"$(subst \,@,$(shell PATH=$$PATH:/usr/ucb whoami))\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_HOST \"$(shell hostname -s 2>/dev/null || hostname 2>/dev/null)\"\n" >> $(obj)/build.ht
	printf "#define COREBOOT_COMPILE_DOMAIN \"$(shell test `uname -s` = "Linux" && dnsdomainname || domainname 2>/dev/null)\"\n" >> $(obj)/build.ht
	printf "#endif\n" >> $(obj)/build.ht
	mv $(obj)/build.ht $(obj)/build.h

$(obj)/ldoptions: $(obj)/config.h
	awk '/^#define ([^"])* ([^"])*$$/ {gsub("\\r","",$$3); print $$2 " = " $$3 ";";}' $< > $@

build-dirs:
	mkdir -p $(objcbfs) $(objgenerated)

#######################################################################
# Build the tools
CBFSTOOL:=$(obj)/cbfstool

$(CBFSTOOL): $(objutil)/cbfstool/cbfstool
	cp $< $@

_WINCHECK=$(shell uname -o 2> /dev/null)
STACK=
ifeq ($(_WINCHECK),Msys)
	STACK=-Wl,--stack,16384000
endif
ifeq ($(_WINCHECK),Cygwin)
	STACK=-Wl,--stack,16384000
endif

ROMCC:= $(objutil)/romcc/romcc
$(ROMCC): $(top)/util/romcc/romcc.c
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

#######################################################################
# needed objects that every mainboard uses
# Creation of these is architecture and mainboard independent
$(obj)/mainboard/$(MAINBOARDDIR)/static.c: $(src)/mainboard/$(MAINBOARDDIR)/devicetree.cb  $(objutil)/sconfig/sconfig
	@printf "    SCONFIG    $(subst $(src)/,,$(<))\n"
	mkdir -p $(obj)/mainboard/$(MAINBOARDDIR)
	$(objutil)/sconfig/sconfig $(MAINBOARDDIR) $(obj)/mainboard/$(MAINBOARDDIR)

ramstage-y+=$(obj)/mainboard/$(MAINBOARDDIR)/static.c
romstage-y+=$(obj)/mainboard/$(MAINBOARDDIR)/static.c

$(objutil)/%.o: $(objutil)/%.c
	@printf "    HOSTCC     $(subst $(objutil)/,,$(@))\n"
	$(HOSTCC) -MMD -I$(subst $(objutil)/,util/,$(dir $<)) -I$(dir $<) $(HOSTCFLAGS) -c -o $@ $<

$(obj)/%.ramstage.o $(abspath $(obj))/%.ramstage.o: $(obj)/%.c $(obj)/config.h $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD $(CFLAGS) -c -o $@ $<

$(obj)/%.romstage.o $(abspath $(obj))/%.romstage.o: $(obj)/%.c $(obj)/config.h $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD -D__PRE_RAM__ $(CFLAGS) -c -o $@ $<

$(obj)/%.bootblock.o $(abspath $(obj))/%.bootblock.o: $(obj)/%.c $(obj)/config.h $(OPTION_TABLE_H)
	@printf "    CC         $(subst $(obj)/,,$(@))\n"
	$(CC) -MMD $(bootblock-c-ccopts) $(CFLAGS) -c -o $@ $<

#######################################################################
# Clean up rules
clean-abuild:
	rm -rf coreboot-builds

clean-for-update-target:
	rm -f $(obj)/coreboot_ram* $(obj)/coreboot.romstage $(obj)/coreboot.pre* $(obj)/coreboot.bootblock $(obj)/coreboot.a
	rm -rf $(obj)/bootblock* $(obj)/romstage* $(obj)/location.*
	rm -f $(obj)/option_table.* $(obj)/crt0.S $(obj)/ldscript
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/static.c $(obj)/mainboard/$(MAINBOARDDIR)/config.py $(obj)/mainboard/$(MAINBOARDDIR)/static.dot
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/crt0.s $(obj)/mainboard/$(MAINBOARDDIR)/crt0.disasm
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/romstage.inc
	rm -f $(obj)/mainboard/$(MAINBOARDDIR)/bootblock.* $(obj)/mainboard/$(MAINBOARDDIR)/dsdt.*
	rm -f $(obj)/cpu/x86/smm/smm_bin.c $(obj)/cpu/x86/smm/smm.* $(obj)/cpu/x86/smm/smm
	$(MAKE) -C payloads/external/SeaBIOS -f Makefile.inc clean OUT=$(abspath $(obj)) HOSTCC="$(HOSTCC)" CC="$(CC)" LD="$(LD)"

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
	FAILED=0; LINTLOG=`mktemp .tmpconfig.lintXXXXX`; \
	for script in util/lint/$@-*; do \
		echo; echo `basename $$script`; \
		grep "^# DESCR:" $$script | sed "s,.*DESCR: *,," ; \
		echo ========; \
		$$script > $$LINTLOG; \
		if [ `cat $$LINTLOG | wc -l` -eq 0 ]; then \
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

gitconfig:
	mkdir -p .git/hooks
	for hook in commit-msg pre-commit ; do                       \
		if [ util/gitconfig/$$hook -nt .git/hooks/$$hook -o  \
		! -x .git/hooks/$$hook ]; then			     \
			cp util/gitconfig/$$hook .git/hooks/$$hook;  \
			chmod +x .git/hooks/$$hook;		     \
		fi;						     \
	done
	git config remote.origin.push HEAD:refs/for/master
	(git config --global user.name >/dev/null && git config --global user.email >/dev/null) || (printf 'Please configure your name and email in git:\n\n git config --global user.name "Your Name Comes Here"\n git config --global user.email your.email@example.com\n'; exit 1)

crossgcc: clean-for-update
	$(MAKE) -C util/crossgcc build-without-gdb

crosstools: clean-for-update
	$(MAKE) -C util/crossgcc build

crossgcc-clean: clean-for-update
	$(MAKE) -C util/crossgcc clean

tools: $(objutil)/kconfig/conf $(objutil)/cbfstool/cbfstool $(objutil)/nvramtool/nvramtool $(objutil)/romcc/romcc $(objutil)/sconfig/sconfig

