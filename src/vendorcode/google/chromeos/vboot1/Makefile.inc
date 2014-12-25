##
## This file is part of the coreboot project.
##
## Copyright (C) 2014 The ChromiumOS Authors.  All rights reserved.
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

ramstage-y += ../vboot_common.c
romstage-y += vboot_loader.c ../vboot_common.c
rmodules_$(ARCH-ROMSTAGE-y)-y += vboot_wrapper.c ../vboot_common.c

ifneq ($(CONFIG_SPI_FLASH_MEMORY_MAPPED),y)
VBOOT_MAKEFLAGS = REGION_READ=1
endif

VB_LIB = $(obj)/external/vboot_reference/vboot_fw.a
# Currently, vboot comes into picture only during the romstage, thus
# is compiled for being used in romstage only. Since, we are splitting
# up all components in one of the three stages of coreboot, vboot seems
# most logical to fall under the romstage. Thus, all references to arch
# and other compiler stuff for vboot is using the romstage arch.
VB_FIRMWARE_ARCH := $(ARCHDIR-$(ARCH-ROMSTAGE-y))

VBOOT_STUB_ELF = $(obj)/vendorcode/google/chromeos/vboot1/vbootstub.elf
VBOOT_STUB = $(VBOOT_STUB_ELF).rmod

# Dependency for the vboot rmodules. Ordering matters.
VBOOT_STUB_DEPS += $(obj)/vendorcode/google/chromeos/vboot1/vboot_wrapper.rmodules_$(ARCH-ROMSTAGE-y).o
VBOOT_STUB_DEPS += $(obj)/lib/memcmp.rmodules_$(ARCH-ROMSTAGE-y).o
ifeq ($(CONFIG_ARCH_ROMSTAGE_X86_32),y)
VBOOT_STUB_DEPS += $(obj)/arch/x86/lib/memset.rmodules_$(ARCH-ROMSTAGE-y).o
VBOOT_STUB_DEPS += $(obj)/arch/x86/lib/memcpy.rmodules_$(ARCH-ROMSTAGE-y).o
endif
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM),y)
VBOOT_STUB_DEPS += $(obj)/arch/arm/memset.rmodules_$(ARCH-ROMSTAGE-y).o
VBOOT_STUB_DEPS += $(obj)/arch/arm/memcpy.rmodules_$(ARCH-ROMSTAGE-y).o
endif
ifeq ($(CONFIG_ARCH_ROMSTAGE_ARM64),y)
VBOOT_STUB_DEPS += $(obj)/lib/memset.rmodules_$(ARCH-ROMSTAGE-y).o
VBOOT_STUB_DEPS += $(obj)/lib/memcpy.rmodules_$(ARCH-ROMSTAGE-y).o
endif
VBOOT_STUB_DEPS += $(VB_LIB)
# Remove the '-include' option since that will break vboot's build and ensure
# vboot_reference can get to coreboot's include files.
VBOOT_CFLAGS += $(patsubst -I%,-I$(top)/%,$(filter-out -include $(src)/include/kconfig.h, $(CFLAGS_romstage)))
VBOOT_CFLAGS += -DVBOOT_DEBUG
VBOOT_CFLAGS += $(rmodules_$(ARCH-ROMSTAGE-y)-c-ccopts)

# Link the vbootstub module with a 64KiB-byte heap.
$(eval $(call rmodule_link,$(VBOOT_STUB_ELF), $(VBOOT_STUB_DEPS), 0x10000,$(ARCH-ROMSTAGE-y)))

# Build vboot library without the default includes from coreboot proper.
$(VB_LIB):
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	$(Q)FIRMWARE_ARCH=$(VB_FIRMWARE_ARCH) \
	CC="$(CC_romstage)" \
	CFLAGS="$(VBOOT_CFLAGS)" \
	$(MAKE) -C $(VB_SOURCE) \
		$(VBOOT_MAKEFLAGS) \
		BUILD=$(top)/$(dir $(VB_LIB)) \
		V=$(V) \
		fwlib
