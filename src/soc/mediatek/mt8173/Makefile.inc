##
## This file is part of the coreboot project.
##
## Copyright 2015 MediaTek Inc.
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

ifeq ($(CONFIG_SOC_MEDIATEK_MT8173),y)

bootblock-y += bootblock.c
bootblock-y += cbfs.c
bootblock-y += pll.c
bootblock-y += timer.c

ifeq ($(CONFIG_BOOTBLOCK_CONSOLE),y)
bootblock-$(CONFIG_DRIVERS_UART) += uart.c
endif

bootblock-y += gpio.c gpio_init.c pmic_wrap.c mt6391.c

################################################################################

romstage-y += cbfs.c
romstage-y += timer.c

romstage-$(CONFIG_DRIVERS_UART) += uart.c
romstage-y += cbmem.c
romstage-y += gpio.c

################################################################################

ramstage-y += cbmem.c
ramstage-y += cbfs.c
ramstage-y += soc.c
ramstage-y += timer.c
ramstage-$(CONFIG_DRIVERS_UART) += uart.c
ramstage-y += gpio.c

################################################################################

CPPFLAGS_common += -Isrc/soc/mediatek/mt8173/include

endif
