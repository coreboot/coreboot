CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include

bootblock-y += bootblock.c

ramstage-y += ramstage.c
ramstage-y += variants/$(VARIANT_DIR)/hda_verb.c

subdirs-y += variants/$(VARIANT_DIR)
