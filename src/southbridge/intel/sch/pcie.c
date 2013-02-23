/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

static void pci_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing SCH PCIe bridge.\n");

	/* Enable Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Set Cache Line Size to 0x10 */
	// This has no effect but the OS might expect it
	pci_write_config8(dev, 0x0c, 0x10);
	//pci_write_config32(dev, 0x18, 0x11);

	//reg16 = pci_read_config16(dev, 0x3e);
	//reg16 &= ~(1 << 0); /* disable parity error response */
	// reg16 &= ~(1 << 1); /* disable SERR */
	//reg16 |= (1 << 2); /* ISA enable */
	//pci_write_config16(dev, 0x3e, reg16);
	/* Slot implemented. */
	reg16 = pci_read_config16(dev, 0x42);
	reg16 |= (1 << 8);
	pci_write_config16(dev, 0x42, reg16);

	reg16 = pci_read_config16(dev, 0x48);
	reg16 |= 0xf;
	pci_write_config16(dev, 0x48, reg16);
}

static void pcie_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	/* NOTE: This is not the default position! */
	if (!vendor || !device) {
		pci_write_config32(dev, 0x94, pci_read_config32(dev, 0));
	} else {
		pci_write_config32(dev, 0x94,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem = pcie_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pci_scan_bridge,
	.ops_pci		= &pci_ops,
};

/* Port 1 */
static const struct pci_driver sch_pcie_port1 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x8110,
};

/*Port 2 */
static const struct pci_driver sch_pcie_port2 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x8112,
};
