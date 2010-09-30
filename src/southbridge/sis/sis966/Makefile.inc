driver-y += sis761.c
driver-y += sis966.c
driver-y += sis966_lpc.c
driver-y += sis966_ide.c
driver-y += sis966_usb.c
driver-y += sis966_usb2.c
driver-y += sis966_nic.c
driver-y += sis966_sata.c
driver-y += sis966_pcie.c
driver-y += sis966_aza.c
ramstage-y += sis966_reset.c

chipset_bootblock_inc += $(src)/southbridge/sis/sis966/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/sis/sis966/romstrap.lds
