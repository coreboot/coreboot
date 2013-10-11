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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#


# SB800 Platform Files

romstage-y += cfg.c
romstage-y += early.c
romstage-y += smbus.c
romstage-y += reset.c

ramstage-y += cfg.c
ramstage-y += late.c
ramstage-y += reset.c

ramstage-$(CONFIG_SB800_MANUAL_FAN_CONTROL) += fan.c
ramstage-$(CONFIG_SB800_IMC_FAN_CONTROL) += fan.c
ramstage-$(CONFIG_HAVE_ACPI_RESUME) += spi.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c

romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += ../../sb800/enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += ../../sb800/enable_usbdebug.c

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

ifeq ($(CONFIG_SB800_IMC_FWM), y)

# ROMSIG At ROMBASE + 0x20000:
# +-----------+---------------+----------------+------------+
# |0x55AA55AA |EC ROM Address |GEC ROM Address |            |
# +-----------+---------------+----------------+------------+
# EC ROM should be 64K aligned.
SB800_FWM_POSITION=$(shell printf %u $(CONFIG_SB800_FWM_POSITION))
#assume the cbfs header is less than 128 bytes.
ROMSIG_SIZE=16

SB800_IMC_POSITION_UNALIGN=$(shell echo $(SB800_FWM_POSITION) $(ROMSIG_SIZE) \
	128 65535 | awk '{printf("%.0f", $$1 + $$2 + $$3 + $$4)}')
SB800_IMC_POSITION=$(shell echo $(SB800_IMC_POSITION_UNALIGN) \
	| awk '{printf("%.0f", $$1 - $$1 % 65536)}')

$(obj)/coreboot_SB800_romsig.bin: \
			$(call strip_quotes, $(CONFIG_SB800_IMC_FWM_FILE)) \
			$(obj)/config.h \
			$(obj)/mainboard/$(MAINBOARDDIR)/static.c
	echo "    SB800 FW  $@"
	for fwm in 1437226410 \
		$(SB800_IMC_POSITION) \
		0 \
		0 ; do \
		echo  $$fwm | LC_ALL=C awk '{printf ("%c%c%c%c", $$1 % 256, int($$1/256) % 256, int($$1/65536) % 256, int($$1/16777216));}'; \
	done > $@

cbfs-files-y += SB800/fwm
SB800/fwm-file := $(obj)/coreboot_SB800_romsig.bin
SB800/fwm-position := $(SB800_FWM_POSITION)
SB800/fwm-type := raw

cbfs-files-y += SB800/imc
SB800/imc-file := $(call strip_quotes, $(CONFIG_SB800_IMC_FWM_FILE))
SB800/imc-position := $(SB800_IMC_POSITION)
SB800/imc-type := raw

endif
