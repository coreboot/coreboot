driver-y += sb700.c
driver-y += usb.c
driver-y += lpc.c
driver-y += smbus.c
driver-y += sm.c
driver-y += ide.c
driver-y += sata.c
driver-y += hda.c
driver-y += pci.c
ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c
romstage-y += reset.c
ramstage-y += reset.c
romstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
smm-$(CONFIG_USBDEBUG) += enable_usbdebug.c

romstage-y += early_setup.c
romstage-y += smbus.c
