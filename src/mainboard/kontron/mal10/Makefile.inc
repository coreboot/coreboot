bootblock-y += bootblock.c
ramstage-y += ramstage.c
ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += variants/$(VARIANT_DIR)/gma-mainboard.ads

subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

subdirs-y += carriers/$(CARRIER_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/carriers/$(CARRIER_DIR)/include
