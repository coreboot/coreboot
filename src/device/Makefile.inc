ramstage-y += device.c
ramstage-y += root_device.c
ramstage-y += cpu_device.c
ramstage-y += device_util.c
ramstage-$(CONFIG_PCI) += pci_device.c
ramstage-$(CONFIG_HYPERTRANSPORT_PLUGIN_SUPPORT) += hypertransport.c
ramstage-$(CONFIG_PCIX_PLUGIN_SUPPORT) += pcix_device.c
ramstage-$(CONFIG_PCIEXP_PLUGIN_SUPPORT) += pciexp_device.c
ramstage-$(CONFIG_CARDBUS_PLUGIN_SUPPORT) += cardbus_device.c
ramstage-$(CONFIG_AZALIA_PLUGIN_SUPPORT) += azalia_device.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += pnp_device.c
ramstage-$(CONFIG_PCI) += pci_ops.c
ramstage-$(CONFIG_PCI) += pci_early.c
ramstage-y += smbus_ops.c

ifeq ($(CONFIG_AZALIA_PLUGIN_SUPPORT),y)
ramstage-srcs += src/mainboard/$(MAINBOARDDIR)/hda_verb.c
endif

romstage-y += device_romstage.c
romstage-$(CONFIG_PCI) += pci_early.c

subdirs-y += oprom dram

ramstage-$(CONFIG_VGA_ROM_RUN) += pci_rom.c

bootblock-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
romstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
ramstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
