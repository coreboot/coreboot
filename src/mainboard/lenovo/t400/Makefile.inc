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

bootblock-y += bootblock.c
bootblock-y += dock.c

romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-y += dock.c
ramstage-y += cstates.c
ramstage-y += blc.c

ifeq ($(CONFIG_MAINBOARD_USE_LIBGFXINIT),y)
ramstage-$(CONFIG_BOARD_LENOVO_T400) += variants/t400/malibu-3/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_R400) += variants/t400/malibu-3/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_T500) += variants/t400/coronado-5/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_W500) += variants/t400/coronado-5/gma-mainboard.ads
ramstage-$(CONFIG_BOARD_LENOVO_R500) += variants/r500/gma-mainboard.ads
endif
