romstage-y += variants/$(VARIANT_DIR)/gpio.c
romstage-y += variants/$(VARIANT_DIR)/romstage.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

subdirs-y += variants/$(VARIANT_DIR)
