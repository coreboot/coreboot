ramstage-y += sis761.c
ramstage-y += sis966.c
ramstage-y += lpc.c
ramstage-y += ide.c
ramstage-y += usb.c
ramstage-y += usb2.c
ramstage-y += nic.c
ramstage-y += sata.c
ramstage-y += pcie.c
ramstage-y += aza.c
ramstage-y += reset.c

romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += enable_usbdebug.c
ramstage-$(CONFIG_USBDEBUG) += enable_usbdebug.c

chipset_bootblock_inc += $(src)/southbridge/sis/sis966/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/sis/sis966/romstrap.lds
