##
## This file is part of the coreboot project.
##
## Copyright (C) 2010 coresystems GmbH
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

ifeq ($(CONFIG_SOUTHBRIDGE_INTEL_SCH),y)

ramstage-y += south.c
ramstage-y += audio.c
ramstage-y += lpc.c
ramstage-y += ide.c
ramstage-y += pcie.c
ramstage-y += usb.c
ramstage-y += usb_ehci.c
ramstage-y += usb_client.c
ramstage-y += mmc.c
ramstage-y += smbus.c

ramstage-y += reset.c

ramstage-$(CONFIG_HAVE_SMI_HANDLER) += smi.c
ramstage-$(CONFIG_HAVE_SMI_HANDLER) += ../../../cpu/x86/smm/smmrelocate.S
smm-$(CONFIG_HAVE_SMI_HANDLER) += smihandler.c

ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c

# We don't ship that, but booting without it is bound to fail
cbfs-files-$(CONFIG_HAVE_CMC) += cmc.bin
cmc.bin-file := $(call strip_quotes,$(CONFIG_CMC_FILE))
cmc.bin-type := raw
cmc.bin-position := 0xfffd0000

endif
