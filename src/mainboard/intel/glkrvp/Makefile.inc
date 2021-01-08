bootblock-y += bootblock.c
bootblock-y += ec.c

romstage-$(CONFIG_CHROMEOS) += chromeos.c
romstage-y += boardid.c

ramstage-y += boardid.c
ramstage-$(CONFIG_CHROMEOS) += chromeos.c
ramstage-y += ec.c
ramstage-y += mainboard.c

verstage-$(CONFIG_CHROMEOS) += chromeos.c

subdirs-y += variants/baseboard
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/baseboard/include

VARIANT_DIR:=$(call strip_quotes,$(CONFIG_VARIANT_DIR))
subdirs-y += variants/$(VARIANT_DIR)
CPPFLAGS_common += -I$(src)/mainboard/$(MAINBOARDDIR)/variants/$(VARIANT_DIR)/include
