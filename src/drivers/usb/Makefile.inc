romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += ehci_debug.c pci_ehci.c console.c gadget.c

ramstage-$(CONFIG_USBDEBUG) += ehci_debug.c pci_ehci.c console.c gadget.c
