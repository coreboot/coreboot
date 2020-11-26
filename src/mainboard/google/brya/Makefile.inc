bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += mainboard.c

VARIANT_DIR:=$(call strip_quotes,$(CONFIG_VARIANT_DIR))

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
