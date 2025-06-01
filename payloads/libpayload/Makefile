##
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

ifneq ($(words $(CURDIR)),1)
	$(error ERROR: Path to the main directory cannot contain spaces)
endif

ifeq ($(INNER_STATIC_ANALYSIS),y)
CC_real:=$(CC)
endif

export top := $(CURDIR)
export coreboottop ?= $(abspath $(top)/../../)
export src := src
export srck := $(abspath $(top)/../../util/kconfig)
export obj ?= build
export objutil ?= $(obj)/util
export objk := $(objutil)/lp_kconfig
export absobj := $(abspath $(obj))
VBOOT_SOURCE ?= $(coreboottop)/3rdparty/vboot

export KCONFIG_AUTOHEADER := $(obj)/config.h
export KCONFIG_AUTOCONFIG := $(obj)/auto.conf
export KCONFIG_DEPENDENCIES := $(obj)/auto.conf.cmd
export KCONFIG_SPLITCONFIG := $(obj)/config/
export KCONFIG_TRISTATE := $(obj)/tristate.conf
export KCONFIG_NEGATIVES := 1
export KBUILD_KCONFIG := Kconfig
export CONFIG_ := CONFIG_LP_

# directory containing the toplevel Makefile.mk
TOPLEVEL := .

CONFIG_LP_SHELL := sh
KBUILD_DEFCONFIG ?= configs/defconfig
UNAME_RELEASE := $(shell uname -r)
DOTCONFIG ?= .config
KCONFIG_CONFIG = $(DOTCONFIG)
export KCONFIG_CONFIG
HAVE_DOTCONFIG := $(wildcard $(DOTCONFIG))
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
HOSTAS ?= as
HOSTLD ?= ld
HOSTNM ?= nm
HOSTOBJCOPY ?= objcopy
HOSTOBJDUMP ?= objdump
HOSTREADELF ?= readelf
HOSTSTRIP ?= strip
HOSTAR ?= ar

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

all: real-all

ifeq ($(INNER_STATIC_ANALYSIS),y)
CC:=$(CC_real)
HOSTCC:=$(CC_real) --hostcc
HOSTCXX:=$(CC_real) --hostcxx
endif

# This include must come _before_ the pattern rules below!
# Order _does_ matter for pattern rules.
include $(srck)/Makefile.mk

include $(HAVE_DOTCONFIG)

ARCHDIR-$(CONFIG_LP_ARCH_ARM)     := arm
ARCHDIR-$(CONFIG_LP_ARCH_ARM64)   := arm64
ARCHDIR-$(CONFIG_LP_ARCH_X86)     := x86
ARCHDIR-$(CONFIG_LP_ARCH_MOCK)    := mock

ARCH-y := $(ARCHDIR-y)

# If architecture folder name is different from xcompile architecture name,
# override here.
ARCH-$(CONFIG_LP_ARCH_ARM)     := arm
ARCH-$(CONFIG_LP_ARCH_ARM64)   := arm64
ARCH-$(CONFIG_LP_ARCH_X86_32)  := x86_32
ARCH-$(CONFIG_LP_ARCH_X86_64)  := x86_64
ARCH-$(CONFIG_LP_ARCH_MOCK)    := mock

# Five cases where we don't need fully populated $(obj) lists:
# 1. when no .config exists
# 2. when make config (in any flavour) is run
# 3. when make distclean is run
# 4. when make help% or make clean% is run
# 5. when make %-test or make %-tests or make %coverage-report is run
# Don't waste time on reading all Makefiles in these cases
ifeq ($(strip $(HAVE_DOTCONFIG)),)
NOCOMPILE := 1
endif
ifneq ($(MAKECMDGOALS),)
ifneq ($(filter %config %clean clean-% help%,$(MAKECMDGOALS)),)
NOCOMPILE := 1
endif
ifneq ($(filter %clean help% clean%, $(MAKECMDGOALS)),)
UNIT_TEST := 1
endif
endif

ifneq ($(filter help%, $(MAKECMDGOALS)),)
NOCOMPILE := 1
UNIT_TEST := 1
else
ifneq ($(filter %-test %-tests %coverage-report, $(MAKECMDGOALS)),)
ifneq ($(filter-out %-test %-tests %coverage-report, $(MAKECMDGOALS)),)
$(error Cannot mix unit-tests targets with other targets)
endif
NOCOMPILE :=
UNIT_TEST := 1
endif
endif

xcompile ?= $(obj)/xcompile
$(xcompile): $(top)/../../util/xcompile/xcompile
	$< $(XGCCPATH) > $@.tmp
	\mv -f $@.tmp $@ 2> /dev/null

ifeq ($(NOCOMPILE),1)
include $(TOPLEVEL)/Makefile.mk
include $(TOPLEVEL)/tests/Makefile.mk
real-all: config

else

ifeq ($(CONFIG_LP_ARCH_MOCK),y)

# Create empty xcompile to satisfy install script
$(shell echo '' > $(xcompile))

CC := $(HOSTCC)
CC-mock := $(HOSTCC)
AS := $(HOSTAS)
AS-mock := $(HOSTAS)
LD := $(HOSTLD)
LD-mock := $(HOSTLD)
NM := $(HOSTNM)
NM-mock := $(HOSTNM)
OBJCOPY := $(HOSTOBJCOPY)
OBJCOPY-mock := $(HOSTOBJCOPY)
OBJDUMP := $(HOSTOBJDUMP)
OBJDUMP-mock := $(HOSTOBJDUMP)
READELF := $(HOSTREADELF)
READELF-mock := $(HOSTEADELF)
STRIP := $(HOSTSTRIP)
STRIP-mock := $(HOSTSTRIP)
AR := $(HOSTAR)
AR-mock := $(HOSTAR)
else

# in addition to the dependency below, create the file if it doesn't exist
# to silence stupid warnings about a file that would be generated anyway.
$(if $(wildcard $(xcompile)),,$(shell	\
	mkdir -p $(dir $(xcompile)) &&	\
	$(top)/../../util/xcompile/xcompile $(XGCCPATH) > $(xcompile) || rm -f $(xcompile)))

include $(xcompile)

ifneq ($(XCOMPILE_COMPLETE),1)
$(shell rm -f $(xcompile))
$(error $(xcompile) deleted because it's invalid. \
	Restarting the build should fix that, or explain the problem)
endif

CC := $(CC_$(ARCH-y))
AS := $(AS_$(ARCH-y))
LD := $(LD_$(ARCH-y))
NM := $(NM_$(ARCH-y))
OBJCOPY := $(OBJCOPY_$(ARCH-y))
OBJDUMP := $(OBJDUMP_$(ARCH-y))
READELF := $(READELF_$(ARCH-y))
STRIP := $(STRIP_$(ARCH-y))
AR := $(AR_$(ARCH-y))
endif

CFLAGS += -std=gnu11 $(CFLAGS_$(ARCH-y))

ifeq ($(CONFIG_LP_COMPILER_LLVM_CLANG),y)
CC:=clang
ifneq ($(CONFIG_LP_ARCH_MOCK),y)
CC += -m32
endif
HOSTCC:=clang
endif

ifeq ($(CONFIG_LP_CCACHE),y)
CCACHE:=$(word 1,$(wildcard $(addsuffix /ccache,$(subst :, ,$(PATH)))))
ifeq ($(CCACHE),)
$(error ccache selected, but not found in PATH)
endif
CCACHE:=CCACHE_COMPILERCHECK=content CCACHE_BASEDIR=$(top) $(CCACHE)
CC := $(CCACHE) $(CC)
HOSTCC := $(CCACHE) $(HOSTCC)
HOSTCXX := $(CCACHE) $(HOSTCXX)
endif

strip_quotes = $(subst ",,$(subst \",,$(1)))

# The primary target needs to be here before we include the
# other files

ifeq ($(INNER_STATIC_ANALYSIS),y)
CONFIG_LP_STATIC_ANALYSIS__ENABLE:=
endif

ifeq ($(CONFIG_LP_STATIC_ANALYSIS_ENABLE),y)
ifneq ($(CONFIG_LP_STATIC_ANALYSIS_REPORT_LOCATION),)
CONFIG_LP_STATIC_ANALYSIS_LOCATION:=-o $(CONFIG_LP_STATIC_ANALYSIS_REPORT_LOCATION)
endif
real-all:
	find src -name '*.c' -o -name '*.h' | xargs clang-tidy $(CONFIG_LP_CLANG_TIDY_CHECKS)
	$(MAKE) INNER_STATIC_ANALYSIS=y
else
real-all: real-target
endif

# must come rather early
.SECONDEXPANSION:

$(KCONFIG_AUTOHEADER): $(KCONFIG_CONFIG)
	$(MAKE) CONFIG_=CONFIG_LP_ olddefconfig
	$(MAKE) CONFIG_=CONFIG_LP_ syncconfig

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

# Clean -y variables, include Makefile.mk
# Add paths to files in X-y to X-srcs
# Add subdirs-y to subdirs
includemakefiles= \
	$(foreach class,classes subdirs $(classes) $(special-classes), $(eval $(class)-y:=)) \
	$(eval -include $(1)) \
	$(foreach class,$(classes-y), $(call add-class,$(class))) \
	$(foreach special,$(special-classes), \
		$(foreach item,$($(special)-y), $(call $(special)-handler,$(dir $(1)),$(item)))) \
	$(foreach class,$(classes), \
		$(eval $(class)-srcs+= \
			$$(subst $(absobj)/,$(obj)/, \
			$$(subst $(top)/,, \
			$$(abspath $$(subst $(dir $(1))/,/,$$(addprefix $(dir $(1)),$$($(class)-y)))))))) \
	$(eval subdirs+=$$(subst $(CURDIR)/,,$$(wildcard $$(abspath $$(addprefix $(dir $(1)),$$(subdirs-y))))))


# For each path in $(subdirs) call includemakefiles
# Repeat until subdirs is empty
evaluate_subdirs= \
	$(eval cursubdirs:=$(subdirs)) \
	$(eval subdirs:=) \
	$(foreach dir,$(cursubdirs), \
		$(eval $(call includemakefiles,$(dir)/Makefile.mk))) \
	$(if $(subdirs),$(eval $(call evaluate_subdirs)))

# collect all object files eligible for building or run unit-tests
ifneq ($(UNIT_TEST),1)
subdirs:=$(TOPLEVEL)
$(eval $(call evaluate_subdirs))
else
include $(TOPLEVEL)/tests/Makefile.mk
endif

# Converts one or more source file paths to the corresponding build/ paths.
# $1 lib name
# $2 file path (list)
src-to-obj=\
	$(addsuffix .$(1).o,\
	$(basename \
	$(addprefix $(obj)/,\
	$(subst $(coreboottop)/,coreboot/,$(2)))))
$(foreach class,$(classes),$(eval $(class)-objs+=$(call src-to-obj,$(class),$($(class)-srcs))))

allsrcs:=$(foreach var, $(addsuffix -srcs,$(classes)), $($(var)))
allobjs:=$(foreach var, $(addsuffix -objs,$(classes)), $($(var)))
alldirs:=$(sort $(abspath $(dir $(allobjs))))

# macro to define template macros that are used by use_template macro
define create_cc_template
# $1 obj class
# $2 source suffix (c, S)
# $3 additional compiler flags
# $4 additional dependencies
ifn$(EMPTY)def $(1)-objs_$(2)_template
de$(EMPTY)fine $(1)-objs_$(2)_template
$$(call src-to-obj,$(1), $$(1).$(2)): $$(1).$(2) $(obj)/libpayload-config.h $(4)
	@printf "    CC         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(CC) $(3) -MMD $$$$(CFLAGS) $(EXTRA_CFLAGS) -c -o $$$$@ $$$$<
en$(EMPTY)def
end$(EMPTY)if
endef

filetypes-of-class=$(subst .,,$(sort $(suffix $($(1)-srcs))))
$(foreach class,$(classes), \
	$(foreach type,$(call filetypes-of-class,$(class)), \
		$(eval $(call create_cc_template,$(class),$(type),$($(class)-$(type)-ccopts),$($(class)-$(type)-deps)))))

foreach-src=$(foreach file,$($(1)-srcs),$(eval $(call $(1)-objs_$(subst .,,$(suffix $(file)))_template,$(basename $(file)))))
$(eval $(foreach class,$(classes),$(call foreach-src,$(class))))

DEPENDENCIES = $($(filter %.o,%(allobjs)):.o=.d)
-include $(DEPENDENCIES)

printall:
	@$(foreach class,$(classes),echo $(class)-objs:=$($(class)-objs); )
	@echo alldirs:=$(alldirs)
	@echo allsrcs=$(allsrcs)
	@echo DEPENDENCIES=$(DEPENDENCIES)
	@$(foreach class,$(special-classes),echo $(class):='$($(class))'; )

endif

$(shell mkdir -p $(KCONFIG_SPLITCONFIG) $(obj) $(objk)/lxdialog $(additional-dirs) $(alldirs))

cscope:
	cscope -bR

doxy: doxygen
doxygen:
	$(DOXYGEN) Doxyfile

doxyclean: doxygen-clean
doxygen-clean:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

clean-for-update: doxygen-clean clean-for-update-target
	rm -f $(allobjs) $(xcompile)
	rm -f $(DEPENDENCIES)
	rmdir -p $(alldirs) 2>/dev/null >/dev/null || true

clean: clean-for-update clean-target
	rm -f .ccwrap junit_config junit_config.old
	rm -rf $(obj)

clean-cscope:
	rm -f cscope.out

distclean: clean-cscope clean
	rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig* .ccwrap .xcompile junit.xml

.PHONY: $(PHONY) clean clean-cscope cscope distclean doxygen doxy
