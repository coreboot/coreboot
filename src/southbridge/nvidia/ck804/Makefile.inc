driver-y += ck804.c
driver-y += usb.c
driver-y += lpc.c
driver-y += smbus.c
driver-y += ide.c
driver-y += sata.c
driver-y += usb2.c
driver-y += ac97.c
driver-y += nic.c
driver-y += pci.c
driver-y += pcie.c
driver-y += ht.c

ramstage-y += reset.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c

romstage-y += enable_usbdebug.c
romstage-y += early_smbus.c

chipset_bootblock_inc += $(src)/southbridge/nvidia/ck804/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/nvidia/ck804/romstrap.lds

