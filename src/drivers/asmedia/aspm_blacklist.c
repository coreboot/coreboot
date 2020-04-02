/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>

static void disable_aspm(struct device *const dev)
{
	printk(BIOS_INFO, "Disabling ASPM for %s [%04x/%04x]\n",
	       dev_path(dev), dev->vendor, dev->device);
	dev->disable_pcie_aspm = 1;
}

static struct device_operations asmedia_noaspm_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= disable_aspm,
};

static const unsigned short pci_device_ids[] = {
	0x0611, /* ASM1061 SATA IDE Controller */
	0
};

static const struct pci_driver asmedia_noaspm __pci_driver = {
	.ops	 = &asmedia_noaspm_ops,
	.vendor	 = 0x1b21,
	.devices = pci_device_ids,
};
