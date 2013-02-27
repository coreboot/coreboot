/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *               2012 secunet Security Networks AG
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
#include <device/pciexp.h>
#include <device/pci_ids.h>

static void pci_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing ICH9 PCIe root port.\n");

	/* Enable Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Set Cache Line Size to 0x10 */
	// This has no effect but the OS might expect it
	pci_write_config8(dev, 0x0c, 0x10);

	reg16 = pci_read_config16(dev, 0x3e);
	reg16 &= ~(1 << 0); /* disable parity error response */
	reg16 |= (1 << 2); /* ISA enable */
	pci_write_config16(dev, 0x3e, reg16);

	/* Enable IO xAPIC on this PCIe port */
	reg32 = pci_read_config32(dev, 0xd8);
	reg32 |= (1 << 7);
	pci_write_config32(dev, 0xd8, reg32);

	/* Enable Backbone Clock Gating */
	reg32 = pci_read_config32(dev, 0xe1);
	reg32 |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
	pci_write_config32(dev, 0xe1, reg32);

#if CONFIG_MMCONF_SUPPORT
	/* Set VC0 transaction class */
	reg32 = pci_mmio_read_config32(dev, 0x114);
	reg32 &= 0xffffff00;
	reg32 |= 1;
	pci_mmio_write_config32(dev, 0x114, reg32);

	/* Mask completion timeouts */
	reg32 = pci_mmio_read_config32(dev, 0x148);
	reg32 |= (1 << 14);
	pci_mmio_write_config32(dev, 0x148, reg32);

	/* Lock R/WO Correctable Error Mask. */
	pci_mmio_write_config32(dev, 0x154, pci_mmio_read_config32(dev, 0x154));
#else
#error "MMIO needed for ICH9 PCIe"
#endif

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, 0x06);
	pci_write_config16(dev, 0x06, reg16);
	reg16 = pci_read_config16(dev, 0x1e);
	pci_write_config16(dev, 0x1e, reg16);

	/* Get configured ASPM state */
	const enum aspm_type apmc = pci_read_config32(dev, 0x50) & 3;

	/* If both L0s and L1 enabled then set root port 0xE8[1]=1 */
	if (apmc == PCIE_ASPM_BOTH) {
		reg32 = pci_read_config32(dev, 0xe8);
		reg32 |= (1 << 1);
		pci_write_config32(dev, 0xe8, reg32);
	}
}

static void pcie_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	/* NOTE: 0x94 is not the default position! */
	if (!vendor || !device) {
		pci_write_config32(dev, 0x94,
				pci_read_config32(dev, 0));
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
	.scan_bus		= pciexp_scan_bridge,
	.ops_pci		= &pci_ops,
};

/* 82801Ix (ICH9DH/ICH9DO/ICH9R/ICH9/ICH9M-E/ICH9M) */
static const unsigned short pci_device_ids[] = {
	0x2940, /* Port 1 */
	0x2942, /* Port 2 */
	0x2944, /* Port 3 */
	0x2946, /* Port 4 */
	0x2948, /* Port 5 */
	0x294a, /* Port 6 */
	0
};
static const struct pci_driver ich9_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= pci_device_ids,
};

