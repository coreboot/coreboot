ramstage-y += sb700.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += smbus.c
ramstage-y += sm.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += hda.c
ramstage-y += pci.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c
romstage-y += reset.c
ramstage-y += reset.c
romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c

romstage-y += early_setup.c
romstage-y += smbus.c
