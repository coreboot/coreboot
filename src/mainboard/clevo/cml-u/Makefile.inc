CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c
subdirs-y += variants/$(VARIANT_DIR)
