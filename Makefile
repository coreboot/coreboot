##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2009-2010 coresystems GmbH
## Copyright (C) 2011 secunet Security Networks AG
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

top := $(CURDIR)
src := src
srck := $(top)/util/kconfig
obj ?= build
override obj := $(subst $(top)/,,$(abspath $(obj)))
objutil ?= $(obj)/util
objk := $(objutil)/kconfig
absobj := $(abspath $(obj))

COREBOOT_EXPORTS := COREBOOT_EXPORTS
COREBOOT_EXPORTS += top src srck obj objutil objk

DOTCONFIG ?= $(top)/.config
KCONFIG_CONFIG = $(DOTCONFIG)
KCONFIG_AUTOHEADER := $(obj)/config.h
KCONFIG_AUTOCONFIG := $(obj)/auto.conf
KCONFIG_DEPENDENCIES := $(obj)/auto.conf.cmd
KCONFIG_SPLITCONFIG := $(obj)/config
KCONFIG_TRISTATE := $(obj)/tristate.conf
KCONFIG_NEGATIVES := 1
KCONFIG_STRICT := 1

COREBOOT_EXPORTS += KCONFIG_CONFIG KCONFIG_AUTOHEADER KCONFIG_AUTOCONFIG
COREBOOT_EXPORTS += KCONFIG_DEPENDENCIES KCONFIG_SPLITCONFIG KCONFIG_TRISTATE
COREBOOT_EXPORTS += KCONFIG_NEGATIVES KCONFIG_STRICT

# directory containing the toplevel Makefile.inc
TOPLEVEL := .

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
UNAME_RELEASE := $(shell uname -r)
HAVE_DOTCONFIG := $(wildcard $(DOTCONFIG))
MAKEFLAGS += -rR --no-print-directory

# Make is silent per default, but 'make V=1' will show all compiler calls.
Q:=@
ifneq ($(V),1)
ifneq ($(Q),)
.SILENT:
endif
endif

# Disable implicit/built-in rules to make Makefile errors fail fast.
.SUFFIXES:

HOSTCC := $(if $(shell type gcc 2>/dev/null),gcc,cc)
HOSTCXX = g++
HOSTCFLAGS := -g
HOSTCXXFLAGS := -g

PREPROCESS_ONLY := -E -P -x assembler-with-cpp -undef -I .

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

export $(COREBOOT_EXPORTS)

all: real-all

help_coreboot help::
	@echo  '*** coreboot platform targets ***'
	@echo  '  Use "make [target] V=1" for extra build debug information'
	@echo  '  all                   - Build coreboot'
	@echo  '  clean                 - Remove coreboot build artifacts'
	@echo  '  distclean             - Remove build artifacts and config files'
	@echo  '  doxygen               - Build doxygen documentation for coreboot'
	@echo  '  doxyplatform          - Build doxygen documentation for the current platform'
	@echo  '  filelist              - Show files used in current build'
	@echo  '  printall              - print makefile info for debugging'
	@echo  '  gitconfig             - set up git to submit patches to coreboot'
	@echo  '  ctags / ctags-project - make ctags file for all of coreboot or current board'
	@echo  '  cscope / cscope-project - make cscope.out file for coreboot or current board'
	@echo

# This include must come _before_ the pattern rules below!
# Order _does_ matter for pattern rules.
include $(srck)/Makefile

# Three cases where we don't need fully populated $(obj) lists:
# 1. when no .config exists
# 2. when make config (in any flavour) is run
# 3. when make distclean is run
# Don't waste time on reading all Makefile.incs in these cases
ifeq ($(strip $(HAVE_DOTCONFIG)),)
NOCOMPILE:=1
endif
ifneq ($(MAKECMDGOALS),)
ifneq ($(filter %config %clean cross% clang iasl gnumake lint% help% what-jenkins-does,$(MAKECMDGOALS)),)
NOCOMPILE:=1
endif
ifeq ($(MAKECMDGOALS), %clean)
NOMKDIR:=1
endif
endif

ifeq ($(NOCOMPILE),1)
include $(TOPLEVEL)/Makefile.inc
include $(TOPLEVEL)/payloads/Makefile.inc
include $(TOPLEVEL)/util/testing/Makefile.inc
-include $(TOPLEVEL)/site-local/Makefile.inc
real-all:
	@echo "Error: Expected config file ($(DOTCONFIG)) not present." >&2
	@echo "Please specify a config file or run 'make menuconfig' to" >&2
	@echo "generate a new config file." >&2
	@exit 1
else

include $(DOTCONFIG)

# in addition to the dependency below, create the file if it doesn't exist
# to silence stupid warnings about a file that would be generated anyway.
$(if $(wildcard .xcompile)$(NOCOMPILE),,$(eval $(shell util/xcompile/xcompile $(XGCCPATH) > .xcompile || rm -f .xcompile)))

.xcompile: util/xcompile/xcompile
	rm -f $@
	$< $(XGCCPATH) > $@.tmp
	\mv -f $@.tmp $@ 2> /dev/null
	rm -f $@.tmp

-include .xcompile

ifneq ($(XCOMPILE_COMPLETE),1)
$(shell rm -f .xcompile)
$(error .xcompile deleted because it's invalid. \
	Restarting the build should fix that, or explain the problem)
endif

ifneq ($(CONFIG_MMX),y)
CFLAGS_x86_32 += -mno-mmx
endif

include toolchain.inc

strip_quotes = $(strip $(subst ",,$(subst \",,$(1))))
# fix makefile syntax highlighting after strip macro \" "))

# The primary target needs to be here before we include the
# other files

real-all: real-target

# must come rather early
.SECONDEXPANSION:
.DELETE_ON_ERROR:

$(KCONFIG_AUTOHEADER): $(KCONFIG_CONFIG)
	+$(MAKE) oldconfig

# Add a new class of source/object files to the build system
add-class= \
	$(eval $(1)-srcs:=) \
	$(eval $(1)-objs:=) \
	$(eval classes+=$(1))

# Special classes are managed types with special behaviour
# On parse time, for each entry in variable $(1)-y
# a handler $(1)-handler is executed with the arguments:
# * $(1): directory the parser is in
# * $(2): current entry
add-special-class= \
	$(eval $(1):=) \
	$(eval special-classes+=$(1))

# Converts one or more source file paths to their corresponding build/ paths.
# Only .ads, adb, .c and .S get converted to .o, other files (like .ld) keep
# their name.
# $1 stage name
# $2 file path (list)
src-to-obj=\
	$(patsubst $(obj)/%,$(obj)/$(1)/%,\
	$(patsubst $(obj)/$(1)/%,$(obj)/%,\
	$(patsubst 3rdparty/%,$(obj)/%,\
	$(patsubst src/%,$(obj)/%,\
	$(patsubst %.ads,%.o,\
	$(patsubst %.adb,%.o,\
	$(patsubst %.c,%.o,\
	$(patsubst %.S,%.o,\
	$(subst .$(1),,$(2))))))))))

# Converts one or more source file paths to the corresponding build/ paths
# of their Ada library information (.ali) files.
# $1 stage name
# $2 file path (list)
src-to-ali=\
	$(patsubst $(obj)/%,$(obj)/$(1)/%,\
	$(patsubst $(obj)/$(1)/%,$(obj)/%,\
	$(patsubst 3rdparty/%,$(obj)/%,\
	$(patsubst src/%,$(obj)/%,\
	$(patsubst %.ads,%.ali,\
	$(patsubst %.adb,%.ali,\
	$(subst .$(1),,\
	$(filter %.ads %.adb,$(2)))))))))

# Clean -y variables, include Makefile.inc
# Add paths to files in X-y to X-srcs
# Add subdirs-y to subdirs
includemakefiles= \
	$(foreach class,classes subdirs $(classes) $(special-classes), $(eval $(class)-y:=)) \
	$(eval -include $(1)) \
	$(foreach class,$(classes-y), $(call add-class,$(class))) \
	$(foreach class,$(classes), \
		$(eval $(class)-srcs+= \
			$$(subst $(absobj)/,$(obj)/, \
			$$(subst $(top)/,, \
			$$(abspath $$(subst $(dir $(1))/,/,$$(addprefix $(dir $(1)),$$($(class)-y)))))))) \
	$(foreach special,$(special-classes), \
		$(foreach item,$($(special)-y), $(call $(special)-handler,$(dir $(1)),$(item)))) \
	$(eval subdirs+=$$(subst $(CURDIR)/,,$$(wildcard $$(abspath $$(addprefix $(dir $(1)),$$(subdirs-y))))))

# For each path in $(subdirs) call includemakefiles
# Repeat until subdirs is empty
evaluate_subdirs= \
	$(eval cursubdirs:=$(subdirs)) \
	$(eval subdirs:=) \
	$(foreach dir,$(cursubdirs), \
		$(eval $(call includemakefiles,$(dir)/Makefile.inc))) \
	$(if $(subdirs),$(eval $(call evaluate_subdirs)))

# collect all object files eligible for building
subdirs:=$(TOPLEVEL)
postinclude-hooks :=
$(eval $(call evaluate_subdirs))
ifeq ($(FAILBUILD),1)
$(error cannot continue build)
endif

# Run hooks registered by subdirectories that need to be evaluated after all files have been parsed
$(eval $(postinclude-hooks))

# Eliminate duplicate mentions of source files in a class
$(foreach class,$(classes),$(eval $(class)-srcs:=$(sort $($(class)-srcs))))

# To track dependencies, we need all Ada specification (.ads) files in
# *-srcs. Extract / filter all specification files that have a matching
# body (.adb) file here (specifications without a body are valid sources
# in Ada).
$(foreach class,$(classes),$(eval $(class)-extra-specs := \
	$(filter \
		$(addprefix %/,$(patsubst %.adb,%.ads,$(notdir $(filter %.adb,$($(class)-srcs))))), \
		$(filter %.ads,$($(class)-srcs)))))
$(foreach class,$(classes),$(eval $(class)-srcs := \
	$(filter-out $($(class)-extra-specs),$($(class)-srcs))))

$(foreach class,$(classes),$(eval $(class)-objs:=$(call src-to-obj,$(class),$($(class)-srcs))))
$(foreach class,$(classes),$(eval $(class)-alis:=$(call src-to-ali,$(class),$($(class)-srcs))))

# For Ada includes
$(foreach class,$(classes),$(eval $(class)-ada-dirs:=$(sort $(dir $(filter %.ads %.adb,$($(class)-srcs)) $($(class)-extra-specs)))))

# Save all objs before processing them (for dependency inclusion)
originalobjs:=$(foreach var, $(addsuffix -objs,$(classes)), $($(var)))

# Call post-processors if they're defined
$(foreach class,$(classes),\
	$(if $(value $(class)-postprocess),$(eval $(call $(class)-postprocess,$($(class)-objs)))))

allsrcs:=$(foreach var, $(addsuffix -srcs,$(classes)), $($(var)))
allobjs:=$(foreach var, $(addsuffix -objs,$(classes)), $($(var)))
alldirs:=$(sort $(abspath $(dir $(allobjs))))

# Reads dependencies from an Ada library information (.ali) file
# Only basenames (with suffix) are preserved so we have to look the
# paths up in $($(stage)-srcs).
# $1 stage name
# $2 ali file
create_ada_deps=$$(if $(2),\
	gnat.adc \
	$$(filter \
		$$(addprefix %/,$$(shell sed -ne's/^D \([^\t]\+\).*$$$$/\1/p' $(2) 2>/dev/null)), \
		$$($(1)-srcs) $$($(1)-extra-specs)))

# macro to define template macros that are used by use_template macro
define create_cc_template
# $1 obj class
# $2 source suffix (c, S, ld, ...)
# $3 additional compiler flags
# $4 additional dependencies
ifn$(EMPTY)def $(1)-objs_$(2)_template
de$(EMPTY)fine $(1)-objs_$(2)_template
ifn$(EMPTY)eq ($(filter ads adb,$(2)),)
$$(call src-to-obj,$1,$$(1).$2): $$(1).$2 $$(call create_ada_deps,$1,$$(call src-to-ali,$1,$$(1).$2)) $(KCONFIG_AUTOHEADER) $(4)
	@printf "    GCC        $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(GCC_$(1)) \
		$$$$(ADAFLAGS_$(1)) $$$$(addprefix -I,$$$$($(1)-ada-dirs)) \
		$(3) -c -o $$$$@ $$$$<
el$(EMPTY)se
$$(call src-to-obj,$1,$$(1).$2): $$(1).$2 $(KCONFIG_AUTOHEADER) $(4)
	@printf "    CC         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(CC_$(1)) \
		-MMD $$$$(CPPFLAGS_$(1)) $$$$(CFLAGS_$(1)) -MT $$$$(@) \
		$(3) -c -o $$$$@ $$$$<
end$(EMPTY)if
en$(EMPTY)def
end$(EMPTY)if
endef

filetypes-of-class=$(subst .,,$(sort $(suffix $($(1)-srcs))))
$(foreach class,$(classes), \
	$(foreach type,$(call filetypes-of-class,$(class)), \
		$(eval $(class)-$(type)-ccopts += $(generic-$(type)-ccopts) $($(class)-generic-ccopts)) \
		$(if $(generic-objs_$(type)_template_gen),$(eval $(call generic-objs_$(type)_template_gen,$(class))),\
		$(eval $(call create_cc_template,$(class),$(type),$($(class)-$(type)-ccopts),$($(class)-$(type)-deps))))))

foreach-src=$(foreach file,$($(1)-srcs),$(eval $(call $(1)-objs_$(subst .,,$(suffix $(file)))_template,$(basename $(file)))))
$(eval $(foreach class,$(classes),$(call foreach-src,$(class))))

# To supported complex package initializations, we need to call the
# emitted code explicitly. gnatbind gathers all the calls for us
# and exports them as a procedure $(stage)_adainit(). Every stage that
# uses Ada code has to call it!
define gnatbind_template
# $1 class
$$(obj)/$(1)/b__$(1).adb: $$$$(filter-out $$(obj)/$(1)/b__$(1).ali,$$$$($(1)-alis))
	@printf "    BIND       $$(subst $$(obj)/,,$$@)\n"
	# We have to give gnatbind a simple filename (without leading
	# path components) so just cd there.
	cd $$(dir $$@) && \
		$$(GNATBIND_$(1)) -a -n \
			--RTS=$$(absobj)/libgnat-$$(ARCH-$(1)-y)/ \
			-L$(1)_ada -o $$(notdir $$@) \
			$$(subst $$(dir $$@),,$$^)
$$(obj)/$(1)/b__$(1).o: $$(obj)/$(1)/b__$(1).adb
	@printf "    GCC        $$(subst $$(obj)/,,$$@)\n"
	$(GCC_$(1)) $$(ADAFLAGS_$(1)) -c -o $$@ $$<
$(1)-objs += $$(obj)/$(1)/b__$(1).o
$($(1)-alis): %.ali: %.o ;
endef

$(eval $(foreach class,$(filter-out libgnat-%,$(classes)), \
	$(if $($(class)-alis),$(call gnatbind_template,$(class)))))

DEPENDENCIES += $(addsuffix .d,$(basename $(allobjs)))
-include $(DEPENDENCIES)

printall:
	@$(foreach class,$(classes), echo $(class)-objs: $($(class)-objs) | tr ' ' '\n'; echo; )
	@echo alldirs: $(alldirs) | tr ' ' '\n'; echo
	@echo allsrcs: $(allsrcs) | tr ' ' '\n'; echo
	@echo DEPENDENCIES: $(DEPENDENCIES) | tr ' ' '\n'; echo
	@$(foreach class,$(special-classes),echo $(class):'$($(class))' | tr ' ' '\n'; echo; )
endif

ifndef NOMKDIR
$(shell mkdir -p $(KCONFIG_SPLITCONFIG) $(objk)/lxdialog $(additional-dirs) $(alldirs))
endif

$(obj)/project_filelist.txt:
	if [ -z "$(wildcard $(obj)/coreboot.rom)" ]; then \
		echo "*** Error: Project must be built before generating file list ***"; \
		exit 1; \
	fi
	find $(obj) -path "$(obj)/util" -prune -o -name "*.d" -exec cat {} \; | \
	  sed "s|$(top)/||" | sed 's/[:\\]/ /g' | sed 's/ /\n/g' | sort | uniq | \
	  grep -v '\.o$$' > $(obj)/project_filelist.txt

filelist: $(obj)/project_filelist.txt
	printf "\nFiles used in build:\n"
	cat $(obj)/project_filelist.txt

#works with either exuberant ctags or ctags.emacs
ctags-project: clean-ctags $(obj)/project_filelist.txt
	cat $(obj)/project_filelist.txt | \
	  xargs ctags -o tags

cscope-project: clean-cscope $(obj)/project_filelist.txt
	cat $(obj)/project_filelist.txt | xargs cscope -b

cscope:
	cscope -bR

doxy: doxygen
doxygen:
	$(DOXYGEN) Documentation/Doxyfile.coreboot

doxygen_simple:
	$(DOXYGEN) Documentation/Doxyfile.coreboot_simple

doxyplatform doxygen_platform: $(obj)/project_filelist.txt
	echo
	echo "Building doxygen documentation for $(CONFIG_MAINBOARD_PART_NUMBER)"
	export DOXYGEN_OUTPUT_DIR="$(DOXYGEN_OUTPUT_DIR)/$(CONFIG_MAINBOARD_VENDOR)/$(CONFIG_MAINBOARD_PART_NUMBER)"; \
	mkdir -p "$$DOXYGEN_OUTPUT_DIR"; \
	export DOXYFILES="$$(cat $(obj)/project_filelist.txt | grep -v '\.ld$$' | sed 's/\.aml/\.dsl/' | tr '\n' ' ')"; \
	export DOXYGEN_PLATFORM="$(CONFIG_MAINBOARD_DIR) ($(CONFIG_MAINBOARD_PART_NUMBER)) version $(KERNELVERSION)"; \
	$(DOXYGEN) Documentation/doxygen/Doxyfile.coreboot_platform

doxyclean: doxygen-clean
doxygen-clean:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

clean-for-update: doxygen-clean clean-for-update-target
	rm -rf $(obj) .xcompile

clean: clean-for-update clean-target clean-utils
	rm -f .ccwrap

clean-cscope:
	rm -f cscope.out

clean-ctags:
	rm -f tags

clean-utils:
	$(foreach tool, $(TOOLLIST), \
		$(MAKE) -C util/$(tool) clean MFLAGS= MAKEFLAGS= ;)

distclean-utils:
	$(foreach tool, $(TOOLLIST), \
		$(MAKE) -C util/$(tool) distclean MFLAGS= MAKEFLAGS= ; \
		rm -f /util/$(tool)/junit.xml;)

distclean: clean clean-ctags clean-cscope distclean-payloads distclean-utils
	rm -f .config .config.old ..config.tmp* .kconfig.d .tmpconfig* .ccwrap .xcompile
	rm -rf coreboot-builds coreboot-builds-chromeos
	rm -f abuild*.xml junit.xml* util/lint/junit.xml

.PHONY: $(PHONY) clean clean-for-update clean-cscope cscope distclean doxygen doxy doxygen_simple
.PHONY: ctags-project cscope-project clean-ctags
