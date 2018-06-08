##
## This file is part of the coreboot project.
##
## Copyright (C) 2016 Marvell, Inc.
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

ifeq ($(CONFIG_SOC_MARVELL_MVMAP2315),y)

bootblock-y += a2bus.c
bootblock-y += apmu.c
bootblock-y += bootblock.c
bootblock-y += bdb.c
bootblock-y += clock.c
bootblock-y += digest.c
bootblock-y += fiq.S
bootblock-y += gic.c
bootblock-y += gpio.c
bootblock-y += flash.c
bootblock-y += load_validate.c
bootblock-y += mcu.c
bootblock-y += media.c
bootblock-y += nvm.c
bootblock-y += pinmux.c
bootblock-y += pmic.c
bootblock-y += reset.c
bootblock-y += timer.c
bootblock-y += sdram.c
bootblock-y += uart.c
bootblock-y += wdt.c

ramstage-y += cbmem.c
ramstage-y += media.c
ramstage-y += gpio.c
ramstage-y += ramstage_entry.S
ramstage-y += reset.c
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-y += sdram.c
ramstage-y += uart.c
ramstage-y += wdt.c

romstage-y += cbmem.c
romstage-y += clock.c
romstage-y += gpio.c
romstage-y += media.c
romstage-y += mmu_operations.c
romstage-y += reset.c
romstage-y += romstage_entry.S
romstage-y += romstage.c
romstage-y += sdram.c
romstage-y += timer.c
romstage-y += uart.c
romstage-y += wdt.c

CPPFLAGS_common += -Isrc/soc/marvell/mvmap2315/include/

#
# Common Boot Flow parameters
#
BOOTBLOCK_REGION_SIZE = 131072
BOOTBLOCK_TYPE = 1
BOOTBLOCK_PARTITION = 1
BOOTBLOCK_LOAD_ADDRESS = 0xe000c000
BDB = $(obj)/bdb.bin
BDB_LOAD_ADDRESS = 0xe0000000
DATA_VERSION = 2

#
# Keys used to create BDB
#
KEYDIR = $(VBOOT_SOURCE)/tests/testkeys
BDBKEY_PUB = $(KEYDIR)/bdbkey.keyb
BDBKEY_PRI = $(KEYDIR)/bdbkey.pem
BDBKEY_DIGEST = $(VBOOT_SOURCE)/tests/testdata/bdbkey_digest.bin
# Use bdbkeys because mvmap2315's bootROM doesn't support RSA3072b with
# exponent 3 and the algorithms for BDB key and data key have to match.
DATAKEY_PUB = $(KEYDIR)/bdbkey.keyb
DATAKEY_PRI = $(KEYDIR)/bdbkey.pem

# Calculate bootblock.raw.bin's size and offset
.PHONY: bootblock_offset
bootblock_offset: $(objcbfs)/bootblock.raw.bin
	$(eval BOOTBLOCK_SIZE = $(call file-size, $<))
	$(eval BOOTBLOCK_OFFSET = $(call int-subtract, \
		$(BOOTBLOCK_REGION_SIZE) $(BOOTBLOCK_SIZE)))

#
# Create BDB
#
# TODO: Verify sign algorithms of the BDB key and data key are supported
$(BDB): $(objcbfs)/bootblock.raw.bin $(FUTILITY) bootblock_offset \
		$(BDBKEY_PRI) $(BDBKEY_PUB) $(DATAKEY_PRI) $(DATAKEY_PUB) \
		$(VBOOT_SOURCE)/tests/testdata/bdbkey_digest.bin
	$(FUTILITY) bdb --create $@.tmp \
		--bdbkey_pri $(BDBKEY_PRI) --bdbkey_pub $(BDBKEY_PUB) \
		--datakey_pri $(DATAKEY_PRI) --datakey_pub $(DATAKEY_PUB) \
		--load_address $(BDB_LOAD_ADDRESS)
	$(FUTILITY) bdb --add $@.tmp \
		--data $< \
		--offset $(BOOTBLOCK_OFFSET) \
		--partition $(BOOTBLOCK_PARTITION) \
		--type $(BOOTBLOCK_TYPE) \
		--load_address $(BOOTBLOCK_LOAD_ADDRESS)
	$(FUTILITY) bdb --resign $@.tmp \
		--datakey_pri $(DATAKEY_PRI) \
		--data_version $(DATA_VERSION)
	$(FUTILITY) bdb --verify $@.tmp --key_digest $(BDBKEY_DIGEST)
	mv $@.tmp $@

#
# Create custom bootblock.bin
#
# This overrides generic pattern rule *.raw.bin -> *.bin for bootblock.bin.
# bootblock.bin is structured as follows:
#   +------------------+
#   |      BDB         |
#   +------------------+
#   | 000............0 |
#   +------------------+
#   |bootblock.raw.bin |
#   +------------------+
# And this will be placed in BOOTBLOCK region.
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin $(BDB) bootblock_offset
	dd if=/dev/zero bs=1 count=$(BOOTBLOCK_REGION_SIZE) of=$@.tmp
	dd if=$(BDB) bs=1 count=$(call file-size, $(BDB)) conv=notrunc of=$@.tmp
	dd if=$(objcbfs)/bootblock.raw.bin bs=1 count=$(BOOTBLOCK_SIZE) \
		seek=$(BOOTBLOCK_OFFSET) conv=notrunc of=$@.tmp
	mv $@.tmp $@

endif
