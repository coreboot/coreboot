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

/* LynxPoint-LP has 6 root ports while non-LP has 8. */
#define MAX_NUM_ROOT_PORTS 8
#define H_NUM_ROOT_PORTS MAX_NUM_ROOT_PORTS
#define LP_NUM_ROOT_PORTS (MAX_NUM_ROOT_PORTS - 2)

struct root_port_config {
	/* RPFN is a write-once register so keep a copy until it is written */
	u32 orig_rpfn;
	u32 new_rpfn;
	u32 pin_ownership;
	u32 strpfusecfg1;
	u32 strpfusecfg2;
	u32 strpfusecfg3;
	int coalesce;
	int gbe_port;
	int num_ports;
	device_t ports[MAX_NUM_ROOT_PORTS];
};

static struct root_port_config rpc;

static inline int max_root_ports(void)
{
	if (pch_is_lp())
		return LP_NUM_ROOT_PORTS;
	else
		return H_NUM_ROOT_PORTS;
}

static inline int root_port_is_first(device_t dev)
{
	return PCI_FUNC(dev->path.pci.devfn) == 0;
}

static inline int root_port_is_last(device_t dev)
{
	return PCI_FUNC(dev->path.pci.devfn) == (rpc.num_ports - 1);
}

/* Root ports are numbered 1..N in the documentation. */
static inline int root_port_number(device_t dev)
{
	return PCI_FUNC(dev->path.pci.devfn) + 1;
}

static void root_port_config_update_gbe_port(void)
{
	/* Is the Gbe Port enabled? */
	if (!((rpc.strpfusecfg1 >> 19) & 1))
		return;

	if (pch_is_lp()) {
		switch ((rpc.strpfusecfg1 >> 16) & 0x7) {
		case 0:
			rpc.gbe_port = 3;
			break;
		case 1:
			rpc.gbe_port = 4;
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			/* Lanes 0-4 of Root Port 5. */
			rpc.gbe_port = 5;
			break;
		default:
			printk(BIOS_DEBUG, "Invalid GbE Port Selection.\n");
		}
	} else {
		/* Non-LP has 1:1 mapping with root ports. */
		rpc.gbe_port = ((rpc.strpfusecfg1 >> 16) & 0x7) + 1;
	}
}

static void root_port_init_config(device_t dev)
{
	int rp;

	if (root_port_is_first(dev)) {
		rpc.orig_rpfn = RCBA32(RPFN);
		rpc.new_rpfn = rpc.orig_rpfn;
		rpc.num_ports = max_root_ports();
		rpc.gbe_port = -1;

		rpc.pin_ownership = pci_read_config32(dev, 0x410);
		root_port_config_update_gbe_port();

		if (dev->chip_info != NULL) {
			struct southbridge_intel_lynxpoint_config *config;

			config = dev->chip_info;
			rpc.coalesce = config->pcie_port_coalesce;
		}
	}

	rp = root_port_number(dev);
	if (rp > rpc.num_ports) {
		printk(BIOS_ERR, "Found Root Port %d, expecting %d\n",
		       rp, rpc.num_ports);
		return;
	}

	/* Read the fuse configuration and pin ownership. */
	switch (rp) {
	case 1:
		rpc.strpfusecfg1 = pci_read_config32(dev, 0xfc);
		break;
	case 5:
		rpc.strpfusecfg2 = pci_read_config32(dev, 0xfc);
		break;
	case 6:
		rpc.strpfusecfg3 = pci_read_config32(dev, 0xfc);
		break;
	default:
		break;
	}

	/* Cache pci device. */
	rpc.ports[rp - 1] = dev;
}

/* Update devicetree with new Root Port function number assignment */
static void pch_pcie_device_set_func(int index, int pci_func)
{
	device_t dev;
	unsigned new_devfn;

	dev = rpc.ports[index];

	/* Set the new PCI function field for this Root Port. */
	rpc.new_rpfn &= ~RPFN_FNMASK(index);
	rpc.new_rpfn |= RPFN_FNSET(index, pci_func);

	/* Determine the new devfn for this port */
	new_devfn = PCI_DEVFN(PCH_PCIE_DEV_SLOT, pci_func);

	if (dev->path.pci.devfn != new_devfn) {
		printk(BIOS_DEBUG,
		       "PCH: PCIe map %02x.%1x -> %02x.%1x\n",
		       PCI_SLOT(dev->path.pci.devfn),
		       PCI_FUNC(dev->path.pci.devfn),
		       PCI_SLOT(new_devfn), PCI_FUNC(new_devfn));

		dev->path.pci.devfn = new_devfn;
	}
}

static void root_port_commit_config(void)
{
	int i;

	/* If the first root port is disabled the coalesce ports. */
	if (!rpc.ports[0]->enabled)
		rpc.coalesce = 1;

	for (i = 0; i < rpc.num_ports; i++) {
		device_t dev;
		u32 reg32;

		dev = rpc.ports[i];

		if (dev == NULL) {
			printk(BIOS_ERR, "Root Port %d device is NULL?\n", i+1);
			continue;
		}

		if (dev->enabled)
			continue;

		printk(BIOS_DEBUG, "%s: Disabling device\n",  dev_path(dev));

		/* Ensure memory, io, and bus master are all disabled */
		reg32 = pci_read_config32(dev, PCI_COMMAND);
		reg32 &= ~(PCI_COMMAND_MASTER |
			   PCI_COMMAND_MEMORY | PCI_COMMAND_IO);
		pci_write_config32(dev, PCI_COMMAND, reg32);

		/* Disable this device if possible */
		pch_disable_devfn(dev);
	}

	if (rpc.coalesce) {
		int current_func;

		/* For all Root Ports N enabled ports get assigned the lower
		 * PCI function number. The disabled ones get upper PCI
		 * function numbers. */
		current_func = 0;
		for (i = 0; i < rpc.num_ports; i++) {
			if (!rpc.ports[i]->enabled)
				continue;
			pch_pcie_device_set_func(i, current_func);
			current_func++;
		}

		/* Allocate the disabled devices' PCI function number. */
		for (i = 0; i < rpc.num_ports; i++) {
			if (rpc.ports[i]->enabled)
				continue;
			pch_pcie_device_set_func(i, current_func);
			current_func++;
		}
	}

	printk(BIOS_SPEW, "PCH: RPFN 0x%08x -> 0x%08x\n",
	       rpc.orig_rpfn, rpc.new_rpfn);
	RCBA32(RPFN) = rpc.new_rpfn;
}

static void root_port_mark_disable(device_t dev)
{
	/* Mark device as disabled. */
	dev->enabled = 0;
	/* Mark device to be hidden. */
	rpc.new_rpfn |= RPFN_HIDE(PCI_FUNC(dev->path.pci.devfn));
}

static void root_port_check_disable(device_t dev)
{
	int rp;
	int is_lp;

	/* Device already disabled. */
	if (!dev->enabled) {
		root_port_mark_disable(dev);
		return;
	}

	rp = root_port_number(dev);

	/* Is the GbE port mapped to this Root Port? */
	if (rp == rpc.gbe_port) {
		root_port_mark_disable(dev);
		return;
	}

	is_lp = pch_is_lp();

	/* Check Root Port Configuration. */
	switch (rp) {
		case 2:
			/* Root Port 2 is disabled for all lane configurations
			 * but config 00b (4x1 links). */
			if ((rpc.strpfusecfg1 >> 14) & 0x3) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 3:
			/* Root Port 3 is disabled in config 11b (1x4 links). */
			if (((rpc.strpfusecfg1 >> 14) & 0x3) == 0x3) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 4:
			/* Root Port 4 is disabled in configs 11b (1x4 links)
			 * and 10b (2x2 links). */
			if ((rpc.strpfusecfg1 >> 14) & 0x2) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 6:
			if (is_lp)
				break;
			/* Root Port 6 is disabled for all lane configurations
			 * but config 00b (4x1 links). */
			if ((rpc.strpfusecfg2 >> 14) & 0x3) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 7:
			if (is_lp)
				break;
			/* Root Port 3 is disabled in config 11b (1x4 links). */
			if (((rpc.strpfusecfg2 >> 14) & 0x3) == 0x3) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 8:
			if (is_lp)
				break;
			/* Root Port 8 is disabled in configs 11b (1x4 links)
			 * and 10b (2x2 links). */
			if ((rpc.strpfusecfg2 >> 14) & 0x2) {
				root_port_mark_disable(dev);
				return;
			}
			break;
	}

	/* Check Pin Ownership. */
	if (is_lp) {
		switch (rp) {
		case 1:
			/* Bit 0 is Root Port 1 ownership. */
			if ((rpc.pin_ownership & 0x1) == 0) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 2:
			/* Bit 2 is Root Port 2 ownership. */
			if ((rpc.pin_ownership & 0x4) == 0) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 6:
			/* Bits 7:4 are Root Port 6 pin-lane ownership. */
			if ((rpc.pin_ownership & 0xf0) == 0) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		}
	} else {
		switch (rp) {
		case 1:
			/* Bits 4 and 0 are Root Port 1 ownership. */
			if ((rpc.pin_ownership & 0x11) == 0) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		case 2:
			/* Bits 5 and 2 are Root Port 2 ownership. */
			if ((rpc.pin_ownership & 0x24) == 0) {
				root_port_mark_disable(dev);
				return;
			}
			break;
		}
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

	/* Enable SERR */
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_SERR;
	pci_write_config32(dev, PCI_COMMAND, reg32);

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
	/* Add this device to the root port config structure. */
	root_port_init_config(dev);

	/* Check to see if this Root Port should be disabled. */
	root_port_check_disable(dev);

	/* Power Management init before enumeration */
	if (dev->enabled)
		pch_pcie_pm_early(dev);

	/*
	 * When processing the last PCIe root port we can now
	 * update the Root Port Function Number and Hide register.
	 */
	if (root_port_is_last(dev))
		root_port_commit_config();
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
