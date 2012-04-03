/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include "pch.h"

static u16 pcie_port_link_width(int port)
{
	u16 link_width;

	link_width = pci_read_config16(
		dev_find_slot(0, PCI_DEVFN(0x1c, port)), 0x52);
	link_width >>= 4;
	link_width &= 0x3f;
	return link_width;
}

static void pch_pcie_pm_early(struct device *dev)
{
	u16 link_width_p0, link_width_p4;
	u8 slot_power_limit = 10; /* 10W for x1 */
	u32 reg32;
	u8 reg8;

	link_width_p0 = pcie_port_link_width(0);
	link_width_p4 = pcie_port_link_width(4);

	/* Enable dynamic clock gating where needed */
	reg8 = pci_read_config8(dev, 0xe1);
	switch (PCI_FUNC(dev->path.pci.devfn)) {
	case 0: /* Port 0 */
		if (link_width_p0 == 4)
			slot_power_limit = 40; /* 40W for x4 */
		else if (link_width_p0 == 2)
			slot_power_limit = 20; /* 20W for x2 */
	case 4: /* Port 4 */
		if (link_width_p4 == 4)
			slot_power_limit = 40; /* 40W for x4 */
		else if (link_width_p4 == 2)
			slot_power_limit = 20; /* 20W for x2 */
		reg8 |= 0x3f;
		break;
	case 1: /* Port 1 only if Port 0 is x1 */
		if (link_width_p0 == 1)
			reg8 |= 0x3;
		break;
	case 2: /* Port 2 only if Port 0 is x1 or x2 */
	case 3: /* Port 3 only if Port 0 is x1 or x2 */
		if (link_width_p0 <= 2)
			reg8 |= 0x3;
		break;
	case 5: /* Port 5 only if Port 4 is x1 */
		if (link_width_p4 == 1)
			reg8 |= 0x3;
		break;
	case 6: /* Port 7 only if Port 4 is x1 or x2 */
	case 7: /* Port 7 only if Port 4 is x1 or x2 */
		if (link_width_p4 <= 2)
			reg8 |= 0x3;
		break;
	}
	pci_write_config8(dev, 0xe1, reg8);

	/* Set 0xE8[0] = 1 */
	reg32 = pci_read_config32(dev, 0xe8);
	reg32 |= 1;
	pci_write_config32(dev, 0xe8, reg32);

	/* Adjust Common Clock exit latency */
	reg32 = pci_read_config32(dev, 0xd8);
	reg32 &= ~(1 << 17);
	reg32 |= (1 << 16) | (1 << 15);
	reg32 &= ~(1 << 31); /* Disable PME# SCI for native PME handling */
	pci_write_config32(dev, 0xd8, reg32);

	/* Adjust ASPM L1 exit latency */
	reg32 = pci_read_config32(dev, 0x4c);
	reg32 &= ~((1 << 17) | (1 << 16) | (1 << 15));
	if (RCBA32(0x2320) & (1 << 16)) {
		/* If RCBA+2320[15]=1 set ASPM L1 to 8-16us */
		reg32 |= (1 << 17);
	} else {
		/* Else set ASPM L1 to 2-4us */
		reg32 |= (1 << 16);
	}
	pci_write_config32(dev, 0x4c, reg32);

	/* Set slot power limit as configured above */
	reg32 = pci_read_config32(dev, 0x54);
	reg32 &= ~((1 << 15) | (1 << 16)); /* 16:15 = Slot power scale */
	reg32 &= ~(0xff << 7);             /* 14:7  = Slot power limit */
	reg32 |= (slot_power_limit << 7);
	pci_write_config32(dev, 0x54, reg32);
}

static void pch_pcie_pm_late(struct device *dev)
{
	enum aspm_type apmc;
	u32 reg32;

	/* Set 0x314 = 0x743a361b */
	pci_mmio_write_config32(dev, 0x314, 0x743a361b);

	/* Set 0x318[31:16] = 0x1414 */
	reg32 = pci_mmio_read_config32(dev, 0x318);
	reg32 &= 0x0000ffff;
	reg32 |= 0x14140000;
	pci_mmio_write_config32(dev, 0x318, reg32);

	/* Set 0x324[5] = 1 */
	reg32 = pci_mmio_read_config32(dev, 0x324);
	reg32 |= (1 << 5);
	pci_mmio_write_config32(dev, 0x324, reg32);

	/* Set 0x330[7:0] = 0x40 */
	reg32 = pci_mmio_read_config32(dev, 0x330);
	reg32 &= ~(0xff);
	reg32 |= 0x40;
	pci_mmio_write_config32(dev, 0x330, reg32);

	/* Set 0x33C[24:0] = 0x854c74 */
	reg32 = pci_mmio_read_config32(dev, 0x33c);
	reg32 &= 0xff000000;
	reg32 |= 0x00854c74;
	pci_mmio_write_config32(dev, 0x33c, reg32);

	/* No IO-APIC, Disable EOI forwarding */
	reg32 = pci_read_config32(dev, 0xd4);
	reg32 |= (1 << 1);
	pci_write_config32(dev, 0xd4, reg32);

	/* Get configured ASPM state */
	apmc = pci_read_config32(dev, 0x50) & 3;

	/* If both L0s and L1 enabled then set root port 0xE8[1]=1 */
	if (apmc == PCIE_ASPM_BOTH) {
		reg32 = pci_read_config32(dev, 0xe8);
		reg32 |= (1 << 1);
		pci_write_config32(dev, 0xe8, reg32);
	}
}

static void pci_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing PCH PCIe bridge.\n");

	/* Enable Bus Master */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Set Cache Line Size to 0x10 */
	// This has no effect but the OS might expect it
	pci_write_config8(dev, 0x0c, 0x10);

	reg16 = pci_read_config16(dev, 0x3e);
	reg16 &= ~(1 << 0); /* disable parity error response */
	// reg16 &= ~(1 << 1); /* disable SERR */
	reg16 |= (1 << 2); /* ISA enable */
	pci_write_config16(dev, 0x3e, reg16);

#ifdef EVEN_MORE_DEBUG
	reg32 = pci_read_config32(dev, 0x20);
	printk(BIOS_SPEW, "    MBL    = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x24);
	printk(BIOS_SPEW, "    PMBL   = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x28);
	printk(BIOS_SPEW, "    PMBU32 = 0x%08x\n", reg32);
	reg32 = pci_read_config32(dev, 0x2c);
	printk(BIOS_SPEW, "    PMLU32 = 0x%08x\n", reg32);
#endif

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, 0x06);
	//reg16 |= 0xf900;
	pci_write_config16(dev, 0x06, reg16);

	reg16 = pci_read_config16(dev, 0x1e);
	//reg16 |= 0xf900;
	pci_write_config16(dev, 0x1e, reg16);

	/* Power Management init after enumeration */
	pch_pcie_pm_late(dev);
}

static void pch_pcie_enable(device_t dev)
{
	/* Power Management init before enumeration */
	pch_pcie_pm_early(dev);
}

static void pcie_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	/* NOTE: This is not the default position! */
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
	.enable			= pch_pcie_enable,
	.scan_bus		= pciexp_scan_bridge,
	.ops_pci		= &pci_ops,
};

static const struct pci_driver pch_pcie_port1 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c10,	/* D28:F0 */
};

static const struct pci_driver pch_pcie_port1_a __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1e10,	/* D28:F0 */
};

static const struct pci_driver pch_pcie_port2 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c12,	/* D28:F1 */
};

static const struct pci_driver pch_pcie_port3 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c14,	/* D28:F2 */
};

static const struct pci_driver pch_pcie_port3_a __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1e14,	/* D28:F2 */
};

static const struct pci_driver pch_pcie_port4 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c16,	/* D28:F3 */
};

static const struct pci_driver pch_pcie_port4_a __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1e16,	/* D28:F3 */
};

static const struct pci_driver pch_pcie_port5 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c18,	/* D28:F4 */
};

static const struct pci_driver pch_pcie_port6 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c1a,	/* D28:F5 */
};

static const struct pci_driver pch_pcie_port7 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c1c,	/* D28:F6 */
};

static const struct pci_driver pch_pcie_port8 __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c1e,	/* D28:F7 */
};
