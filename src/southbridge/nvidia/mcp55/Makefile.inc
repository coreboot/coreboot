ramstage-y += mcp55.c
ramstage-y += azalia.c
ramstage-y += ht.c
ramstage-y += ide.c
ramstage-y += lpc.c
ramstage-y += nic.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += sata.c
ramstage-y += smbus.c
ramstage-y += usb2.c
ramstage-y += usb.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c

ramstage-y += reset.c

romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c

chipset_bootblock_inc += $(src)/southbridge/nvidia/mcp55/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/nvidia/mcp55/romstrap.lds
