## SPDX-License-Identifier: GPL-2.0-only

bootblock-y += variants/$(VARIANT_DIR)/early_init.c
romstage-y  += variants/$(VARIANT_DIR)/early_init.c
bootblock-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y  += variants/$(VARIANT_DIR)/gpio.c
ramstage-y  += variants/$(VARIANT_DIR)/hda_verb.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

# FIXME: Other variants with same size onboard RAM may exist.
SPD_SOURCES = hynix_4g

subdirs-y += variants/$(VARIANT_DIR)
