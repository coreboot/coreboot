## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c

romstage-y += variants/$(VARIANT_DIR)/gpio.c

ramstage-y += cstates.c
ramstage-y += blc.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads
