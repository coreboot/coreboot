bootblock-y += bootblock.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += romstage.c

ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += mainboard.c
ramstage-y += ec.c

VARIANT_DIR:=$(call strip_quotes,$(CONFIG_VARIANT_DIR))

subdirs-y += variants/baseboard
subdirs-y += variants/$(VARIANT_DIR)
subdirs-y += variants/$(VARIANT_DIR)/memory
subdirs-y += spd

CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
