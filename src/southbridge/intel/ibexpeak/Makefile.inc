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

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_IBEXPEAK),y)

ramstage-y += ../bd82x6x/pch.c
ramstage-y += azalia.c
ramstage-y += lpc.c
ramstage-y += ../bd82x6x/pci.c
ramstage-y += ../bd82x6x/pcie.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += me.c
ramstage-y += ../bd82x6x/me_8.x.c
ramstage-y += smbus.c
ramstage-y += thermal.c
ramstage-y += ../common/pciehp.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c

ramstage-y += ../bd82x6x/me_status.c

ramstage-$(CONFIG_ELOG) += ../bd82x6x/elog.c
ramstage-y += madt.c

smm-y += smihandler.c me.c ../bd82x6x/me_8.x.c ../bd82x6x/pch.c

romstage-y += ../bd82x6x/early_usb.c early_smbus.c ../bd82x6x/early_me.c ../bd82x6x/me_status.c ../common/gpio.c early_thermal.c
romstage-y += ../bd82x6x/early_rcba.c

endif
