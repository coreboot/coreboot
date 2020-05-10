/* SPDX-License-Identifier: GPL-2.0-only */

/* This code should work for all ICH* southbridges with a NIC. */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

static void nic_init(struct device *dev)
{
	/* Nothing yet */
}

static struct device_operations nic_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= nic_init,
};

/* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
/* Note: 82801GU (ICH7-U) doesn't have a NIC. */
/* PCI ID loaded from EEPROM. If EEPROM is 0, 0x27dc is used. */
static const struct pci_driver i82801gx_nic __pci_driver = {
	.ops	= &nic_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27dc,
};
