##
## This file is part of the coreboot project.
##
## Copyright 2017-present Facebook, Inc.
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

ifeq ($(CONFIG_SOC_CAVIUM_CN81XX),y)

# bootblock
bootblock-$(CONFIG_BOOTBLOCK_CUSTOM) += bootblock_custom.S
bootblock-y += bootblock.c
bootblock-y += twsi.c
bootblock-y += clock.c
bootblock-y += gpio.c
bootblock-y += timer.c
bootblock-y += spi.c
bootblock-y += uart.c
bootblock-y += cpu.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += uart.c
endif

################################################################################
# romstage

romstage-y += twsi.c
romstage-y += clock.c
romstage-y += gpio.c
romstage-y += timer.c
romstage-y += spi.c
romstage-y += uart.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-< += cpu.c

romstage-y += sdram.c
romstage-y += mmu.c

romstage-y += ../common/cbmem.c
# BDK coreboot interface
romstage-y += ../common/bdk-coreboot.c


################################################################################
# ramstage

ramstage-y += twsi.c
ramstage-y += clock.c
ramstage-y += gpio.c
ramstage-y += timer.c
ramstage-y += spi.c
ramstage-y += uart.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c
ramstage-y += sdram.c
ramstage-y += soc.c
ramstage-y += cpu.c
ramstage-y += cpu_secondary.S
ramstage-y += ecam0.c

# BDK coreboot interface
ramstage-y += ../common/bdk-coreboot.c


CPPFLAGS_common += -Isrc/soc/cavium/cn81xx/include

endif
