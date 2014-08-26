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
bootblock-y += timer.c
bootblock-y += monotonic_timer.c
bootblock-y += media.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += uart.c
endif
bootblock-y += clock.c
bootblock-y += spi.c

romstage-y += cbmem.c
romstage-y += timer.c
romstage-y += monotonic_timer.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += i2c.c
romstage-y += clock.c
romstage-y += gpio.c
romstage-y += spi.c
romstage-y += media.c


ramstage-y += cbmem.c
ramstage-y += timer.c
ramstage-y += monotonic_timer.c
ramstage-y += i2c.c
ramstage-y += clock.c
ramstage-y += spi.c
ramstage-y += gpio.c
ramstage-y += media.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c

$(objcbfs)/bootblock.raw.elf: $(objcbfs)/bootblock.elf
	cp $< $@

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	@mkdir -p $(dir $@)
	@$(IDBTOOL) --from=$< --to=$@ --enable-align
