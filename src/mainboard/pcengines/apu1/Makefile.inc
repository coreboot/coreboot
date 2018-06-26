#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 Advanced Micro Devices, Inc.
# Copyright (C) 2013 Sage Electronic Engineering, LLC
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

ifeq ($(CONFIG_AHCI_BIOS),y)
stripped_ahcibios_id = $(call strip_quotes,$(CONFIG_AHCI_BIOS_ID))
cbfs-files-$(CONFIG_AHCI_BIOS) += pci$(stripped_ahcibios_id).rom
pci$(stripped_ahcibios_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_BIOS_FILE))
pci$(stripped_ahcibios_id).rom-type := optionrom
endif

romstage-y += buildOpts.c
romstage-y += BiosCallOuts.c
romstage-y += OemCustomize.c
romstage-y += gpio_ftns.c

ramstage-y += buildOpts.c
ramstage-y += BiosCallOuts.c
ramstage-y += OemCustomize.c
ramstage-y += gpio_ftns.c


# Order of names in SPD_SOURCES is important!
SPD_SOURCES = HYNIX-H5TQ2G83CFR
SPD_SOURCES += HYNIX-H5TQ4G83MFR
