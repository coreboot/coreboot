driver-y += sis761.c
driver-y += sis966.c
driver-y += lpc.c
driver-y += ide.c
driver-y += usb.c
driver-y += usb2.c
driver-y += nic.c
driver-y += sata.c
driver-y += pcie.c
driver-y += aza.c
ramstage-y += reset.c
romstage-y += enable_usbdebug.c

chipset_bootblock_inc += $(src)/southbridge/sis/sis966/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/sis/sis966/romstrap.lds
