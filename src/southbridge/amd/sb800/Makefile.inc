ifeq ($(CONFIG_SOUTHBRIDGE_AMD_SB800),y)

ramstage-y += sb800.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += sm.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += hda.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c
ramstage-y += reset.c

bootblock-y += enable_usbdebug.c
romstage-y += enable_usbdebug.c
ramstage-y += enable_usbdebug.c

romstage-y += ramtop.c
ramstage-y += ramtop.c

endif
