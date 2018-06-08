romstage-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += ehci_debug.c pci_ehci.c console.c gadget.c
postcar-$(CONFIG_USBDEBUG_IN_ROMSTAGE) += ehci_debug.c console.c

ramstage-$(CONFIG_USBDEBUG) += ehci_debug.c pci_ehci.c console.c gadget.c
