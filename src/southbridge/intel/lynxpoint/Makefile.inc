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

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_LYNXPOINT),y)

ramstage-y += pch.c
ramstage-y += azalia.c
ramstage-y += lpc.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += usb_ehci.c
ramstage-y += usb_xhci.c
ramstage-y += me_9.x.c
ramstage-y += smbus.c
ramstage-y += hda_verb.c
ramstage-$(CONFIG_INTEL_LYNXPOINT_LP) += serialio.c

ifneq ($(CONFIG_VARIANT_DIR),)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/hda_verb.c
else
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c
endif

ramstage-y += rcba.c
ramstage-y += me_status.c
ramstage-y += reset.c
ramstage-y += watchdog.c
ramstage-y += acpi.c

ramstage-$(CONFIG_ELOG) += elog.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c pmutil.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c me_9.x.c finalize.c pch.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += pmutil.c usb_ehci.c usb_xhci.c

romstage-y += early_usb.c early_smbus.c early_me.c me_status.c early_pch.c
romstage-y += reset.c early_spi.c rcba.c pmutil.c

ifeq ($(CONFIG_INTEL_LYNXPOINT_LP),y)
romstage-y += lp_gpio.c
ramstage-y += lp_gpio.c
smm-$(CONFIG_HAVE_SMI_HANDLER) += lp_gpio.c
endif

postcar-y += reset.c

endif
