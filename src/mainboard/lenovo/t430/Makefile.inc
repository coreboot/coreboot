romstage-y += romstage.c
romstage-y += gpio.c
ramstage-y += acpi_tables.c
smm-y += smihandler.c

ramstage-$(CONFIG_MAINBOARD_USE_LIBGFXINIT) += gma-mainboard.ads
