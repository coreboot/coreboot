##
## This file is part of the coreboot project.
##
## Copyright 2015 Google Inc.
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
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
##

bootblock-y += bootblock.c
bootblock-y += cbmem.c
bootblock-y += i2c.c
bootblock-y += timer.c
bootblock-$(CONFIG_SPI_FLASH) += spi.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += ns16550.c
endif

verstage-y += i2c.c
verstage-y += timer.c
verstage-$(CONFIG_SPI_FLASH) += spi.c
verstage-$(CONFIG_DRIVERS_UART) += ns16550.c

romstage-y += cbmem.c
romstage-y += i2c.c
romstage-y += timer.c
romstage-y += romstage.c
romstage-y += sdram.c
romstage-$(CONFIG_SPI_FLASH) += spi.c
romstage-$(CONFIG_DRIVERS_UART) += ns16550.c
romstage-y += ddr_init.c
romstage-y += ddr_init_table.c
romstage-y += shmoo_and28.c
romstage-y += phy_reg_access.c
romstage-y += ydc_ddr_bist.c
romstage-y += timer.c

ramstage-y += cbmem.c
ramstage-y += i2c.c
ramstage-y += timer.c
ramstage-$(CONFIG_SPI_FLASH) += spi.c
ramstage-$(CONFIG_DRIVERS_UART) += ns16550.c

CPPFLAGS_common += -Isrc/soc/broadcom/cygnus/include/

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.elf
	@printf "    OBJCOPY    $(subst $(obj)/,,$(@))\n"
	$(OBJCOPY_bootblock) -O binary $< $@
