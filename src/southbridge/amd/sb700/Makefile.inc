ifeq ($(CONFIG_SOUTHBRIDGE_AMD_SB700),y)

ramstage-y += sb700.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += smbus.c
ramstage-y += sm.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += hda.c
ramstage-y += pci.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
romstage-y += reset.c
ramstage-y += reset.c
ramstage-y += spi.c

bootblock-y += enable_usbdebug.c
romstage-y += enable_usbdebug.c
ramstage-y += enable_usbdebug.c

romstage-y += early_setup.c
romstage-y += smbus.c

romstage-y += ramtop.c
ramstage-y += ramtop.c

endif
