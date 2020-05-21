/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "i82801gx.h"

/* Low Power variant has 6 root ports. */
#define NUM_ROOT_PORTS 6

struct root_port_config {
	/* RPFN is a write-once register so keep a copy until it is written */
	u32 orig_rpfn;
	u32 new_rpfn;
	int num_ports;
	struct device *ports[NUM_ROOT_PORTS];
};

static struct root_port_config rpc;

static inline int root_port_is_first(struct device *dev)
{
	return PCI_FUNC(dev->path.pci.devfn) == 0;
}

static inline int root_port_is_last(struct device *dev)
{
	return PCI_FUNC(dev->path.pci.devfn) == (rpc.num_ports - 1);
}

/* Root ports are numbered 1..N in the documentation. */
static inline int root_port_number(struct device *dev)
{
	return PCI_FUNC(dev->path.pci.devfn) + 1;
}

static void pci_init(struct device *dev)
{
	u16 reg16;
	u32 reg32;

	printk(BIOS_DEBUG, "Initializing ICH7 PCIe bridge.\n");

	/* Enable Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Set Cache Line Size to 0x10 */
	// This has no effect but the OS might expect it
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, 0x10);

	reg16 = pci_read_config16(dev, PCI_BRIDGE_CONTROL);
	reg16 &= ~PCI_BRIDGE_CTL_PARITY;
	reg16 |= PCI_BRIDGE_CTL_NO_ISA;
	pci_write_config16(dev, PCI_BRIDGE_CONTROL, reg16);

	/* Enable IO xAPIC on this PCIe port */
	reg32 = pci_read_config32(dev, 0xd8);
	reg32 |= (1 << 7);
	pci_write_config32(dev, 0xd8, reg32);

	/* Enable Backbone Clock Gating */
	reg32 = pci_read_config32(dev, 0xe1);
	reg32 |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
	pci_write_config32(dev, 0xe1, reg32);

	/* Set VC0 transaction class */
	reg32 = pci_read_config32(dev, 0x114);
	reg32 &= 0xffffff00;
	reg32 |= 1;
	pci_write_config32(dev, 0x114, reg32);

	/* Mask completion timeouts */
	reg32 = pci_read_config32(dev, 0x148);
	reg32 |= (1 << 14);
	pci_write_config32(dev, 0x148, reg32);

	/* Enable common clock configuration */
	// Are there cases when we don't want that?
	reg16 = pci_read_config16(dev, 0x50);
	reg16 |= (1 << 6);
	pci_write_config16(dev, 0x50, reg16);

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
}

static int get_num_ports(void)
{
	struct device *dev = pcidev_on_root(31, 0);
	if (pci_read_config32(dev, FDVCT) & PCIE_4_PORTS_MAX)
		return 4;
	else
		return 6;
}

static void root_port_init_config(struct device *dev)
{
	int rp;

	if (root_port_is_first(dev)) {
		rpc.orig_rpfn = RCBA32(RPFN);
		rpc.new_rpfn = rpc.orig_rpfn;
		rpc.num_ports = get_num_ports();
	}

	rp = root_port_number(dev);
	if (rp > rpc.num_ports) {
		printk(BIOS_ERR, "Found Root Port %d, expecting %d\n", rp, rpc.num_ports);
		return;
	}

	/* Cache pci device. */
	rpc.ports[rp - 1] = dev;
}

/* Update devicetree with new Root Port function number assignment */
static void ich_pcie_device_set_func(int index, int pci_func)
{
	struct device *dev;
	unsigned int new_devfn;

	dev = rpc.ports[index];

	/* Set the new PCI function field for this Root Port. */
	rpc.new_rpfn &= ~RPFN_FNMASK(index);
	rpc.new_rpfn |= RPFN_FNSET(index, pci_func);

	/* Determine the new devfn for this port */
	new_devfn = PCI_DEVFN(ICH_PCIE_DEV_SLOT, pci_func);

	if (dev->path.pci.devfn != new_devfn) {
		printk(BIOS_DEBUG,
		       "ICH: PCIe map %02x.%1x -> %02x.%1x\n",
		       PCI_SLOT(dev->path.pci.devfn),
		       PCI_FUNC(dev->path.pci.devfn),
		       PCI_SLOT(new_devfn), PCI_FUNC(new_devfn));

		dev->path.pci.devfn = new_devfn;
	}
}

static void root_port_commit_config(struct device *dev)
{
	int i;
	int coalesce = 0;

	if (dev->chip_info != NULL) {
		struct southbridge_intel_i82801gx_config *config = dev->chip_info;
		coalesce = config->pcie_port_coalesce;
	}

	if (!rpc.ports[0]->enabled)
		coalesce = 1;

	for (i = 0; i < rpc.num_ports; i++) {
		struct device *pcie_dev;

		pcie_dev = rpc.ports[i];

		if (pcie_dev == NULL) {
			printk(BIOS_ERR, "Root Port %d device is NULL?\n", i + 1);
			continue;
		}

		if (pcie_dev->enabled)
			continue;

		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(pcie_dev));

		/* Disable this device if possible */
		i82801gx_enable(pcie_dev);
	}

	if (coalesce) {
		int current_func;

		/* For all Root Ports N enabled ports get assigned the lower
		 * PCI function number. The disabled ones get upper PCI
		 * function numbers. */
		current_func = 0;
		for (i = 0; i < rpc.num_ports; i++) {
			if (!rpc.ports[i]->enabled)
				continue;
			ich_pcie_device_set_func(i, current_func);
			current_func++;
		}

		/* Allocate the disabled devices' PCI function number. */
		for (i = 0; i < rpc.num_ports; i++) {
			if (rpc.ports[i]->enabled)
				continue;
			ich_pcie_device_set_func(i, current_func);
			current_func++;
		}
	}

	printk(BIOS_SPEW, "ICH: RPFN 0x%08x -> 0x%08x\n", rpc.orig_rpfn, rpc.new_rpfn);
	RCBA32(RPFN) = rpc.new_rpfn;
}

static void ich_pcie_enable(struct device *dev)
{
	/* Add this device to the root port config structure. */
	root_port_init_config(dev);

	/*
	 * When processing the last PCIe root port we can now
	 * update the Root Port Function Number and Hide register.
	 */
	if (root_port_is_last(dev))
		root_port_commit_config(dev);
}

static struct pci_operations pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.enable			= ich_pcie_enable,
	.scan_bus		= pci_scan_bridge,
	.ops_pci		= &pci_ops,
};

static const unsigned short i82801gx_pcie_ids[] = {
	0x27d0, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27d2, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27d4, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27d6, /* 82801GB/GR/GDH/GBM/GHM (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH) */
	0x27e0, /* 82801GR/GDH/GHM (ICH7R/ICH7DH/ICH7-M DH) */
	0x27e2, /* 82801GR/GDH/GHM (ICH7R/ICH7DH/ICH7-M DH) */
	0
};

static const struct pci_driver i82801gx_pcie __pci_driver = {
	.ops		= &device_ops,
	.vendor		= PCI_VENDOR_ID_INTEL,
	.devices	= i82801gx_pcie_ids,
};
