##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 Google Inc
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

# ccache integration
ifeq ($(CONFIG_CCACHE),y)

CCACHE:=$(word 1,$(wildcard $(addsuffix /ccache,$(subst :, ,$(PATH)))))
ifeq ($(CCACHE),)
$(error ccache selected, but not found in PATH)
endif

export CCACHE_COMPILERCHECK=content
export CCACHE_BASEDIR=$(top)

$(foreach arch,$(ARCH_SUPPORTED), \
	$(eval CC_$(arch):=$(CCACHE) $(CC_$(arch))))

HOSTCC:=$(CCACHE) $(HOSTCC)
HOSTCXX:=$(CCACHE) $(HOSTCXX)
ROMCC=$(CCACHE) $(ROMCC_BIN)
endif

# scan-build integration
ifneq ($(CCC_ANALYZER_OUTPUT_FORMAT),)

ifeq ($(CCC_ANALYZER_ANALYSIS),)
export CCC_ANALYZER_ANALYSIS := -analyzer-opt-analyze-headers
endif

$(foreach arch,$(ARCH_SUPPORTED), \
	$(eval CC_$(arch):=CCC_CC="$(CC_$(arch))" $(CC) ))

HOSTCC:=CCC_CC="$(HOSTCC)" $(CC)
HOSTCXX:=CCC_CXX="$(HOSTCXX)" $(CXX)
ROMCC=CCC_CC="$(ROMCC_BIN)" $(CC)
endif

COREBOOT_STANDARD_STAGES := bootblock verstage romstage ramstage

ARCHDIR-i386	:= x86
ARCHDIR-x86_32	:= x86
ARCHDIR-arm	:= arm
ARCHDIR-arm64	:= arm64
ARCHDIR-riscv	:= riscv
ARCHDIR-mips	:= mips

CFLAGS_arm      := -mno-unaligned-access -ffunction-sections -fdata-sections

CFLAGS_arm64 := -ffunction-sections -fdata-sections

CFLAGS_mips	:= -mips32r2 -G 0  -ffunction-sections -fdata-sections
CFLAGS_mips	+= -mno-abicalls -fno-pic

CFLAGS_x86_32 += -ffunction-sections -fdata-sections

toolchain_to_dir = \
	$(foreach arch,$(ARCH_SUPPORTED),\
	$(eval CPPFLAGS_$(arch) += \
	-Isrc/arch/$(ARCHDIR-$(arch))/include))

# set_stage_toolchain: Decides the toolchain to be used by every stage
# E.g.: If bootblock is x86_32, it sets ARCH-BOOTBLOCK-y = x86_32, whereas
# ARCH-BOOTBLOCK-n = armv7. Then, ARCH-BOOTBLOCK-y can be used anywhere to
# decide the compiler toolchain for bootblock stage
# This step is essential for initializing the toolchain for coreboot standard
# stages i.e. bootblock, romstage and ramstage, since it acts as the second
# parameter to create_class_compiler below in init_standard_toolchain
set_stage_toolchain= \
	$(foreach arch,$(ARCH_SUPPORTED),$(eval ARCH-$(1)-$($(shell echo CONFIG_ARCH_$(1)_$(arch) | tr '[:lower:]' '[:upper:]')) := $(arch)))

# create_class_compiler: Used to create compiler tool set for
# special classes
# @1: special class
# @2: compiler set to be used
# e.g.: smm special class uses i386 as compiler set
define create_class_compiler
$(if $(2),,$(error building $(1) without the required toolchain))
CC_$(1) := $(CC_$(2))
LD_$(1) := $(LD_$(2))
NM_$(1) := $(NM_$(2))
AR_$(1) := $(AR_$(2))
OBJCOPY_$(1) := $(OBJCOPY_$(2))
OBJDUMP_$(1) := $(OBJDUMP_$(2))
STRIP_$(1) := $(STRIP_$(2))
READELF_$(1) := $(READELF_$(2))
CFLAGS_$(1) = $$(CFLAGS_common) $$(CFLAGS_$(2))
CPPFLAGS_$(1) = $$(CPPFLAGS_common) $$(CPPFLAGS_$(2))
COMPILER_RT_FLAGS_$(1) :=
ifeq ($(CONFIG_COMPILER_LLVM_CLANG),y)
COMPILER_RT_$(1) = $(shell $(CC_$(2)) $(CFLAGS_$(2)) -print-librt-file-name 2>/dev/null)
else
COMPILER_RT_$(1) = $(wildcard $(shell $(CC_$(2)) $(CFLAGS_$(2)) -print-libgcc-file-name))
ifeq ($(2),x86_32)
COMPILER_RT_FLAGS_$(1) := --wrap __divdi3 --wrap __udivdi3 --wrap __moddi3 --wrap __umoddi3
endif
endif
endef

# define_class: Allows defining any program as dynamic class and compiler tool
# set for the same based on the architecture for which the program is to be
# compiled
# @1: program (class name)
# @2: architecture for which the program needs to be compiled
# IMP: Ensure that define_class is called before any .c or .S files are added to
# the class of the program. Check subdirs-y for order of subdirectory inclusions
define define_class
classes-y += $(1)
$(eval $(call create_class_compiler,$(1),$(2)))
endef

# initialize standard toolchain (CC,AS and others) for give stage
# @1 : stage for which the toolchain is to be initialized
init_standard_toolchain = \
	$(eval $(call set_stage_toolchain,$(1))) \
	$(eval $(call create_class_compiler,$(1),$(ARCH-$(1)-y)))

init_stages = \
	    $(foreach stage,$(COREBOOT_STANDARD_STAGES),$(eval $(call init_standard_toolchain,$(stage))))

$(eval $(call toolchain_to_dir))

$(call init_stages)

# Test for coreboot toolchain (except when explicitely not requested)
ifneq ($(NOCOMPILE),1)
# only run if we're doing a build (not for tests, kconfig, ...), using gcc
# rationale: gcc versions by Linux distributions tend to be quite messed up
COMPILERFAIL:=0
ifeq ($(CONFIG_COMPILER_GCC),y)
ifneq ($(CONFIG_ANY_TOOLCHAIN),y)
$(foreach arch,$(sort $(foreach stage,$(COREBOOT_STANDARD_STAGES),$(ARCH-$(stage)-y))), \
	$(if $(shell $(CC_$(arch)) -v 2>&1 |grep -q "gcc version .*coreboot toolchain" || echo not-coreboot), \
		$(eval COMPILERFAIL:=1)$(warning Please use the coreboot toolchain for '$(arch)' (or prove that your toolchain works))))
endif
endif
endif
ifeq ($(COMPILERFAIL),1)
$(error consider building our compilers: make crossgcc)
endif
