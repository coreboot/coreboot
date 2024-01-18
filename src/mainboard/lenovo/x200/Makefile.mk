## SPDX-License-Identifier: GPL-2.0-only

ramstage-$(CONFIG_BOARD_LENOVO_X200) += variants/$(VARIANT_DIR)/dock.c
ramstage-y += cstates.c
ramstage-y += blc.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads

$(call add_vbt_to_cbfs, data_ccfl.vbt, data_ccfl.vbt)
$(call add_vbt_to_cbfs, data_led.vbt, data_led.vbt)
