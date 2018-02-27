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

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += gnvs.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += acpi.c
ramstage-$(CONFIG_CHROMEOS_RAMOOPS) += ramoops.c
romstage-y += vpd_decode.c
ramstage-y += vpd_decode.c cros_vpd.c vpd_mac.c vpd_serialno.c vpd_calibration.c
ramstage-$(CONFIG_CHROMEOS_DISABLE_PLATFORM_HIERARCHY_ON_RESUME) += tpm2.c
ramstage-$(CONFIG_HAVE_REGULATORY_DOMAIN) += wrdd.c
ramstage-$(CONFIG_USE_SAR) += sar.c
ramstage-$(CONFIG_TPM_CR50) += cr50_enable_update.c
ifeq ($(CONFIG_ARCH_MIPS),)
bootblock-y += watchdog.c
ramstage-y += watchdog.c
endif
