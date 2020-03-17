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

ramstage-$(CONFIG_BOARD_LENOVO_X200) += variants/$(VARIANT_DIR)/dock.c
ramstage-y += cstates.c
ramstage-y += blc.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

$(call add_vbt_to_cbfs, data_ccfl.vbt, data_ccfl.vbt)
$(call add_vbt_to_cbfs, data_led.vbt, data_led.vbt)
