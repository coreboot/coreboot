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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include "pch.h"

/* Check if any port in set X to X+3 is enabled */
static int pch_pcie_check_set_enabled(device_t dev)
{
	device_t port;
	int port_func;
	int dev_func = PCI_FUNC(dev->path.pci.devfn);

	printk(BIOS_DEBUG, "%s: check set enabled\n", dev_path(dev));

	/* Go through static device tree list of devices
	 * because enumeration is still in progress */
	for (port = all_devices; port; port = port->next) {
		/* Only care about PCIe root ports */
		if (PCI_SLOT(port->path.pci.devfn) !=
		    PCI_SLOT(dev->path.pci.devfn))
			continue;

		/* Check if port is in range and enabled */
		port_func = PCI_FUNC(port->path.pci.devfn);
		if (port_func >= dev_func &&
		    port_func < (dev_func + 4) &&
		    port->enabled)
			return 1;
	}

	/* None of the ports in this set are enabled */
	return 0;
}

/* RPFN is a write-once register so keep a copy until it is written */
static u32 new_rpfn;

/* Swap function numbers assigned to two PCIe Root Ports */
static void pch_pcie_function_swap(u8 old_fn, u8 new_fn)
{
	u32 old_rpfn = new_rpfn;

	printk(BIOS_DEBUG, "PCH: Remap PCIe function %d to %d\n",
	       old_fn, new_fn);

	new_rpfn &= ~(RPFN_FNMASK(old_fn) | RPFN_FNMASK(new_fn));

	/* Old function set to new function and disabled */
	new_rpfn |= RPFN_FNSET(old_fn, RPFN_FNGET(old_rpfn, new_fn));
	new_rpfn |= RPFN_FNSET(new_fn, RPFN_FNGET(old_rpfn, old_fn));
}

/* Update devicetree with new Root Port function number assignment */
static void pch_pcie_devicetree_update(void)
{
	device_t dev;

	/* Update the function numbers in the static devicetree */
	for (dev = all_devices; dev; dev = dev->next) {
		u8 new_devfn;

		/* Only care about PCH PCIe root ports */
		if (PCI_SLOT(dev->path.pci.devfn) !=
		    PCH_PCIE_DEV_SLOT)
			continue;

		/* Determine the new devfn for this port */
		new_devfn = PCI_DEVFN(PCH_PCIE_DEV_SLOT,
			      RPFN_FNGET(new_rpfn,
				 PCI_FUNC(dev->path.pci.devfn)));

		if (dev->path.pci.devfn != new_devfn) {
			printk(BIOS_DEBUG,
			       "PCH: PCIe map %02x.%1x -> %02x.%1x\n",
			       PCI_SLOT(dev->path.pci.devfn),
			       PCI_FUNC(dev->path.pci.devfn),
			       PCI_SLOT(new_devfn), PCI_FUNC(new_devfn));

			dev->path.pci.devfn = new_devfn;
		}
	}
}

/* Special handling for PCIe Root Port devices */
void pch_pcie_enable_dev(device_t dev)
{
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;
	u32 reg32;

	/*
	 * Save a copy of the Root Port Function Number map when
	 * starting to walk the list of PCIe Root Ports so it can
	 * be updated locally and written out when the last port
	 * has been processed.
	 */
	if (PCI_FUNC(dev->path.pci.devfn) == 0) {
		new_rpfn = RCBA32(RPFN);

		/*
		 * Enable Root Port coalescing if the first port is disabled
		 * or the other devices will not be enumerated by the OS.
		 */
		if (!dev->enabled)
			config->pcie_port_coalesce = 1;

		if (config->pcie_port_coalesce)
			printk(BIOS_INFO,
			       "PCH: PCIe Root Port coalescing is enabled\n");
	}

	if (!dev->enabled) {
		printk(BIOS_DEBUG, "%s: Disabling device\n",  dev_path(dev));

		/*
		 * PCIE Power Savings for PantherPoint and CougarPoint/B1+
		 *
		 * If PCIe 0-3 disabled set Function 0 0xE2[0] = 1
		 * If PCIe 4-7 disabled set Function 4 0xE2[0] = 1
		 *
		 * This check is done here instead of pcie driver
		 * because the pcie driver enable() handler is not
		 * called unless the device is enabled.
		 */
		if ((PCI_FUNC(dev->path.pci.devfn) == 0 ||
		     PCI_FUNC(dev->path.pci.devfn) == 4)) {
			/* Handle workaround for PPT and CPT/B1+ */
			if (!pch_pcie_check_set_enabled(dev)) {
				u8 reg8 = pci_read_config8(dev, 0xe2);
				reg8 |= 1;
				pci_write_config8(dev, 0xe2, reg8);
			}

			/*
			 * Enable Clock Gating for shared PCIe resources
			 * before disabling this particular port.
			 */
			pci_write_config8(dev, 0xe1, 0x3c);
		}

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Do not claim downstream transactions for PCIe ports */
		new_rpfn |= RPFN_HIDE(PCI_FUNC(dev->path.pci.devfn));

		/* Disable this device if possible */
		pch_disable_devfn(dev);
	} else {
		int fn;

		/*
		 * Check if there is a lower disabled port to swap with this
		 * port in order to maintain linear order starting at zero.
		 */
		if (config->pcie_port_coalesce) {
			for (fn=0; fn < PCI_FUNC(dev->path.pci.devfn); fn++) {
				if (!(new_rpfn & RPFN_HIDE(fn)))
					continue;

				/* Swap places with this function */
				pch_pcie_function_swap(
					PCI_FUNC(dev->path.pci.devfn), fn);
				break;
			}
		}

		/* Enable SERR */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 |= PCI_COMMAND_SERR;
		pci_write_config32(dev, PCI_COMMAND, reg32);
	}

	/*
	 * When processing the last PCIe root port we can now
	 * update the Root Port Function Number and Hide register.
	 */
	if (PCI_FUNC(dev->path.pci.devfn) == 7) {
		printk(BIOS_SPEW, "PCH: RPFN 0x%08x -> 0x%08x\n",
		       RCBA32(RPFN), new_rpfn);
		RCBA32(RPFN) = new_rpfn;

		/* Update static devictree with new function numbers */
		if (config->pcie_port_coalesce)
			pch_pcie_devicetree_update();
	}
}

static void pch_pcie_pm_early(struct device *dev)
{
/* RPC has been moved. It is in PCI config space now.  */
#if 0
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
#endif
}

static void pch_pcie_pm_late(struct device *dev)
{
	enum aspm_type apmc;
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

static const unsigned short pci_device_ids[] = {
	/* Lynxpoint Mobile */
	0x8c10, 0x8c12, 0x8c14, 0x8c16, 0x8c18, 0x8c1a, 0x8c1c, 0x8c1e,
	/* Lynxpoint Low Power */
	0x9c10, 0x9c12, 0x9c14, 0x9c16, 0x9c18, 0x9c1a,
	0
};

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
