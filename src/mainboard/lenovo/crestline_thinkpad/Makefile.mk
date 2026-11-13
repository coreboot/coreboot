## SPDX-License-Identifier: GPL-2.0-only

smm-y += variants/$(VARIANT_DIR)/dock.c
bootblock-y += variants/$(VARIANT_DIR)/dock.c
romstage-y += variants/$(VARIANT_DIR)/dock.c
ramstage-y += variants/$(VARIANT_DIR)/dock.c
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

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
