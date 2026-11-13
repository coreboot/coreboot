## SPDX-License-Identifier: GPL-2.0-only

smm-y += dock.c
bootblock-y += dock.c
romstage-y += dock.c
ramstage-y += dock.c
ramstage-y += mainboard.c
ramstage-y += cstates.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
bootblock-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
bootblock-y += early_init.c
romstage-y += early_init.c
all-$(CONFIG_DRIVERS_OPTION_CFR) += cfr.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
