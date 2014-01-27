/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
#include <reg_script.h>

#include <baytrail/pci_devs.h>
#include <baytrail/pcie.h>
#include <baytrail/ramstage.h>

#include "chip.h"

static int pll_en_off;
static uint32_t strpfusecfg;

static inline int root_port_offset(device_t dev)
{
	return PCI_FUNC(dev->path.pci.devfn);
}

static inline int is_first_port(device_t dev)
{
	return root_port_offset(dev) == PCIE_PORT1_FUNC;
}

static const struct reg_script init_static_before_exit_latency[] = {
	/* Disable optimized buffer flush fill and latency tolerant reporting */
	REG_PCI_RMW32(DCAP2, ~(OBFFS | LTRMS), 0),
	REG_PCI_RMW32(DSTS2, ~(OBFFEN| LTRME), 0),
	/* Set maximum payload size. */
	REG_PCI_RMW32(DCAP, ~MPS_MASK, 0),
	/* Disable transmit datapath flush timer, clear transmit config change
	 * wait time, clear sideband interface idle counter. */
	REG_PCI_RMW32(PHYCTL2_IOSFBCTL, ~(TDFT | TXCFGCHWAIT | SIID), 0),
	REG_SCRIPT_END,
};

static const struct reg_script init_static_after_exit_latency[] = {
	/* Set common clock configuration. */
	REG_PCI_OR16(LCTL, CCC),
	/* Set NFTS to 0x743a361b */
	REG_PCI_WRITE32(NFTS, 0x743a361b),
	/* Set common clock latency to 0x3 */
	REG_PCI_RMW32(MPC, ~CCEL_MASK, (0x3 << CCEL_SHIFT)),
	/* Set relay timer policy. */
	REG_PCI_RMW32(RTP, 0xff000000, 0x854c74),
	/* Set IOSF packet fast transmit mode and link speed training policy. */
	REG_PCI_OR16(MPC2, IPF | LSTP),
	/* Channel configuration - enable upstream posted split, set non-posted
	 * and posted request size */
	REG_PCI_RMW32(CHCFG, ~UPSD, UNRS | UPRS),
	/* Completion status replay enable and set TLP grant count */
	REG_PCI_RMW32(CFG2, ~(LATGC_MASK), CSREN | (3 << LATGC_SHIFT)),
	/* Assume no IOAPIC behind root port -- disable EOI forwarding. */
	REG_PCI_OR16(MPC2, EOIFD),
	/* Expose AER */
	REG_PCI_RMW32(AERCH, ~0, (1 << 16) | (1 << 0)),
	/* set completion timeout to 160ms to 170ms */
	REG_PCI_RMW16(DSTS2, ~CTD, 0x6),
	/* Enable AER */
	REG_PCI_OR16(DCTL_DSTS, URE | FEE | NFE | CEE),
	/* Read and write back capabaility registers. */
	REG_PCI_OR32(0x34, 0),
	REG_PCI_OR32(0x80, 0),
	/* Retrain the link. */
	REG_PCI_OR16(LCTL, RL),
	REG_SCRIPT_END,
};

static void byt_pcie_init(device_t dev)
{
	struct reg_script init_script[] = {
		REG_SCRIPT_SET_DEV(dev),
		REG_SCRIPT_NEXT(init_static_before_exit_latency),
		/* Exit latency configuration based on
		 * PHYCTL2_IOSFBCTL[PLL_OFF_EN] set in root port 1*/
		REG_PCI_RMW32(LCAP, ~L1EXIT_MASK,
			2 << (L1EXIT_MASK + pll_en_off)),
		REG_SCRIPT_NEXT(init_static_after_exit_latency),
		/* Disable hot plug, set power to 10W, set slot number. */
		REG_PCI_RMW32(SLCAP, ~(HPC | HPS),
			(1 << SLS_SHIFT) | (100 << SLV_SHIFT) |
			(root_port_offset(dev) << SLN_SHIFT)),
		/* Dynamic clock gating. */
		REG_PCI_OR32(RPPGEN, RPDLCGEN | RPDBCGEN | RPSCGEN),
		REG_PCI_OR32(PWRCTL, RPL1SQPOL | RPDTSQPOL),
		REG_PCI_OR32(PCIEDBG, SPCE),
		REG_SCRIPT_END,
	};

	reg_script_run(init_script);

	if (is_first_port(dev)) {
		struct soc_intel_baytrail_config *config = dev->chip_info;
		uint32_t reg = pci_read_config32(dev, RPPGEN);
		reg |= SRDLCGEN | SRDBCGEN;

		if (config && config->clkreq_enable)
			reg |= LCLKREQEN | BBCLKREQEN;

		pci_write_config32(dev, RPPGEN, reg);
	}
}

static const struct reg_script no_dev_behind_port[] = {
	REG_PCI_OR32(PCIEALC, (1 << 26)),
	REG_PCI_POLL32(PCIESTS1, 0x1f000000, (1 << 24), 50000),
	REG_PCI_OR32(PHYCTL4, SQDIS),
	REG_SCRIPT_END,
};

static void check_port_enabled(device_t dev)
{
	int rp_config = (strpfusecfg & LANECFG_MASK) >> LANECFG_SHIFT;

	switch (root_port_offset(dev)) {
	case PCIE_PORT1_FUNC:
		/* Port 1 cannot be disabled from strapping config. */
		break;
	case PCIE_PORT2_FUNC:
		/* Port 2 disabled in all configs but 4x1. */
		if (rp_config != 0x0)
			dev->enabled = 0;
		break;
	case PCIE_PORT3_FUNC:
		/* Port 3 disabled only in 1x4 config. */
		if (rp_config == 0x3)
			dev->enabled = 0;
		break;
	case PCIE_PORT4_FUNC:
		/* Port 4 disabled in 1x4 and 2x2 config. */
		if (rp_config >= 0x2)
			dev->enabled = 0;
		break;
	}
}

static void check_device_present(device_t dev)
{
	struct reg_script no_dev[] = {
		REG_SCRIPT_SET_DEV(dev),
		REG_SCRIPT_NEXT(no_dev_behind_port),
		REG_SCRIPT_END,
	};

	/* Set slot implemented. */
	pci_write_config32(dev, XCAP, pci_read_config32(dev, XCAP) | SI);

	/* No device present. */
	if (!(pci_read_config32(dev, SLCTL_SLSTS) & PDS)) {
		printk(BIOS_DEBUG, "No PCIe device present.\n");
		reg_script_run(no_dev);
		dev->enabled = 0;
	} else if(!dev->enabled) {
		/* Port is disabled, but device present. Disable link. */
		pci_write_config32(dev, LCTL,
			pci_read_config32(dev, LCTL) | LD);
	}
}

static void byt_pcie_enable(device_t dev)
{
	if (is_first_port(dev)) {
		uint32_t reg = pci_read_config32(dev, PHYCTL2_IOSFBCTL);
		pll_en_off = !!(reg & PLL_OFF_EN);

		strpfusecfg = pci_read_config32(dev, STRPFUSECFG);
	}

	/* Check if device is enabled in strapping. */
	check_port_enabled(dev);
	/* Determine if device is behind port. */
	check_device_present(dev);

	southcluster_enable_dev(dev);
}

static void pcie_root_set_subsystem(device_t dev, unsigned vid, unsigned did)
{
	uint32_t didvid = ((did & 0xffff) << 16) | (vid & 0xffff);

	if (!didvid)
		didvid = pci_read_config32(dev, PCI_VENDOR_ID);
	pci_write_config32(dev, 0x94, didvid);
}

static struct pci_operations pcie_root_ops = {
	.set_subsystem = &pcie_root_set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= byt_pcie_init,
	.scan_bus		= pciexp_scan_bridge,
	.enable			= byt_pcie_enable,
	.ops_pci		= &pcie_root_ops,
};

static const unsigned short pci_device_ids[] = {
	PCIE_PORT1_DEVID, PCIE_PORT2_DEVID, PCIE_PORT3_DEVID, PCIE_PORT4_DEVID,
	0
};

static const struct pci_driver pcie_root_ports __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
