obj-y += device.o
obj-y += root_device.o
obj-y += device_util.o
obj-y += pci_device.o
obj-$(CONFIG_HYPERTRANSPORT_PLUGIN_SUPPORT) += hypertransport.o
obj-y += pcix_device.o
obj-y += pciexp_device.o
obj-y += agp_device.o
obj-y += cardbus_device.o
obj-y += pnp_device.o
obj-y += pci_ops.o
obj-y += smbus_ops.o

ifeq ($(CONFIG_PCI_ROM_RUN),y)
obj-y += pci_rom.o
subdirs-y += ../../util/x86emu
else
obj-$(CONFIG_VGA_ROM_RUN) += pci_rom.o
subdirs-$(CONFIG_VGA_ROM_RUN) += ../../util/x86emu
endif

