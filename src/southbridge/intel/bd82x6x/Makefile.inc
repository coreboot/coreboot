##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 Google Inc.
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
## Foundation, Inc.
##

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_C216)$(CONFIG_SOUTHBRIDGE_INTEL_BD82X6X),y)

ramstage-y += pch.c
ramstage-y += azalia.c
ramstage-y += lpc.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += usb_xhci.c
ramstage-y += me.c
ramstage-y += me_8.x.c
ramstage-y += smbus.c
ramstage-y += ../common/pciehp.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c

ramstage-y += me_status.c
ramstage-y += reset.c
ramstage-y += watchdog.c

ramstage-$(CONFIG_ELOG) += elog.c
ramstage-y += ../common/spi.c
smm-$(CONFIG_SPI_FLASH_SMM) += ../common/spi.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c me.c me_8.x.c finalize.c pch.c

romstage-y += early_smbus.c me_status.c gpio.c
romstage-y += reset.c
romstage-y += early_spi.c early_pch_common.c
romstage-y += early_rcba.c

romstage-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE_MRC) += early_me_mrc.c early_usb_mrc.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE_MRC) += early_me_mrc.c early_usb_mrc.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE) += early_thermal.c early_pch.c early_me.c early_usb.c
romstage-$(CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE) += early_thermal.c early_pch.c early_me.c early_usb.c

ramstage-y += madt.c

endif
