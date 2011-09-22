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

stripped_ahci_rom_id = $(call strip_quotes,$(CONFIG_AHCI_ROM_ID))
cbfs-files-$(CONFIG_SB800_SATA_AHCI) += pci$(stripped_ahci_rom_id).rom
pci$(stripped_ahci_rom_id).rom-file := $(call strip_quotes,$(CONFIG_AHCI_ROM_FILE))
pci$(stripped_ahci_rom_id).rom-type := optionrom

stripped_raid_rom_id = $(call strip_quotes,$(CONFIG_RAID_ROM_ID))
cbfs-files-$(CONFIG_SB800_SATA_RAID) += pci$(stripped_raid_rom_id).rom
pci$(stripped_raid_rom_id).rom-file := $(call strip_quotes,$(CONFIG_RAID_ROM_FILE))
pci$(stripped_raid_rom_id).rom-type := optionrom

cbfs-files-$(CONFIG_RAID_MISC_ROM) += raid/misc.bin
raid/misc.bin-file := $(call strip_quotes,$(CONFIG_RAID_MISC_ROM_FILE))
raid/misc.bin-type := raw

