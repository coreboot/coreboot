## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += early_init.c
bootblock-y += variants/$(VARIANT_DIR)/gpio.c

romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
