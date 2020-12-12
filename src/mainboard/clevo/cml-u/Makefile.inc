CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/include

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/gpio.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c

subdirs-y += variants/$(VARIANT_DIR)
