##
## This file is part of the coreboot project.
##
## Copyright 2014 Google Inc.
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
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc.
##

ifeq ($(CONFIG_SOC_MARVELL_BG4CD),y)

bootblock-$(CONFIG_VBOOT_VERIFY_FIRMWARE) += bootblock_asm.S
bootblock-$(CONFIG_VBOOT_VERIFY_FIRMWARE) += bootblock.c

bootblock-y += cbmem.c
bootblock-y += i2c.c
bootblock-y += monotonic_timer.c
bootblock-$(CONFIG_SPI_FLASH) += spi.c

verstage-y += i2c.c
verstage-y += monotonic_timer.c
verstage-$(CONFIG_SPI_FLASH) += spi.c

romstage-y += romstage.S
romstage-y += cbmem.c
romstage-y += i2c.c
romstage-y += monotonic_timer.c
romstage-y += sdram.c
romstage-$(CONFIG_SPI_FLASH) += spi.c

ramstage-y += cbmem.c
ramstage-y += i2c.c
ramstage-y += monotonic_timer.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c

CPPFLAGS_common += -Isrc/soc/marvell/bg4cd/include/

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	@mkdir -p $(dir $@)
	@mv $< $@

endif
