/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <delay.h>
#include "k8t890.h"

/*
 * Note:
 * The pcie bridges are similar to the VX800 ones documented at
 * http://linux.via.com.tw/
 */

static void pcie_common_init(struct device *dev)
{
	u8 reg;
	int i, up;

	/* Disable downstream read cycle retry,
	 * otherwise the bus scan will hang if no device is plugged in. */
	reg = pci_read_config8(dev, 0xa3);
	pci_write_config8(dev, 0xa3, reg & ~0x01);

	/* Use PHY negotiation for lane config */
	reg = pci_read_config8(dev, 0xc1);
	pci_write_config8(dev, 0xc1, reg & ~0x1f);

	/* Award has 0xb, VIA recommends 0xd, default 0x8.
	 * bit4: receive polarity change control
	 * bits3:2: squelch window select 64~175mv
	 * bit1: Number of non-idle bits detected before exiting idle state
	 *       0: 10 bits, 1: 2 bits
	 * bit0: Number of idle bits detected before entering idle state
	 *       0: 10 bits, 1: 2 bits
	 */
	pci_write_config8(dev, 0xe1, 0xb);

	/* Set replay timer limit. */
	pci_write_config8(dev, 0xb1, 0xf0);

	/* Enable link. */
	reg = pci_read_config8(dev, 0x50);
	pci_write_config8(dev, 0x50, reg & ~0x10);

	/* Wait up to 100ms for link to come up */
	up = 0;
	for (i=0; i<1000; i++) {
		if (pci_read_config16(dev, 0x52) & (1<<13)) {
			up = 1;
			break;
		}
		udelay(100);
	}

	printk(BIOS_SPEW, "%s PCIe link ", dev_path(dev));
	if (up)
		printk(BIOS_SPEW, "up after %d us\n", i*100);
	else
		printk(BIOS_SPEW, "timeout\n");

	dump_south(dev);
}

static void peg_init(struct device *dev)
{
	u8 reg;

	printk(BIOS_DEBUG, "Configuring PCIe PEG\n");
	dump_south(dev);

	/* Disable link. */
	reg = pci_read_config8(dev, 0x50);
	pci_write_config8(dev, 0x50, reg | 0x10);

	/*
	 * pci_write_config8(dev, 0xe2, 0x0);
	 * pci_write_config8(dev, 0xe3, 0x92);
	 */

	/* Bit0 = 1 SDP (Start DLLP) always at Lane0. */
	reg = pci_read_config8(dev, 0xb8);
	pci_write_config8(dev, 0xb8, reg | 0x1);

	/*
	 * Downstream wait and Upstream Checking Malformed TLP through
	 * "Byte Enable Rule" And "Over 4K Boundary Rule".
	 */
	reg = pci_read_config8(dev, 0xa4);
	pci_write_config8(dev, 0xa4, reg | 0x30);

	pcie_common_init(dev);
}

static void pcie_init(struct device *dev)
{
	u8 reg;

	printk(BIOS_DEBUG, "Configuring PCIe PEXs\n");
	dump_south(dev);

	/* Disable link. */
	reg = pci_read_config8(dev, 0x50);
	pci_write_config8(dev, 0x50, reg | 0x10);

	pcie_common_init(dev);
}

static const struct device_operations peg_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.enable			= peg_init,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.ops_pci		= 0,
};

static const struct device_operations pcie_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.enable			= pcie_init,
	.scan_bus		= pciexp_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.ops_pci		= 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops	= &peg_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_PEG,
};

static const struct pci_driver pcie_drvd3f0 __pci_driver = {
	.ops	= &pcie_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_PEX0,
};

static const struct pci_driver pcie_drvd3f1 __pci_driver = {
	.ops	= &pcie_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_PEX1,
};

static const struct pci_driver pcie_drvd3f2 __pci_driver = {
	.ops	= &pcie_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_PEX2,
};

static const struct pci_driver pcie_drvd3f3 __pci_driver = {
	.ops	= &pcie_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_PEX3,
};
