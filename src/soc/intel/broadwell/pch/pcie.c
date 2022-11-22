/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/intel/haswell/haswell.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <soc/lpc.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/rcba.h>
#include <soc/intel/broadwell/pch/chip.h>
#include <southbridge/intel/lynxpoint/iobp.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <types.h>

/* Low Power variant has 6 root ports. */
#define MAX_NUM_ROOT_PORTS 6

struct root_port_config {
	/* RPFN is a write-once register so keep a copy until it is written */
	u32 orig_rpfn;
	u32 new_rpfn;
	u32 pin_ownership;
	u32 strpfusecfg1;
	u32 strpfusecfg2;
	u32 strpfusecfg3;
	u32 b0d28f0_32c;
	u32 b0d28f4_32c;
	u32 b0d28f5_32c;
	bool coalesce;
	int gbe_port;
	int num_ports;
	struct device *ports[MAX_NUM_ROOT_PORTS];
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

static void root_port_config_update_gbe_port(void)
{
	/* Is the Gbe Port enabled? */
	if (!((rpc.strpfusecfg1 >> 19) & 1))
		return;

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
}

static void pcie_iosf_port_grant_count(struct device *dev)
{
	u8 update_val;
	u32 rpcd = (pci_read_config32(dev, 0xfc) >> 14) & 0x3;

	switch (rpcd) {
	case 1:
	case 3:
		update_val = 0x02;
		break;
	case 2:
		update_val = 0x22;
		break;
	default:
		update_val = 0x00;
		break;
	}

	RCBA32(0x103C) = (RCBA32(0x103C) & (~0xff)) | update_val;
}

static void root_port_init_config(struct device *dev)
{
	int rp;
	u32 data = 0;
	u8 resp, id;

	if (root_port_is_first(dev)) {
		rpc.orig_rpfn = RCBA32(RPFN);
		rpc.new_rpfn = rpc.orig_rpfn;
		rpc.num_ports = MAX_NUM_ROOT_PORTS;
		rpc.gbe_port = -1;
		/* RP0 f5[3:0] = 0101b*/
		pci_update_config8(dev, 0xf5, ~0xa, 0x5);

		pcie_iosf_port_grant_count(dev);

		rpc.pin_ownership = pci_read_config32(dev, 0x410);
		root_port_config_update_gbe_port();

		pci_or_config8(dev, 0xe2, 3 << 4);
		const struct soc_intel_broadwell_pch_config *config = config_of(dev);
		rpc.coalesce = config->pcie_port_coalesce;
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
		rpc.b0d28f0_32c = pci_read_config32(dev, 0x32c);
		break;
	case 5:
		rpc.strpfusecfg2 = pci_read_config32(dev, 0xfc);
		rpc.b0d28f4_32c = pci_read_config32(dev, 0x32c);
		break;
	case 6:
		rpc.b0d28f5_32c = pci_read_config32(dev, 0x32c);
		rpc.strpfusecfg3 = pci_read_config32(dev, 0xfc);
		break;
	default:
		break;
	}

	pci_write_config32(dev, 0x418, 0x02000430);

	if (root_port_is_first(dev)) {
		/*
		 * set RP0 PCICFG E2h[5:4] = 11b and E1h[6] = 1
		 * before configuring ASPM
		 */
		id = 0xe0 + (u8)(RCBA32(RPFN) & 0x07);
		pch_iobp_exec(0xE00000E0, IOBP_PCICFG_READ, id, &data, &resp);
		data |= ((0x30 << 16) | (0x40 << 8));
		pch_iobp_exec(0xE00000E0, IOBP_PCICFG_WRITE, id, &data, &resp);
	}

	/* Cache pci device. */
	rpc.ports[rp - 1] = dev;
}

/* Update devicetree with new Root Port function number assignment */
static void pch_pcie_device_set_func(int index, int pci_func)
{
	struct device *dev;
	unsigned int new_devfn;

	dev = rpc.ports[index];

	/* Set the new PCI function field for this Root Port. */
	rpc.new_rpfn &= ~RPFN_FNMASK(index);
	rpc.new_rpfn |= RPFN_FNSET(index, pci_func);

	/* Determine the new devfn for this port */
	new_devfn = PCI_DEVFN(PCH_DEV_SLOT_PCIE, pci_func);

	if (dev && dev->path.pci.devfn != new_devfn) {
		printk(BIOS_DEBUG,
		       "PCH: PCIe map %02x.%1x -> %02x.%1x\n",
		       PCI_SLOT(dev->path.pci.devfn),
		       PCI_FUNC(dev->path.pci.devfn),
		       PCI_SLOT(new_devfn), PCI_FUNC(new_devfn));

		dev->path.pci.devfn = new_devfn;
	}
}

static void pcie_enable_clock_gating(void)
{
	int i;
	int enabled_ports = 0;
	int is_broadwell = !!(cpu_family_model() == BROADWELL_FAMILY_ULT);

	for (i = 0; i < rpc.num_ports; i++) {
		struct device *dev;
		int rp;

		dev = rpc.ports[i];
		if (!dev)
			continue;

		rp = root_port_number(dev);

		if (!dev->enabled) {
			/* Configure shared resource clock gating. */
			if (rp == 1 || rp == 5 || rp == 6)
				pci_or_config8(dev, 0xe1, 0x3c);

			pci_or_config8(dev, 0xe2, 3 << 4);
			pci_or_config32(dev, 0x420, 1 << 31);

			/* Per-Port CLKREQ# handling. */
			if (gpio_is_native(18 + rp - 1))
				pci_or_config32(dev, 0x420, 3 << 29);

			/* Enable static clock gating. */
			if (rp == 1 && !rpc.ports[1]->enabled &&
			    !rpc.ports[2]->enabled && !rpc.ports[3]->enabled) {
				pci_or_config8(dev, 0xe2, 1);
				pci_or_config8(dev, 0xe1, 1 << 7);
			} else if (rp == 5 || rp == 6) {
				pci_or_config8(dev, 0xe2, 1);
				pci_or_config8(dev, 0xe1, 1 << 7);
			}
			continue;
		}

		enabled_ports++;

		/* Enable dynamic clock gating. */
		pci_or_config8(dev, 0xe1, 0x03);
		pci_or_config8(dev, 0xe2, 1 << 6);
		pci_update_config8(dev, 0xe8, ~(3 << 2), (2 << 2));

		/* Update PECR1 register. */
		pci_or_config8(dev, 0xe8, 3);

		if (is_broadwell) {
			pci_or_config32(dev, 0x324, (1 << 5) | (1 << 14));
		} else {
			pci_or_config32(dev, 0x324, 1 << 5);
		}
		/* Per-Port CLKREQ# handling. */
		if (gpio_is_native(18 + rp - 1))
			/*
			 * In addition to D28Fx PCICFG 420h[30:29] = 11b,
			 * set 420h[17] = 0b and 420[0] = 1b for L1 SubState.
			 */
			pci_update_config32(dev, 0x420, ~(1 << 17), (3 << 29) | 1);

		/* Configure shared resource clock gating. */
		if (rp == 1 || rp == 5 || rp == 6)
			pci_or_config8(dev, 0xe1, 0x3c);

		/* CLKREQ# VR Idle Enable */
		RCBA32_OR(0x2b1c, (1 << (16 + i)));
	}

	if (!enabled_ports)
		pci_or_config8(rpc.ports[0], 0xe1, 1 << 6);
}

static void root_port_commit_config(void)
{
	int i;

	/* If the first root port is disabled the coalesce ports. */
	if (!rpc.ports[0]->enabled)
		rpc.coalesce = true;

	/* Perform clock gating configuration. */
	pcie_enable_clock_gating();

	for (i = 0; i < rpc.num_ports; i++) {
		struct device *dev;
		u32 reg32;
		int n = 0;

		dev = rpc.ports[i];

		if (dev == NULL) {
			printk(BIOS_ERR, "Root Port %d device is NULL?\n", i+1);
			continue;
		}

		if (dev->enabled)
			continue;

		printk(BIOS_DEBUG, "%s: Disabling device\n", dev_path(dev));

		/* 8.2 Configuration of PCI Express Root Ports */
		pci_or_config32(dev, 0x338, 1 << 26);

		do {
			reg32 = pci_read_config32(dev, 0x328);
			n++;
			if (((reg32 & 0xff000000) == 0x01000000) || (n > 50))
				break;
			udelay(100);
		} while (1);

		if (n > 50)
			printk(BIOS_DEBUG, "%s: Timeout waiting for 328h\n",
				dev_path(dev));

		pci_or_config32(dev, 0x408, 1 << 27);

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

static void root_port_mark_disable(struct device *dev)
{
	/* Mark device as disabled. */
	dev->enabled = 0;
	/* Mark device to be hidden. */
	rpc.new_rpfn |= RPFN_HIDE(PCI_FUNC(dev->path.pci.devfn));
}

static void root_port_check_disable(struct device *dev)
{
	int rp;

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
	}

	/* Check Pin Ownership. */
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
}

static void pcie_add_0x0202000_iobp(u32 reg)
{
	u32 reg32;

	reg32 = pch_iobp_read(reg);
	reg32 += (0x2 << 16) | (0x2 << 8);
	pch_iobp_write(reg, reg32);
}

static void pch_pcie_early(struct device *dev)
{
	const struct soc_intel_broadwell_pch_config *config = config_of(dev);
	int do_aspm = 0;
	int rp = root_port_number(dev);

	switch (rp) {
	case 1:
	case 2:
	case 3:
	case 4:
		/*
		 * Bits 31:28 of b0d28f0 0x32c register correspond to
		 * Root Ports 4:1.
		 */
		do_aspm = !!(rpc.b0d28f0_32c & (1 << (28 + rp - 1)));
		break;
	case 5:
		/*
		 * Bit 28 of b0d28f4 0x32c register correspond to
		 * Root Ports 4:1.
		 */
		do_aspm = !!(rpc.b0d28f4_32c & (1 << 28));
		break;
	case 6:
		/*
		 * Bit 28 of b0d28f5 0x32c register correspond to
		 * Root Ports 4:1.
		 */
		do_aspm = !!(rpc.b0d28f5_32c & (1 << 28));
		break;
	}

	/* Allow ASPM to be forced on in devicetree */
	if ((config->pcie_port_force_aspm & (1 << (rp - 1))))
		do_aspm = 1;

	printk(BIOS_DEBUG, "PCIe Root Port %d ASPM is %sabled\n",
	       rp, do_aspm ? "en" : "dis");

	if (do_aspm) {
		/* Set ASPM bits in MPC2 register. */
		pci_update_config32(dev, 0xd4, ~(0x3 << 2), (1 << 4) | (0x2 << 2));

		/* Set unique clock exit latency in MPC register. */
		pci_update_config32(dev, 0xd8, ~(0x7 << 18), (0x7 << 18));

		switch (rp) {
		case 1:
			pcie_add_0x0202000_iobp(0xe9002440);
			break;
		case 2:
			pcie_add_0x0202000_iobp(0xe9002640);
			break;
		case 3:
			pcie_add_0x0202000_iobp(0xe9000840);
			break;
		case 4:
			pcie_add_0x0202000_iobp(0xe9000a40);
			break;
		case 5:
			pcie_add_0x0202000_iobp(0xe9000c40);
			pcie_add_0x0202000_iobp(0xe9000e40);
			pcie_add_0x0202000_iobp(0xe9001040);
			pcie_add_0x0202000_iobp(0xe9001240);
			break;
		case 6:
			/* Update IOBP based on lane ownership. */
			if (rpc.pin_ownership & (1 << 4))
				pcie_add_0x0202000_iobp(0xea002040);
			if (rpc.pin_ownership & (1 << 5))
				pcie_add_0x0202000_iobp(0xea002240);
			if (rpc.pin_ownership & (1 << 6))
				pcie_add_0x0202000_iobp(0xea002440);
			if (rpc.pin_ownership & (1 << 7))
				pcie_add_0x0202000_iobp(0xea002640);
			break;
		}

		pci_update_config32(dev, 0x338, ~(1 << 26), 0);
	}

	/* Enable LTR in Root Port. Disable OBFF. */
	pci_update_config32(dev, 0x64, ~(3 << 18), (1 << 11));
	pci_or_config32(dev, 0x68, 1 << 10);

	pci_update_config32(dev, 0x318, ~(0xffff << 16), (0x1414 << 16));

	/* Set L1 exit latency in LCAP register. */
	if (!do_aspm && (pci_read_config8(dev, 0xf5) & 0x1))
		pci_update_config32(dev, 0x4c, ~(0x7 << 15), (0x4 << 15));
	else
		pci_update_config32(dev, 0x4c, ~(0x7 << 15), (0x2 << 15));

	pci_update_config32(dev, 0x314, 0, 0x743a361b);

	/* Set Common Clock Exit Latency in MPC register. */
	pci_update_config32(dev, 0xd8, ~(0x7 << 15), (0x3 << 15));

	pci_update_config32(dev, 0x33c, ~0x00ffffff, 0x854d74);

	/* Set Invalid Receive Range Check Enable in MPC register. */
	pci_or_config32(dev, 0xd8, 1 << 25);

	pci_and_config8(dev, 0xf5, 0x0f);

	/* Set AER Extended Cap ID to 01h and Next Cap Pointer to 200h. */
	if (CONFIG(PCIEXP_AER))
		pci_update_config32(dev, 0x100, ~0xfffff, (1 << 29) | 0x10001);
	else
		pci_update_config32(dev, 0x100, ~0xfffff, (1 << 29));

	/* Set L1 Sub-State Cap ID to 1Eh and Next Cap Pointer to None. */
	if (CONFIG(PCIEXP_L1_SUB_STATE))
		pci_update_config32(dev, 0x200, ~0xfffff, 0x001e);
	else
		pci_update_config32(dev, 0x200, ~0xfffff, 0);

	pci_update_config32(dev, 0x320, ~(3 << 20) & ~(7 << 6), (1 << 20) | (3 << 6));

	/* Enable Relaxed Order from Root Port. */
	pci_or_config32(dev, 0x320, 3 << 23);

	if (rp == 1 || rp == 5 || rp == 6)
		pci_update_config8(dev, 0xf7, ~0xc, 0);

	/* Set EOI forwarding disable. */
	pci_update_config32(dev, 0xd4, ~0, (1 << 1));

	/* Read and write back write-once capability registers. */
	pci_update_config32(dev, 0x34, ~0, 0);
	pci_update_config32(dev, 0x40, ~0, 0);
	pci_update_config32(dev, 0x80, ~0, 0);
	pci_update_config32(dev, 0x90, ~0, 0);
}

static void pch_pcie_init(struct device *dev)
{
	printk(BIOS_DEBUG, "Initializing PCH PCIe bridge.\n");

	/* Enable SERR */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_SERR);

	/* Enable Bus Master */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* Set Cache Line Size to 0x10 */
	pci_write_config8(dev, 0x0c, 0x10);

	pci_and_config16(dev, PCI_BRIDGE_CONTROL, ~PCI_BRIDGE_CTL_PARITY);

	/* Clear errors in status registers */
	pci_update_config16(dev, 0x06, ~0, 0);
	pci_update_config16(dev, 0x1e, ~0, 0);
}

static void pch_pcie_enable(struct device *dev)
{
	/* Add this device to the root port config structure. */
	root_port_init_config(dev);

	/* Check to see if this Root Port should be disabled. */
	root_port_check_disable(dev);

	/* Power Management init before enumeration */
	if (dev->enabled)
		pch_pcie_early(dev);

	/*
	 * When processing the last PCIe root port we can now
	 * update the Root Port Function Number and Hide register.
	 */
	if (root_port_is_last(dev))
		root_port_commit_config();
}

static void pcie_get_ltr_max_latencies(u16 *max_snoop, u16 *max_nosnoop)
{
	*max_snoop = PCIE_LTR_MAX_SNOOP_LATENCY_3146US;
	*max_nosnoop = PCIE_LTR_MAX_NO_SNOOP_LATENCY_3146US;
}

static struct pci_operations pcie_ops = {
	.set_subsystem = pci_dev_set_subsystem,
	.get_ltr_max_latencies = pcie_get_ltr_max_latencies,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pch_pcie_init,
	.enable			= pch_pcie_enable,
	.scan_bus		= pciexp_scan_bridge,
	.ops_pci		= &pcie_ops,
};

static const unsigned short pcie_device_ids[] = {
	/* Lynxpoint-LP */
	0x9c10, 0x9c12, 0x9c14, 0x9c16, 0x9c18, 0x9c1a,
	/* WildcatPoint */
	0x9c90, 0x9c92, 0x9c94, 0x9c96, 0x9c98, 0x9c9a, 0x2448,
	0
};

static const struct pci_driver pch_pcie __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pcie_device_ids,
};
