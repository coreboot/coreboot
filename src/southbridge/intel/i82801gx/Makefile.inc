##
## This file is part of the coreboot project.
##
## Copyright (C) 2008-2009 coresystems GmbH
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

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_I82801GX),y)

bootblock-y += early_init.c
bootblock-y += bootblock.c

ramstage-y += i82801gx.c
ramstage-y += ac97.c
ramstage-y += azalia.c
ramstage-y += ide.c
ramstage-y += lpc.c
ramstage-y += nic.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += smbus.c
ramstage-y += usb.c
ramstage-y += usb_ehci.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c

smm-y += smihandler.c

romstage-y += early_init.c
romstage-y += early_smbus.c
romstage-y += early_cir.c

endif
