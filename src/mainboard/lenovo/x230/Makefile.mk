## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c

ifeq ($(CONFIG_BOARD_LENOVO_X230_EDP),y)
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/x230_edp/gma-mainboard.ads
else
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads
endif
