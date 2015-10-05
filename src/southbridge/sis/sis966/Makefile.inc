ifeq ($(CONFIG_SOUTHBRIDGE_SIS_SIS966),y)

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

bootblock-y += romstrap.ld
bootblock-y += romstrap.S

endif
