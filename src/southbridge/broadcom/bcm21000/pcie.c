/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 University of Heidelberg
 * Written by Mondrian Nuessle <nuessle@uni-heidelberg.de> for
 * University of Heidelberg.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

static void pcie_init(struct device *dev)
{
	/* Enable pci error detecting */
	uint32_t dword;
	uint32_t msicap;

	printk(BIOS_DEBUG, "PCIE enable.... dev= %s\n",dev_path(dev));

	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1<<8); /* System error enable */
	dword |= (1<<30); /* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

	/* enable MSI on PCIE: */
	msicap = pci_read_config32(dev, 0xa0);
	msicap |= (1<<16); /* enable MSI*/
	pci_write_config32(dev, 0xa0, msicap);
}

static struct pci_operations lops_pci = {
        .set_subsystem = 0,
};

static struct device_operations pcie_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = pcie_init,
	.scan_bus         = pci_scan_bridge,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver pcie_driver1 __pci_driver = {
	.ops    = &pcie_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM21000_EXB0,
};

static const struct pci_driver pcie_driver2 __pci_driver = {
	.ops    = &pcie_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM21000_EXB1,
};

static const struct pci_driver pcie_driver3 __pci_driver = {
	.ops    = &pcie_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM21000_EXB2,
};
