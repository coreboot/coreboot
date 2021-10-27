CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

bootblock-y += bootblock.c
bootblock-y += variants/$(VARIANT_DIR)/gpio_early.c

romstage-y += romstage.c

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
