ifeq ($(CONFIG_SOUTHBRIDGE_BROADCOM_BCM5785),y)

ramstage-y += bcm5785.c
ramstage-y += usb.c
ramstage-y += lpc.c
ramstage-y += sb_pci_main.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += reset.c

endif
