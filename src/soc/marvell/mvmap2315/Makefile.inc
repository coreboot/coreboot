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

endif
