#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 Advanced Micro Devices, Inc.
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
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#


# SB800 Platform Files

romstage-y += cfg.c
romstage-y += early.c
romstage-y += smbus.c

ramstage-y += cfg.c
ramstage-y += late.c

ramstage-$(CONFIG_HAVE_ACPI_RESUME) += spi.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c

ramstage-y   += smbus.c
ramstage-y   += lpc.c

ifeq ($(CONFIG_SB800_SATA_AHCI), y)
ifdef CONFIG_SB800_AHCI_ROM
	stripped_ahci_rom_id = $(call strip_quotes,$(CONFIG_AHCI_ROM_ID))
	cbfs-files-y += pci$(stripped_ahci_rom_id).rom
	pci$(stripped_ahci_rom_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_ROM_FILE))
	pci$(stripped_ahci_rom_id).rom-type := optionrom
endif
endif

ifeq ($(CONFIG_SB800_SATA_RAID), y)
	stripped_raid_rom_id = $(call strip_quotes,$(CONFIG_RAID_ROM_ID))
	cbfs-files-y += pci$(stripped_raid_rom_id).rom
	pci$(stripped_raid_rom_id).rom-file := $(call strip_quotes,$(CONFIG_RAID_ROM_FILE))
	pci$(stripped_raid_rom_id).rom-type := optionrom

	cbfs-files-y += raid/misc.bin
	raid/misc.bin-file := $(call strip_quotes,$(CONFIG_RAID_MISC_ROM_FILE))
	raid/misc.bin-position := $(CONFIG_RAID_MISC_ROM_POSITION)
	raid/misc.bin-type := raw
endif
