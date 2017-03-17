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

ifeq ($(CONFIG_VBOOT),y)

bootblock-y += bootmode.c
romstage-y += bootmode.c
ramstage-y += bootmode.c
verstage-y += bootmode.c
postcar-y += bootmode.c

libverstage-generic-ccopts += -D__PRE_RAM__ -D__VERSTAGE__
verstage-generic-ccopts += -D__PRE_RAM__ -D__VERSTAGE__

bootblock-y += vbnv.c
verstage-y += vbnv.c
romstage-y += vbnv.c
ramstage-y += vbnv.c

bootblock-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c
verstage-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c
romstage-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c
ramstage-$(CONFIG_VBOOT_VBNV_CMOS) += vbnv_cmos.c

bootblock-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c
verstage-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c
romstage-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c
ramstage-$(CONFIG_VBOOT_VBNV_CMOS_BACKUP_TO_FLASH) += vbnv_flash.c

bootblock-$(CONFIG_VBOOT_VBNV_EC) += vbnv_ec.c
verstage-$(CONFIG_VBOOT_VBNV_EC) += vbnv_ec.c
romstage-$(CONFIG_VBOOT_VBNV_EC) += vbnv_ec.c
ramstage-$(CONFIG_VBOOT_VBNV_EC) += vbnv_ec.c

bootblock-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c
verstage-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c
romstage-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c
ramstage-$(CONFIG_VBOOT_VBNV_FLASH) += vbnv_flash.c

bootblock-y += vboot_loader.c
romstage-y += vboot_loader.c
ramstage-y += vboot_loader.c
verstage-y += vboot_loader.c
postcar-y += vboot_loader.c

bootblock-y += vboot_common.c
verstage-y += vboot_common.c
romstage-y += vboot_common.c
ramstage-y += vboot_common.c
postcar-y += vboot_common.c

bootblock-y += common.c
libverstage-y += vboot_logic.c
verstage-y += common.c
verstage-y += verstage.c
ifeq (${CONFIG_VBOOT_MOCK_SECDATA},y)
libverstage-y += secdata_mock.c
romstage-$(CONFIG_SEPARATE_VERSTAGE) += secdata_mock.c
else
libverstage-y += secdata_tpm.c
romstage-$(CONFIG_SEPARATE_VERSTAGE) += secdata_tpm.c
endif
romstage-y += vboot_handoff.c common.c

ramstage-y += common.c
postcar-y += common.c

ifeq ($(CONFIG_SEPARATE_VERSTAGE),y)
VB_FIRMWARE_ARCH := $(ARCHDIR-$(ARCH-verstage-y))
else
ifeq ($(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK),y)
VB_FIRMWARE_ARCH := $(ARCHDIR-$(ARCH-bootblock-y))
else
VB_FIRMWARE_ARCH := $(ARCHDIR-$(ARCH-romstage-y))
endif
endif # CONFIG_SEPARATE_VERSTAGE

VB2_LIB = $(obj)/external/vboot_reference/vboot_fw20.a
VBOOT_CFLAGS += $(patsubst -I%,-I$(top)/%, $(filter-out -I$(obj), $(filter-out -include $(src)/include/kconfig.h, $(CPPFLAGS_libverstage))))
VBOOT_CFLAGS += $(CFLAGS_libverstage)
VBOOT_CFLAGS += $(libverstage-c-ccopts)
VBOOT_CFLAGS += -I$(abspath $(obj)) -include $(top)/src/include/kconfig.h -Wno-missing-prototypes
VBOOT_CFLAGS += -DVBOOT_DEBUG

$(VB2_LIB): $(obj)/config.h
	printf "    MAKE       $(subst $(obj)/,,$(@))\n"
	+FIRMWARE_ARCH=$(VB_FIRMWARE_ARCH) \
	CC="$(CC_verstage)" \
	CFLAGS="$(VBOOT_CFLAGS)" VBOOT2="y" \
	$(MAKE) -C $(VBOOT_SOURCE) \
		BUILD=$(abspath $(dir $(VB2_LIB))) \
		V=$(V) \
		fwlib20

libverstage-srcs += $(VB2_LIB)

ifeq ($(CONFIG_SEPARATE_VERSTAGE),y)

# This works under the assumption that romstage and verstage use the same
# architecture and thus CC_verstage is the same as CC_romstage. If this is not
# true, VB2_LIB needs to ensure that correct CC is being used.
ifeq ($(CONFIG_VBOOT_HAS_REC_HASH_SPACE),y)
romstage-srcs += $(VB2_LIB)
endif

cbfs-files-$(CONFIG_SEPARATE_VERSTAGE) += $(CONFIG_CBFS_PREFIX)/verstage
$(CONFIG_CBFS_PREFIX)/verstage-file := $(objcbfs)/verstage.elf
$(CONFIG_CBFS_PREFIX)/verstage-type := stage
$(CONFIG_CBFS_PREFIX)/verstage-compression := $(CBFS_PRERAM_COMPRESS_FLAG)

ifeq ($(CONFIG_ARCH_VERSTAGE_X86_32)$(CONFIG_ARCH_VERSTAGE_X86_64),y)
$(CONFIG_CBFS_PREFIX)/verstage-options := -a 64 -S ".car.data"

# If CAR does not support execution of code, verstage on x86 is expected to be
# xip.
ifneq ($(CONFIG_NO_XIP_EARLY_STAGES),y)
$(CONFIG_CBFS_PREFIX)/verstage-options += --xip
endif

endif

else
ifeq ($(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK),y)
bootblock-srcs += $(objgenerated)/libverstage.a
else
romstage-srcs += $(objgenerated)/libverstage.a
endif
endif # CONFIG_SEPARATE_VERSTAGE

# Define a list of files that need to be in RO only.
# All other files will be installed into RO and RW regions
# Use $(sort) to cut down on extra spaces that would be translated to commas
regions-for-file = $(subst $(spc),$(comma),$(sort \
	$(if $(filter \
		$(if $(filter y,$(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK)),, \
			%/romstage) \
		mts \
		%/verstage \
		locales \
		locale_%.bin \
		font.bin \
		vbgfx.bin \
		rmu.bin \
		,$(1)),COREBOOT,COREBOOT FW_MAIN_A FW_MAIN_B)))

endif # CONFIG_VBOOT
