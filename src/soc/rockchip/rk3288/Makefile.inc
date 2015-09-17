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
## Foundation, Inc.
##

ifeq ($(CONFIG_SOC_ROCKCHIP_RK3288),y)

IDBTOOL = util/rockchip/make_idb.py

bootblock-y += bootblock.c
bootblock-y += cbmem.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += uart.c
endif
bootblock-y += timer.c
bootblock-y += clock.c
bootblock-y += spi.c
bootblock-y += gpio.c
bootblock-y += i2c.c
bootblock-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
bootblock-y += rk808.c

verstage-y += spi.c
verstage-y += timer.c
verstage-$(CONFIG_DRIVERS_UART) += uart.c
verstage-y += gpio.c
verstage-y += clock.c
libverstage-y += crypto.c
verstage-y += i2c.c
verstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c

romstage-y += cbmem.c
romstage-y += timer.c
romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += i2c.c
romstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
romstage-y += clock.c
romstage-y += gpio.c
romstage-y += spi.c
romstage-y += sdram.c
romstage-y += rk808.c
romstage-y += pwm.c
romstage-y += tsadc.c

ramstage-y += soc.c
ramstage-y += cbmem.c
ramstage-y += timer.c
ramstage-y += i2c.c
ramstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
ramstage-y += clock.c
ramstage-y += spi.c
ramstage-y += sdram.c
ramstage-y += gpio.c
ramstage-y += rk808.c
ramstage-y += pwm.c
ramstage-y += vop.c
ramstage-y += edp.c
ramstage-y += hdmi.c
ramstage-y += display.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c

CPPFLAGS_common += -Isrc/soc/rockchip/rk3288/include/

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	@mkdir -p $(dir $@)
	@$(IDBTOOL) --from=$< --to=$@ --enable-align

endif
