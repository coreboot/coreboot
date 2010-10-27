driver-y += ck804.c
driver-y += ck804_usb.c
driver-y += ck804_lpc.c
driver-y += ck804_smbus.c
driver-y += ck804_ide.c
driver-y += ck804_sata.c
driver-y += ck804_usb2.c
driver-y += ck804_ac97.c
driver-y += ck804_nic.c
driver-y += ck804_pci.c
driver-y += ck804_pcie.c
driver-y += ck804_ht.c

ramstage-y += ck804_reset.c

ramstage-$(CONFIG_GENERATE_ACPI_TABLES) += ck804_fadt.c

romstage-y += ck804_enable_usbdebug.c
romstage-y += ck804_early_smbus.c

chipset_bootblock_inc += $(src)/southbridge/nvidia/ck804/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/nvidia/ck804/romstrap.lds

