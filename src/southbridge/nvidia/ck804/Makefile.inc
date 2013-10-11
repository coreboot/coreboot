ramstage-y += ck804.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += smbus.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += usb2.c
ramstage-y += ac97.c
ramstage-y += nic.c
ramstage-y += pci.c
ramstage-y += pcie.c
ramstage-y += ht.c

ramstage-y += reset.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += fadt.c

romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c
romstage-y += early_smbus.c

chipset_bootblock_inc += $(src)/southbridge/nvidia/ck804/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/nvidia/ck804/romstrap.lds

