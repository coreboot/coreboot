##
## This file is part of the coreboot project.
##
## Copyright 2016 Rockchip Inc.
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

bootblock-y += ../common/spi.c
ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += ../common/uart.c
endif
bootblock-y += bootblock.c
bootblock-y += clock.c
bootblock-y += timer.c

verstage-y += ../common/cbmem.c
verstage-y += ../common/spi.c
verstage-$(CONFIG_DRIVERS_UART) += ../common/uart.c
verstage-y += clock.c
verstage-y += timer.c

################################################################################

romstage-y += ../common/cbmem.c
romstage-y += ../common/spi.c
romstage-$(CONFIG_DRIVERS_UART) += ../common/uart.c
romstage-y += clock.c
romstage-y += timer.c

################################################################################

ramstage-y += ../common/cbmem.c
ramstage-y += ../common/spi.c
ramstage-$(CONFIG_DRIVERS_UART) += ../common/uart.c
ramstage-y += clock.c
ramstage-y += soc.c
ramstage-y += timer.c

################################################################################

CPPFLAGS_common += -Isrc/soc/rockchip/rk3399/include
CPPFLAGS_common += -Isrc/soc/rockchip/common/include

$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	@printf "Generating: $(subst $(obj)/,,$(@))\n"
	@mkdir -p $(dir $@)
	@$(IDBTOOL) --from=$< --to=$@ --enable-align --chip=RK33

endif
