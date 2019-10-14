bootblock-$(CONFIG_USBDEBUG) += ehci_debug.c pci_ehci.c console.c gadget.c
verstage-$(CONFIG_USBDEBUG) += ehci_debug.c console.c
romstage-$(CONFIG_USBDEBUG) += ehci_debug.c pci_ehci.c console.c gadget.c
postcar-$(CONFIG_USBDEBUG) += ehci_debug.c console.c

ramstage-$(CONFIG_USBDEBUG) += ehci_debug.c pci_ehci.c console.c gadget.c
