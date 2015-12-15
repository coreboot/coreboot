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

bootblock-y += chromeos.c
verstage-y += chromeos.c
romstage-y += chromeos.c
ramstage-y += chromeos.c

bootblock-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
verstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
romstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c
ramstage-$(CONFIG_CHROMEOS_VBNV_CMOS) += vbnv_cmos.c

bootblock-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
verstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
romstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c
ramstage-$(CONFIG_CHROMEOS_VBNV_EC) += vbnv_ec.c

bootblock-$(CONFIG_CHROMEOS_VBNV_FLASH) += vbnv_flash.c
verstage-$(CONFIG_CHROMEOS_VBNV_FLASH) += vbnv_flash.c
romstage-$(CONFIG_CHROMEOS_VBNV_FLASH) += vbnv_flash.c
ramstage-$(CONFIG_CHROMEOS_VBNV_FLASH) += vbnv_flash.c

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += gnvs.c
ramstage-$(CONFIG_CHROMEOS_RAMOOPS) += ramoops.c
romstage-y += vpd_decode.c
ramstage-y += vpd_decode.c cros_vpd.c vpd_mac.c vpd_serialno.c vpd_calibration.c
ifeq ($(CONFIG_ARCH_X86)$(CONFIG_ARCH_MIPS),)
bootblock-y += watchdog.c
ramstage-y += watchdog.c
endif

ifeq ($(MOCK_TPM),1)
CFLAGS_common += -DMOCK_TPM=1
else
CFLAGS_common += -DMOCK_TPM=0
endif

subdirs-$(CONFIG_VBOOT_VERIFY_FIRMWARE) += vboot2
