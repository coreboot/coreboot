driver-y += mcp55.c
driver-y += azalia.c
driver-y += ht.c
driver-y += ide.c
driver-y += lpc.c
driver-y += nic.c
driver-y += pci.c
driver-y += pcie.c
driver-y += sata.c
driver-y += smbus.c
driver-y += usb2.c
driver-y += usb.c

driver-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c

ramstage-y += reset.c

romstage-y += enable_usbdebug.c

chipset_bootblock_inc += $(src)/southbridge/nvidia/mcp55/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/nvidia/mcp55/romstrap.lds
