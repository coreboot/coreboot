/* SPDX-License-Identifier: GPL-2.0-only */

#include "chip.h"
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <reg_script.h>
#include <soc/pci_devs.h>
#include <soc/pcie.h>
#include <soc/ramstage.h>
#include <soc/smm.h>

static int pll_en_off;
static uint32_t strpfusecfg;

static inline int root_port_offset(struct device *dev)
{
	return PCI_FUNC(dev->path.pci.devfn);
}

static inline int is_first_port(struct device *dev)
{
	return root_port_offset(dev) == PCIE_PORT1_FUNC;
}

static void pcie_init(struct device *dev)
{
}

static const struct reg_script no_dev_behind_port[] = {
	REG_PCI_OR32(PCIEALC, (1 << 26)),
	REG_PCI_POLL32(PCIESTS1, 0x1f000000, (1 << 24), 50000),
	REG_PCI_OR32(PHYCTL4, SQDIS),
	REG_SCRIPT_END,
};

static void check_port_enabled(struct device *dev)
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

static void check_device_present(struct device *dev)
{
	/* port1_dev will store the dev struct pointer of the PORT1 */
	static struct device *port1_dev;

	/*
	 * The SOC has 4 ROOT ports defined with MAX_ROOT_PORTS_BSW. For each port initial
	 * assumption is that, each port will have devices connected to it. Later we will
	 * scan each PORT and if the device is not attached to that port we will update
	 * rootports_in_use. If none of the root port is in use we will disable PORT1
	 * otherwise we will keep PORT1 enabled per spec. In future if the SoC has more
	 * number of PCIe Root ports then change MAX_ROOT_PORTS_BSW value accordingly.
	 */

	static uint32_t rootports_in_use = MAX_ROOT_PORTS_BSW;

	/* Set slot implemented. */
	pci_write_config32(dev, XCAP, pci_read_config32(dev, XCAP) | SI);

	/* No device present. */
	if (!(pci_read_config32(dev, SLCTL_SLSTS) & PDS)) {
		rootports_in_use--;
		printk(BIOS_DEBUG, "No PCIe device present.");

		/*
		 * Defer PORT1 disabling for now. When we are at Last port we will check
		 * rootports_in_use and disable PORT1 if none of the ports have any device
		 * connected to it.
		 */
		if (!is_first_port(dev)) {
			reg_script_run_on_dev(dev, no_dev_behind_port);
			dev->enabled = 0;
		} else
			port1_dev = dev;
		/*
		 * If none of the ROOT PORT has devices connected then disable PORT1.
		 * Else, keep the PORT1 enabled.
		 */
		if (!rootports_in_use) {
			reg_script_run_on_dev(port1_dev, no_dev_behind_port);
			port1_dev->enabled = 0;
			southcluster_enable_dev(port1_dev);
		}
	} else if (!dev->enabled) {
		/* Port is disabled, but device present. Disable link. */
		pci_write_config32(dev, LCTL,
			pci_read_config32(dev, LCTL) | LD);
	}
}

static void pcie_enable(struct device *dev)
{
	if (is_first_port(dev)) {
		struct soc_intel_braswell_config *config = config_of(dev);
		uint32_t reg = pci_read_config32(dev, PHYCTL2_IOSFBCTL);
		pll_en_off = !!(reg & PLL_OFF_EN);

		strpfusecfg = pci_read_config32(dev, STRPFUSECFG);

		if (config->pcie_wake_enable)
			smm_southcluster_save_param(SMM_SAVE_PARAM_PCIE_WAKE_ENABLE, 1);
	}

	/* Check if device is enabled in strapping. */
	check_port_enabled(dev);
	/* Determine if device is behind port. */
	check_device_present(dev);

	southcluster_enable_dev(dev);
}

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pcie_init,
	.scan_bus		= pciexp_scan_bridge,
	.enable			= pcie_enable,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCIE_PORT1_DEVID, PCIE_PORT2_DEVID, PCIE_PORT3_DEVID, PCIE_PORT4_DEVID,
	0
};

static const struct pci_driver pcie_root_ports __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
