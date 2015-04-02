##
## This file is part of the coreboot project.
##
## Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

romstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
ramstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
romstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
ramstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
romstage-$(CONFIG_ARCH_ROMSTAGE_X86_32) += vboot.c
ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += gnvs.c
romstage-y += fmap.c
ramstage-y += fmap.c
ramstage-$(CONFIG_CHROMEOS_RAMOOPS) += ramoops.c
smm-y += fmap.c
romstage-y += vpd_decode.c cros_vpd.c
ramstage-y += vpd_decode.c cros_vpd.c

ifeq ($(MOCK_TPM),1)
CFLAGS_common += -DMOCK_TPM=1
else
CFLAGS_common += -DMOCK_TPM=0
endif

ifeq ($(CONFIG_VBOOT_VERIFY_FIRMWARE),y)
romstage-y += vboot_helper.c
ramstage-y += vboot_helper.c
romstage-y += vboot_loader.c
rmodules_$(ARCH-romstage-y)-y += vboot_wrapper.c

ifneq ($(CONFIG_SPI_FLASH_MEMORY_MAPPED),y)
VBOOT_MAKEFLAGS = REGION_READ=1
endif

VB_LIB = $(obj)/external/vboot_reference/vboot_fw.a
# Currently, vboot comes into picture only during the romstage, thus
# is compiled for being used in romstage only. Since, we are splitting
# up all components in one of the three stages of coreboot, vboot seems
# most logical to fall under the romstage. Thus, all references to arch
# and other compiler stuff for vboot is using the romstage arch.
VB_FIRMWARE_ARCH := $(ARCHDIR-$(ARCH-romstage-y))
VB_SOURCE := vboot_reference

# Add the vboot include paths.
CPPFLAGS_common += -I$(VB_SOURCE)/firmware/include

VBOOT_STUB_ELF = $(obj)/vendorcode/google/chromeos/vbootstub.elf
VBOOT_STUB = $(VBOOT_STUB_ELF).rmod

# Dependency for the vboot rmodules. Ordering matters.
VBOOT_STUB_DEPS += $(obj)/vendorcode/google/chromeos/vboot_wrapper.rmodules_$(ARCH-romstage-y).o
VBOOT_STUB_DEPS += $(obj)/lib/memcmp.rmodules_$(ARCH-romstage-y).o
VBOOT_STUB_DEPS += $(obj)/arch/x86/lib/memset.rmodules_$(ARCH-romstage-y).o
VBOOT_STUB_DEPS += $(obj)/arch/x86/lib/memcpy.rmodules_$(ARCH-romstage-y).o
VBOOT_STUB_DEPS += $(VB_LIB)
# Remove the '-include' option since that will break vboot's build and ensure
# vboot_reference can get to coreboot's include files.
VBOOT_CFLAGS += $(patsubst -I%,-I$(top)/%,$(filter-out -include $(src)/include/kconfig.h, $(CFLAGS_romstage) $(CPPFLAGS_romstage)))
VBOOT_CFLAGS += -DVBOOT_DEBUG
VBOOT_CFLAGS += $(rmodules_$(ARCH-ROMSTAGE-y)-c-ccopts)

# Link the vbootstub module with a 64KiB-byte heap.
$(eval $(call rmodule_link,$(VBOOT_STUB_ELF), $(VBOOT_STUB_DEPS), 0x10000,$(ARCH-romstage-y)))

# Build vboot library without the default includes from coreboot proper.
$(VB_LIB):
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	$(Q)$(MAKE) -C $(VB_SOURCE) \
		CC="$(CC_romstage)" \
		CFLAGS="$(VBOOT_CFLAGS)" \
		$(VBOOT_MAKEFLAGS) \
		FIRMWARE_ARCH=$(VB_FIRMWARE_ARCH) \
		BUILD=$(top)/$(dir $(VB_LIB)) \
		V=$(V) \
		fwlib

endif

ifeq ($(CONFIG_VBOOT2_VERIFY_FIRMWARE),y)
VB_SOURCE := vboot_reference
CPPFLAGS_common += -I$(VB_SOURCE)/firmware/2lib/include
CPPFLAGS_common += -I$(VB_SOURCE)/firmware/include

verstage-generic-ccopts += -D__PRE_RAM__ -D__VERSTAGE__

ifeq ($(CONFIG_RETURN_FROM_VERSTAGE),y)
bootblock-y += verstub.c chromeos.c
else
verstage-y += verstub.c
endif
verstage-y += verstage.c fmap.c chromeos.c
verstage-y += antirollback.c
verstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
verstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
verstage-$(CONFIG_CHROMEOS_VBNV_FLASH) += vbnv_flash.c
romstage-y += vboot_handoff.c

VB_FIRMWARE_ARCH := $(ARCHDIR-$(ARCH-VERSTAGE-y))
VB2_LIB = $(obj)/external/vboot_reference/vboot_fw2.a
VBOOT_CFLAGS += $(patsubst -I%,-I$(top)/%,$(filter-out -include $(src)/include/kconfig.h, $(CFLAGS_verstage) $(CPPFLAGS_verstage)))
VBOOT_CFLAGS += $(verstage-c-ccopts)
VBOOT_CFLAGS += -include $(top)/src/include/kconfig.h -Wno-missing-prototypes
VBOOT_CFLAGS += -DVBOOT_DEBUG

$(VB2_LIB): $(obj)/config.h
	@printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	$(Q)$(MAKE) -C $(VB_SOURCE) \
		CC="$(CC_verstage)" \
		CFLAGS="$(VBOOT_CFLAGS)" VBOOT2="y" \
		FIRMWARE_ARCH=$(VB_FIRMWARE_ARCH) \
		BUILD=$(top)/$(dir $(VB2_LIB)) \
		V=$(V) \
		fwlib2

VERSTAGE_ELF = $(objcbfs)/verstage.elf
cbfs-files-y += $(call strip_quotes,$(CONFIG_CBFS_PREFIX))/verstage
fallback/verstage-file = $(VERSTAGE_ELF)
fallback/verstage-type = stage
fallback/verstage-compression = none
endif  # CONFIG_VBOOT2_VERIFY_FIRMWARE
