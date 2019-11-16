ifeq ($(CONFIG_SOUTHBRIDGE_NVIDIA_CK804),y)

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

ramstage-$(CONFIG_HAVE_ACPI_TABLES) += fadt.c

bootblock-y += enable_usbdebug.c
romstage-y += enable_usbdebug.c
ramstage-y += enable_usbdebug.c
romstage-y += early_smbus.c

bootblock-y += romstrap.ld
bootblock-y += romstrap.S

endif
