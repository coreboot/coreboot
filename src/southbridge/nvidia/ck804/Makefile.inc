driver-y += ck804.o
driver-y += ck804_usb.o
driver-y += ck804_lpc.o
driver-y += ck804_smbus.o
driver-y += ck804_ide.o
driver-y += ck804_sata.o
driver-y += ck804_usb2.o
driver-y += ck804_ac97.o
driver-y += ck804_nic.o
driver-y += ck804_pci.o
driver-y += ck804_pcie.o
driver-y += ck804_ht.o

obj-y += ck804_reset.o

obj-$(CONFIG_HAVE_ACPI_TABLES) += ck804_fadt.o
