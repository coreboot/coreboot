/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void bcm57xx_disable_aspm(struct device *const dev)
{
	printk(BIOS_INFO, "bcm57xx: Disabling ASPM for %s [%04x/%04x]\n",
	       dev_path(dev), dev->vendor, dev->device);

	dev->disable_pcie_aspm = 1;
}

static struct device_operations bcm57xx_aspm_fixup_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= bcm57xx_disable_aspm,
};

static const unsigned short pci_device_ids[] = {
	0x1677, /* BCM5751 */
	0,
};

static const struct pci_driver bcm57xx_aspm_fixup __pci_driver = {
	.ops	 = &bcm57xx_aspm_fixup_ops,
	.vendor	 = PCI_VENDOR_ID_BROADCOM,
	.devices = pci_device_ids,
};
