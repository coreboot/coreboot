ramstage-y += device.c
ramstage-y += root_device.c
ramstage-y += cpu_device.c
ramstage-y += device_util.c
ramstage-$(CONFIG_AZALIA_PLUGIN_SUPPORT) += azalia_device.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += pnp_device.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_64) += pnp_device.c
ramstage-y += smbus_ops.c

ifeq ($(CONFIG_AZALIA_PLUGIN_SUPPORT),y)
ramstage-srcs += $(wildcard src/mainboard/$(MAINBOARDDIR)/hda_verb.c)
endif

bootblock-y += device_const.c
postcar-y += device_const.c
smm-y += device_const.c
verstage-y += device_const.c
romstage-y += device_const.c
ramstage-y += device_const.c

ifeq ($(CONFIG_PCI),y)
bootblock-y += pci_early.c
verstage-y += pci_early.c
romstage-y += pci_early.c
postcar-y += pci_early.c

ramstage-y += pci_class.c
ramstage-y += pci_device.c
ramstage-y += pci_rom.c

bootblock-y += pci_ops.c
verstage-y += pci_ops.c
romstage-y += pci_ops.c
postcar-y += pci_ops.c
ramstage-y += pci_ops.c
smm-y += pci_ops.c

ramstage-$(CONFIG_PCIX_PLUGIN_SUPPORT) += pcix_device.c
ramstage-$(CONFIG_PCIEXP_PLUGIN_SUPPORT) += pciexp_device.c
ramstage-$(CONFIG_CARDBUS_PLUGIN_SUPPORT) += cardbus_device.c
endif

subdirs-y += oprom dram

bootblock-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
verstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
romstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c
ramstage-$(CONFIG_SOFTWARE_I2C) += software_i2c.c

bootblock-y += i2c.c
verstage-y += i2c.c
romstage-y += i2c.c
ramstage-y += i2c.c
ramstage-y += i2c_bus.c

bootblock-y += mmio.c
verstage-y += mmio.c
romstage-y += mmio.c
ramstage-y += mmio.c

ramstage-y += resource_allocator_common.c
ramstage-y += resource_allocator_v4.c

ramstage-$(CONFIG_XHCI_UTILS) += xhci.c

ramstage-y += gpio.c
ramstage-y += mdio.c
