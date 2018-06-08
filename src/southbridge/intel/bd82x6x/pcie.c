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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <southbridge/intel/common/pciehp.h>
#include <assert.h>
#include <southbridge/intel/common/rcba.h>
#include "pch.h"

static void pch_pcie_pm_early(struct device *dev)
{
	u16 link_width_p0, link_width_p4;
	u8 slot_power_limit = 10; /* 10W for x1 */
	u32 reg32;
	u8 reg8;

	reg32 = RCBA32(RPC);

	/* Port 0-3 link aggregation from PCIEPCS1[1:0] soft strap */
	switch (reg32 & 3) {
	case 3:
		link_width_p0 = 4;
		break;
	case 1:
	case 2:
		link_width_p0 = 2;
		break;
	case 0:
	default:
		link_width_p0 = 1;
	}

	/* Port 4-7 link aggregation from PCIEPCS2[1:0] soft strap */
	switch ((reg32 >> 2) & 3) {
	case 3:
		link_width_p4 = 4;
		break;
	case 1:
	case 2:
		link_width_p4 = 2;
		break;
	case 0:
	default:
		link_width_p4 = 1;
	}

	/* Enable dynamic clock gating where needed */
	reg8 = pci_read_config8(dev, 0xe1);
	switch (PCI_FUNC(dev->path.pci.devfn)) {
	case 0: /* Port 0 */
		if (link_width_p0 == 4)
			slot_power_limit = 40; /* 40W for x4 */
		else if (link_width_p0 == 2)
			slot_power_limit = 20; /* 20W for x2 */
		reg8 |= 0x3f;
		break;
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
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;
	enum aspm_type apmc = 0;
	u32 reg32;

	/* Set 0x314 = 0x743a361b */
	pci_write_config32(dev, 0x314, 0x743a361b);

	/* Set 0x318[31:16] = 0x1414 */
	reg32 = pci_read_config32(dev, 0x318);
	reg32 &= 0x0000ffff;
	reg32 |= 0x14140000;
	pci_write_config32(dev, 0x318, reg32);

	/* Set 0x324[5] = 1 */
	reg32 = pci_read_config32(dev, 0x324);
	reg32 |= (1 << 5);
	pci_write_config32(dev, 0x324, reg32);

	/* Set 0x330[7:0] = 0x40 */
	reg32 = pci_read_config32(dev, 0x330);
	reg32 &= ~(0xff);
	reg32 |= 0x40;
	pci_write_config32(dev, 0x330, reg32);

	/* Set 0x33C[24:0] = 0x854c74 */
	reg32 = pci_read_config32(dev, 0x33c);
	reg32 &= 0xff000000;
	reg32 |= 0x00854c74;
	pci_write_config32(dev, 0x33c, reg32);

	/* No IO-APIC, Disable EOI forwarding */
	reg32 = pci_read_config32(dev, 0xd4);
	reg32 |= (1 << 1);
	pci_write_config32(dev, 0xd4, reg32);

	/* Check for a rootport ASPM override */
	switch (PCI_FUNC(dev->path.pci.devfn)) {
	case 0:
		apmc = config->pcie_aspm_f0;
		break;
	case 1:
		apmc = config->pcie_aspm_f1;
		break;
	case 2:
		apmc = config->pcie_aspm_f2;
		break;
	case 3:
		apmc = config->pcie_aspm_f3;
		break;
	case 4:
		apmc = config->pcie_aspm_f4;
		break;
	case 5:
		apmc = config->pcie_aspm_f5;
		break;
	case 6:
		apmc = config->pcie_aspm_f6;
		break;
	case 7:
		apmc = config->pcie_aspm_f7;
		break;
	}

	/* Setup the override or get the real ASPM setting */
	if (apmc) {
		reg32 = pci_read_config32(dev, 0xd4);
		reg32 |= (apmc << 2) | (1 << 4);
		pci_write_config32(dev, 0xd4, reg32);
	} else {
		apmc = pci_read_config32(dev, 0x50) & 3;
	}

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
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;

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

	/* Enable expresscard hotplug events.  */
	if (config->pcie_hotplug_map[PCI_FUNC(dev->path.pci.devfn)]) {
		pci_write_config32(dev, 0xd8,
				   pci_read_config32(dev, 0xd8)
				   | (1 << 30));
		pci_write_config16(dev, 0x42, 0x142);
	}
}

static void pch_pcie_enable(device_t dev)
{
	/* Power Management init before enumeration */
	pch_pcie_pm_early(dev);
}

static void pch_pciexp_scan_bridge(device_t dev)
{
	struct southbridge_intel_bd82x6x_config *config = dev->chip_info;

	/* Normal PCIe Scan */
	pciexp_scan_bridge(dev);

	if (config->pcie_hotplug_map[PCI_FUNC(dev->path.pci.devfn)]) {
		intel_acpi_pcie_hotplug_scan_slot(dev->link_list);
	}

	/* Late Power Management init after bridge device enumeration */
	pch_pcie_pm_late(dev);
}

static const char *pch_pcie_acpi_name(const struct device *dev)
{
	ASSERT(dev);

	if (PCI_SLOT(dev->path.pci.devfn) == 0x1c) {
		static const char *names[] = { "RP01",
				"RP02",
				"RP03",
				"RP04",
				"RP05",
				"RP06",
				"RP07",
				"RP08"};

		return names[PCI_FUNC(dev->path.pci.devfn)];
	}

	return NULL;
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
	.scan_bus		= pch_pciexp_scan_bridge,
	.acpi_name		= pch_pcie_acpi_name,
	.ops_pci		= &pci_ops,
};

static const unsigned short pci_device_ids[] = { 0x1c10, 0x1c12, 0x1c14, 0x1c16,
						 0x1c18, 0x1c1a, 0x1c1c, 0x1c1e,
						 0x1e10, 0x1e12, 0x1e14, 0x1e16,
						 0x1e18, 0x1e1a, 0x1e1c, 0x1e1e,
						 0 };

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
