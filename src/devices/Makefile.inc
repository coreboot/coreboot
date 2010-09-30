ramstage-y += device.c
ramstage-y += root_device.c
ramstage-y += device_util.c
ramstage-y += pci_device.c
ramstage-$(CONFIG_HYPERTRANSPORT_PLUGIN_SUPPORT) += hypertransport.c
ramstage-y += pcix_device.c
ramstage-y += pciexp_device.c
ramstage-y += agp_device.c
ramstage-y += cardbus_device.c
ramstage-y += pnp_device.c
ramstage-y += pci_ops.c
ramstage-y += smbus_ops.c

subdirs-y += oprom

ifeq ($(CONFIG_PCI_ROM_RUN),y)
ramstage-y += pci_rom.c
else
ramstage-$(CONFIG_VGA_ROM_RUN) += pci_rom.c
endif

