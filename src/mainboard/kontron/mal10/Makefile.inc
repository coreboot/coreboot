bootblock-y += bootblock.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

subdirs-y += variants/$(VARIANT_DIR)
subdirs-y += carriers/$(CARRIER_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include
