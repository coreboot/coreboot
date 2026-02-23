/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ehci.h>
#include <device/pci_ops.h>
#include <soc/ehci.h>
#include <soc/pch.h>

static void usb_ehci_set_subsystem(struct device *dev, unsigned int vendor,
	unsigned int device)
{
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	pci_dev_set_subsystem(dev, vendor, device);

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
}

static void ehci_enable(struct device *dev)
{
	if (CONFIG(USBDEBUG))
		dev->enabled = 1;
	else
		pch_disable_devfn(dev);
}

static struct pci_operations ehci_ops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_ehci_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.ops_pci		= &ehci_ops_pci,
	.enable			= ehci_enable,
};

static const unsigned short pci_device_ids[] = {
	0x9c26, /* LynxPoint-LP */
	0x9ca6, /* WildcatPoint */
	0
};

static const struct pci_driver pch_usb_ehci __pci_driver = {
	.ops	 = &usb_ehci_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
