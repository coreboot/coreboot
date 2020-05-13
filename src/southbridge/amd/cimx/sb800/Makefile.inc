# SPDX-License-Identifier: GPL-2.0-only

# SB800 Platform Files

bootblock-y += bootblock.c

romstage-y += cfg.c
romstage-y += early.c
romstage-y += smbus.c smbus_spd.c

ramstage-y += cfg.c
ramstage-y += late.c

all-y += reset.c

ramstage-$(CONFIG_SB800_MANUAL_FAN_CONTROL) += fan.c
ramstage-$(CONFIG_SB800_IMC_FAN_CONTROL) += fan.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

postcar-y += ramtop.c
romstage-y += ramtop.c
ramstage-y += ramtop.c

bootblock-$(CONFIG_USBDEBUG) += ../../sb800/enable_usbdebug.c
romstage-$(CONFIG_USBDEBUG) += ../../sb800/enable_usbdebug.c
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

SB800_IMC_POSITION=$(call int-align,$(call add-int,$(SB800_FWM_POSITION) $(ROMSIG_SIZE) 128),65536)

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
