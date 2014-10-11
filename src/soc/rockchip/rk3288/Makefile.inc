##
## This file is part of the coreboot project.
##
## Copyright 2014 Rockchip Inc.
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

IDBTOOL = util/rockchip/make_idb.py

#bootblock-y += bootblock.c
bootblock-y += cbmem.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += uart.c
endif
bootblock-y += timer.c
bootblock-y += monotonic_timer.c
bootblock-y += clock.c
bootblock-y += spi.c
bootblock-y += media.c
bootblock-y += gpio.c
bootblock-y += i2c.c
bootblock-y += rk808.c

verstage-y += monotonic_timer.c
verstage-y += spi.c
verstage-y += timer.c
verstage-$(CONFIG_CONSOLE_SERIAL_UART) += uart.c
verstage-y += gpio.c
verstage-y += clock.c
verstage-y += i2c.c
verstage-y += media.c

romstage-y += cbmem.c
romstage-y += timer.c
romstage-y += monotonic_timer.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += i2c.c
romstage-y += clock.c
romstage-y += gpio.c
romstage-y += spi.c
romstage-y += media.c
romstage-y += sdram.c
romstage-y += pwm.c

ramstage-y += soc.c
ramstage-y += cbmem.c
ramstage-y += timer.c
ramstage-y += monotonic_timer.c
ramstage-y += i2c.c
ramstage-y += clock.c
ramstage-y += spi.c
ramstage-y += gpio.c
ramstage-y += media.c
ramstage-y += rk808.c
ramstage-y += pwm.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c

$(objcbfs)/bootblock.raw.elf: $(objcbfs)/bootblock.elf
	cp $< $@

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	@mkdir -p $(dir $@)
	@$(IDBTOOL) --from=$< --to=$@ --enable-align
