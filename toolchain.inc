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

ARCH_SUPPORTED := ARMV7 X86_32

ARCH_TO_TOOLCHAIN_X86_32 := x86_32
ARCH_TO_TOOLCHAIN_ARMV7  := armv7

COREBOOT_STANDARD_STAGES := bootblock romstage ramstage

ARCHDIR-i386    := x86
ARCHDIR-x86_32  := x86
ARCHDIR-armv7   := armv7

CFLAGS_armv7 = \
	-ffixed-r8\
	-march=armv7-a\
	-marm\
	-mno-unaligned-access\
	-mthumb\
	-mthumb-interwork

toolchain_to_dir = \
	$(foreach arch,$(ARCH_SUPPORTED),\
	$(eval INCLUDES_$(ARCH_TO_TOOLCHAIN_$(arch)) = \
	-Isrc/arch/$(ARCHDIR-$(ARCH_TO_TOOLCHAIN_$(arch)))/include))

# set_stage_toolchain: Decides the toolchain to be used by every stage
# E.g.: If bootblock is x86_32, it sets ARCH-BOOTBLOCK-y = x86_32, whereas
# ARCH-BOOTBLOCK-n = armv7. Then, ARCH-BOOTBLOCK-y can be used anywhere to
# decide the compiler toolchain for bootblock stage
# This step is essential for initializing the toolchain for coreboot standard
# stages i.e. bootblock, romstage and ramstage, since it acts as the second
# parameter to create_class_compiler below in init_standard_toolchain
set_stage_toolchain= \
	$(foreach arch,$(ARCH_SUPPORTED),$(eval ARCH-$(1)-$(CONFIG_ARCH_$(1)_$(arch)) := $(ARCH_TO_TOOLCHAIN_$(arch))))

# create_class_compiler: Used to create compiler tool set for
# special classes
# @1: special class
# @2: compiler set to be used
# e.g.: smm special class uses i386 as compiler set
define create_class_compiler
CC_$(1) := $(CC_$(2))
LD_$(1) := $(LD_$(2))
NM_$(1) := $(NM_$(2))
OBJCOPY_$(1) := $(OBJCOPY_$(2))
OBJDUMP_$(1) := $(OBJDUMP_$(2))
STRIP_$(1) := $(STRIP_$(2))
READELF_$(1) := $(READELF_$(2))
INCLUDES_$(1) = -Isrc/arch/$(ARCHDIR-$(2))/include
CFLAGS_$(1) = $$(CFLAGS_common) $$(INCLUDES_$(1)) $(CFLAGS_$(2))
LIBGCC_FILE_NAME_$(1) = $(shell [ -r `$(CC_$(2)) -print-libgcc-file-name` ] && \
	$(CC_$(2)) -print-libgcc-file-name)
endef

# initialize standard toolchain (CC,AS and others) for give stage
# @1 : stage for which the toolchain is to be initialized
init_standard_toolchain = \
	$(eval stage_caps := $(shell printf "%s" $(1) | tr '[:lower:]' '[:upper:]' )) \
	$(eval $(call set_stage_toolchain,$(stage_caps))) \
	$(eval $(call create_class_compiler,$(1),$(ARCH-$(stage_caps)-y))) \
	$(eval $(call set_stage_libgcc,$(1)))

init_stages = \
	    $(foreach stage,$(COREBOOT_STANDARD_STAGES),$(eval $(call init_standard_toolchain,$(stage))))

# This mapping is created to have consistency with xcompile naming
$(eval $(call create_class_compiler,x86_32,i386))

$(eval $(call toolchain_to_dir))

$(call init_stages)
