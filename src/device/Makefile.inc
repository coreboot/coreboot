ramstage-y += device.c
ramstage-y += root_device.c
ramstage-y += cpu_device.c
ramstage-y += device_util.c
ramstage-$(CONFIG_PCI) += pci_device.c
ramstage-$(CONFIG_HYPERTRANSPORT_PLUGIN_SUPPORT) += hypertransport.c
ramstage-$(CONFIG_PCIX_PLUGIN_SUPPORT) += pcix_device.c
ramstage-$(CONFIG_PCIEXP_PLUGIN_SUPPORT) += pciexp_device.c
ramstage-$(CONFIG_AGP_PLUGIN_SUPPORT) += agp_device.c
ramstage-$(CONFIG_CARDBUS_PLUGIN_SUPPORT) += cardbus_device.c
ramstage-$(CONFIG_AZALIA_PLUGIN_SUPPORT) += azalia_device.c
ramstage-$(CONFIG_ARCH_X86) += pnp_device.c
ramstage-$(CONFIG_PCI) += pci_ops.c
ramstage-y += smbus_ops.c

romstage-y+= device_romstage.c

subdirs-y += oprom

ifeq ($(CONFIG_PCI_ROM_RUN),y)
ramstage-y += pci_rom.c
else
ramstage-$(CONFIG_VGA_ROM_RUN) += pci_rom.c
endif

