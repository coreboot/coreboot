driver-y += sis761.o
driver-y += sis966.o
driver-y += sis966_lpc.o
driver-y += sis966_ide.o
driver-y += sis966_usb.o
driver-y += sis966_usb2.o
driver-y += sis966_nic.o
driver-y += sis966_sata.o
driver-y += sis966_pcie.o
driver-y += sis966_aza.o
obj-y += sis966_reset.o

chipset_bootblock_inc += $(src)/southbridge/sis/sis966/romstrap.inc
chipset_bootblock_lds += $(src)/southbridge/sis/sis966/romstrap.lds
