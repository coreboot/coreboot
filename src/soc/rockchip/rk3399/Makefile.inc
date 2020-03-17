##
## This file is part of the coreboot project.
##
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

ifeq ($(CONFIG_SOC_ROCKCHIP_RK3399),y)

IDBTOOL = util/rockchip/make_idb.py

decompressor-y += decompressor.c
decompressor-y += timer.c

bootblock-y += ../common/i2c.c
bootblock-y += ../common/spi.c
bootblock-y += ../common/uart.c
bootblock-y += ../common/gpio.c
bootblock-y += ../common/pwm.c
bootblock-y += bootblock.c
bootblock-y += clock.c
bootblock-y += gpio.c
bootblock-y += saradc.c
bootblock-y += timer.c

verstage-y += ../common/gpio.c
verstage-y += gpio.c
verstage-y += sdram.c
verstage-y += ../common/i2c.c
verstage-y += ../common/spi.c
verstage-y += ../common/uart.c
verstage-y += clock.c
verstage-y += timer.c

################################################################################

romstage-y += ../common/cbmem.c
romstage-y += sdram.c
romstage-y += ../common/spi.c
romstage-y += ../common/uart.c
romstage-y += clock.c
romstage-y += ../common/pwm.c
romstage-y += timer.c
romstage-y += tsadc.c
romstage-y += usb.c
romstage-y += gpio.c
romstage-y += saradc.c
romstage-y += ../common/gpio.c
romstage-y += ../common/i2c.c

################################################################################

ramstage-y += sdram.c
ramstage-y += ../common/spi.c
ramstage-y += ../common/uart.c
ramstage-y += clock.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += display.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += ../common/edp.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += mipi.c
ramstage-y += ../common/gpio.c
ramstage-y += gpio.c
ramstage-y += ../common/i2c.c
ramstage-y += saradc.c
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += ../common/vop.c
ramstage-y += usb.c

BL31_MAKEARGS += PLAT=rk3399 M0_CROSS_COMPILE="$(CROSS_COMPILE_arm)"
################################################################################

CPPFLAGS_common += -Isrc/soc/rockchip/rk3399/include
CPPFLAGS_common += -Isrc/soc/rockchip/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	@mkdir -p $(dir $@)
	@$(IDBTOOL) --from=$< --to=$@ --enable-align --chip=RK33

endif
