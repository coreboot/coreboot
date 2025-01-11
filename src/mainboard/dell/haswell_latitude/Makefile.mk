## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += bootblock.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y += variants/$(VARIANT_DIR)/romstage.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
