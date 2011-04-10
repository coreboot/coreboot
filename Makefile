##
## This file is part of the coreboot project.
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2009-2010 coresystems GmbH
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


export KCONFIG_AUTOHEADER := $(obj)/config.h
export KCONFIG_AUTOCONFIG := $(obj)/auto.conf

# directory containing the toplevel Makefile.inc
TOPLEVEL := .

CONFIG_SHELL := sh
KBUILD_DEFCONFIG := configs/defconfig
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

HOSTCC = gcc
HOSTCXX = g++
HOSTCFLAGS := -I$(srck) -I$(objk) -g
HOSTCXXFLAGS := -I$(srck) -I$(objk)
LIBGCC_FILE_NAME := $(shell test -r `$(CC) -print-libgcc-file-name` && $(CC) -print-libgcc-file-name)

DOXYGEN := doxygen
DOXYGEN_OUTPUT_DIR := doxygen

all: real-all

# This include must come _before_ the pattern rules below!
# Order _does_ matter for pattern rules.
include util/kconfig/Makefile

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
include $(TOPLEVEL)/Makefile.inc
real-all: config

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

# The primary target needs to be here before we include the
# other files

ifeq ($(INNER_SCANBUILD),y)
CONFIG_SCANBUILD_ENABLE:=
endif

ifeq ($(CONFIG_SCANBUILD_ENABLE),y)
ifneq ($(CONFIG_SCANBUILD_REPORT_LOCATION),)
CONFIG_SCANBUILD_REPORT_LOCATION:=-o $(CONFIG_SCANBUILD_REPORT_LOCATION)
endif
real-all:
	echo '#!/bin/sh' > .ccwrap
	echo 'CC="$(CC)"' >> .ccwrap
	echo 'if [ "$$1" = "--hostcc" ]; then shift; CC="$(HOSTCC)"; fi' >> .ccwrap
	echo 'if [ "$$1" = "--hostcxx" ]; then shift; CC="$(HOSTCXX)"; fi' >> .ccwrap
	echo 'eval $$CC $$*' >> .ccwrap
	chmod +x .ccwrap
	scan-build $(CONFIG_SCANBUILD_REPORT_LOCATION) -analyze-headers --use-cc=$(top)/.ccwrap --use-c++=$(top)/.ccwrap $(MAKE) INNER_SCANBUILD=y
else
real-all: real-target
endif

# must come rather early
.SECONDEXPANSION:

$(obj)/config.h:
	$(MAKE) oldconfig

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

# Clean -y variables, include Makefile.inc
# Add paths to files in X-y to X-srcs
# Add subdirs-y to subdirs
includemakefiles= \
	$(foreach class,classes subdirs $(classes) $(special-classes), $(eval $(class)-y:=)) \
	$(eval -include $(1)) \
	$(foreach class,$(classes-y), $(call add-class,$(class))) \
	$(foreach class,$(classes), \
		$(eval $(class)-srcs+= \
			$$(subst $(top)/,, \
			$$(abspath $$(addprefix $(dir $(1)),$$($(class)-y)))))) \
	$(foreach special,$(special-classes), \
		$(foreach item,$($(special)-y), $(call $(special)-handler,$(dir $(1)),$(item)))) \
	$(eval subdirs+=$$(subst $(CURDIR)/,,$$(abspath $$(addprefix $(dir $(1)),$$(subdirs-y)))))

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
$(eval $(call evaluate_subdirs))

src-to-obj=$(addsuffix .$(1).o, $(basename $(patsubst src/%, $(obj)/%, $($(1)-srcs))))
$(foreach class,$(classes),$(eval $(class)-objs:=$(call src-to-obj,$(class))))

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
$(obj)/$$(1).$(1).o: src/$$(1).$(2) $(obj)/config.h $(4)
	@printf "    CC         $$$$(subst $$$$(obj)/,,$$$$(@))\n"
	$(CC) $(3) -MMD $$$$(CFLAGS) -c -o $$$$@ $$$$<
en$(EMPTY)def
end$(EMPTY)if
endef

filetypes-of-class=$(subst .,,$(sort $(suffix $($(1)-srcs))))
$(foreach class,$(classes), \
	$(foreach type,$(call filetypes-of-class,$(class)), \
		$(eval $(call create_cc_template,$(class),$(type),$($(class)-$(type)-ccopts),$($(class)-$(type)-deps)))))

foreach-src=$(foreach file,$($(1)-srcs),$(eval $(call $(1)-objs_$(subst .,,$(suffix $(file)))_template,$(subst src/,,$(basename $(file))))))
$(eval $(foreach class,$(classes),$(call foreach-src,$(class))))

DEPENDENCIES = $(allobjs:.o=.d)
-include $(DEPENDENCIES)

printall:
	@$(foreach class,$(classes),echo $(class)-objs:=$($(class)-objs); )
	@echo alldirs:=$(alldirs)
	@echo allsrcs=$(allsrcs)
	@echo DEPENDENCIES=$(DEPENDENCIES)
	@echo LIBGCC_FILE_NAME=$(LIBGCC_FILE_NAME)
	@$(foreach class,$(special-classes),echo $(class):='$($(class))'; )

endif

$(shell mkdir -p $(obj) $(objutil)/kconfig/lxdialog $(additional-dirs) $(alldirs))

cscope:
	cscope -bR

doxy: doxygen
doxygen:
	$(DOXYGEN) documentation/Doxyfile.coreboot

doxyclean: doxygen-clean
doxygen-clean:
	rm -rf $(DOXYGEN_OUTPUT_DIR)

clean-for-update: doxygen-clean clean-for-update-target
	rm -f $(allobjs) .xcompile
	rm -f $(DEPENDENCIES)
	rmdir -p $(alldirs) 2>/dev/null >/dev/null || true

clean: clean-for-update clean-target
	rm -f .ccwrap

clean-cscope:
	rm -f cscope.out

distclean: clean-cscope
	rm -rf $(obj)
	rm -f .config .config.old ..config.tmp .kconfig.d .tmpconfig* .ccwrap .xcompile

.PHONY: $(PHONY) clean clean-cscope cscope distclean doxygen doxy .xcompile
