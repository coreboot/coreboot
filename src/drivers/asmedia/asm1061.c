/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <option.h>

#define ASM1061_CTRL_REG 0xec
#define ASM1061_PCI_CFG_UNLOCK (1 << 2)

static void asm1061_enable(struct device *const dev)
{
	printk(BIOS_INFO, "Disabling ASPM for %s [%04x/%04x]\n",
	       dev_path(dev), dev->vendor, dev->device);
	dev->disable_pcie_aspm = 1;

	u8 sata_mode = get_uint_option("sata_mode", 0);
	pci_or_config8(dev, ASM1061_CTRL_REG, ASM1061_PCI_CFG_UNLOCK);
	if (sata_mode == 0) {
		printk(BIOS_INFO, "Setting AHCI mode for %s [%04x/%04x]\n",
		       dev_path(dev), dev->vendor, dev->device);
		pci_write_config16(dev, PCI_CLASS_DEVICE, PCI_CLASS_STORAGE_SATA);
	} else {
		printk(BIOS_INFO, "Setting IDE mode for %s [%04x/%04x]\n",
		       dev_path(dev), dev->vendor, dev->device);
		pci_write_config16(dev, PCI_CLASS_DEVICE, PCI_CLASS_STORAGE_IDE);
	}
	pci_and_config8(dev, ASM1061_CTRL_REG, ~ASM1061_PCI_CFG_UNLOCK);
}

static struct device_operations asm1061_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= asm1061_enable,
};

static const unsigned short pci_device_ids[] = {
	0x0611, /* ASM1061 SATA IDE Controller */
	0
};

static const struct pci_driver asmedia_asm1061 __pci_driver = {
	.ops		= &asm1061_ops,
	.vendor		= 0x1b21,
	.devices	= pci_device_ids,
};
