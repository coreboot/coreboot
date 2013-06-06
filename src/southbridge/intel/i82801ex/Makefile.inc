ramstage-y += i82801ex.c
ramstage-y += uhci.c
ramstage-y += lpc.c
ramstage-y += ide.c
ramstage-y += sata.c
ramstage-y += ehci.c
ramstage-y += smbus.c
ramstage-y += pci.c
ramstage-y += ac97.c
ramstage-y += watchdog.c
ramstage-y += reset.c

romstage-$(CONFIG_USBDEBUG) += ../i82801gx/usb_debug.c
ramstage-$(CONFIG_USBDEBUG) += ../i82801gx/usb_debug.c
