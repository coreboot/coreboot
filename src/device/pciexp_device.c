/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
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
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pciexp.h>

#if CONFIG_PCIEXP_COMMON_CLOCK
/*
 * Re-train a PCIe link
 */
#define PCIE_TRAIN_RETRY 10000
static int pciexp_retrain_link(device_t dev, unsigned cap)
{
	unsigned try = PCIE_TRAIN_RETRY;
	u16 lnk;

	/* Start link retraining */
	lnk = pci_read_config16(dev, cap + PCI_EXP_LNKCTL);
	lnk |= PCI_EXP_LNKCTL_RL;
	pci_write_config16(dev, cap + PCI_EXP_LNKCTL, lnk);

	/* Wait for training to complete */
	while (try--) {
		lnk = pci_read_config16(dev, cap + PCI_EXP_LNKSTA);
		if (!(lnk & PCI_EXP_LNKSTA_LT))
			return 0;
		udelay(100);
	}

	printk(BIOS_ERR, "%s: Link Retrain timeout\n", dev_path(dev));
	return -1;
}

/*
 * Check the Slot Clock Configuration for root port and endpoint
 * and enable Common Clock Configuration if possible.  If CCC is
 * enabled the link must be retrained.
 */
static void pciexp_enable_common_clock(device_t root, unsigned root_cap,
				       device_t endp, unsigned endp_cap)
{
	u16 root_scc, endp_scc, lnkctl;

	/* Get Slot Clock Configuration for root port */
	root_scc = pci_read_config16(root, root_cap + PCI_EXP_LNKSTA);
	root_scc &= PCI_EXP_LNKSTA_SLC;

	/* Get Slot Clock Configuration for endpoint */
	endp_scc = pci_read_config16(endp, endp_cap + PCI_EXP_LNKSTA);
	endp_scc &= PCI_EXP_LNKSTA_SLC;

	/* Enable Common Clock Configuration and retrain */
	if (root_scc && endp_scc) {
		printk(BIOS_INFO, "Enabling Common Clock Configuration\n");

		/* Set in endpoint */
		lnkctl = pci_read_config16(endp, endp_cap + PCI_EXP_LNKCTL);
		lnkctl |= PCI_EXP_LNKCTL_CCC;
		pci_write_config16(endp, endp_cap + PCI_EXP_LNKCTL, lnkctl);

		/* Set in root port */
		lnkctl = pci_read_config16(root, root_cap + PCI_EXP_LNKCTL);
		lnkctl |= PCI_EXP_LNKCTL_CCC;
		pci_write_config16(root, root_cap + PCI_EXP_LNKCTL, lnkctl);

		/* Retrain link if CCC was enabled */
		pciexp_retrain_link(root, root_cap);
	}
}
#endif /* CONFIG_PCIEXP_COMMON_CLOCK */

#if CONFIG_PCIEXP_ASPM
/*
 * Determine the ASPM L0s or L1 exit latency for a link
 * by checking both root port and endpoint and returning
 * the highest latency value.
 */
static int pciexp_aspm_latency(device_t root, unsigned root_cap,
			       device_t endp, unsigned endp_cap,
			       enum aspm_type type)
{
	int root_lat = 0, endp_lat = 0;
	u32 root_lnkcap, endp_lnkcap;

	root_lnkcap = pci_read_config32(root, root_cap + PCI_EXP_LNKCAP);
	endp_lnkcap = pci_read_config32(endp, endp_cap + PCI_EXP_LNKCAP);

	/* Make sure the link supports this ASPM type by checking
	 * capability bits 11:10 with aspm_type offset by 1 */
	if (!(root_lnkcap & (1 << (type + 9))) ||
	    !(endp_lnkcap & (1 << (type + 9))))
		return -1;

	/* Find the one with higher latency */
	switch (type) {
	case PCIE_ASPM_L0S:
		root_lat = (root_lnkcap & PCI_EXP_LNKCAP_L0SEL) >> 12;
		endp_lat = (endp_lnkcap & PCI_EXP_LNKCAP_L0SEL) >> 12;
		break;
	case PCIE_ASPM_L1:
		root_lat = (root_lnkcap & PCI_EXP_LNKCAP_L1EL) >> 15;
		endp_lat = (endp_lnkcap & PCI_EXP_LNKCAP_L1EL) >> 15;
		break;
	default:
		return -1;
	}

	return (endp_lat > root_lat) ? endp_lat : root_lat;
}

/*
 * Enable ASPM on PCIe root port and endpoint.
 *
 * Returns APMC value:
 *   -1 = Error
 *    0 = no ASPM
 *    1 = L0s Enabled
 *    2 = L1 Enabled
 *    3 = L0s and L1 Enabled
 */
static enum aspm_type pciexp_enable_aspm(device_t root, unsigned root_cap,
					 device_t endp, unsigned endp_cap)
{
	const char *aspm_type_str[] = { "None", "L0s", "L1", "L0s and L1" };
	enum aspm_type apmc = PCIE_ASPM_NONE;
	int exit_latency, ok_latency;
	u16 lnkctl;
	u32 devcap;

	/* Get endpoint device capabilities for acceptable limits */
	devcap = pci_read_config32(endp, endp_cap + PCI_EXP_DEVCAP);

	/* Enable L0s if it is within endpoint acceptable limit */
	ok_latency = (devcap & PCI_EXP_DEVCAP_L0S) >> 6;
	exit_latency = pciexp_aspm_latency(root, root_cap, endp, endp_cap,
					   PCIE_ASPM_L0S);
	if (exit_latency >= 0 && exit_latency <= ok_latency)
		apmc |= PCIE_ASPM_L0S;

	/* Enable L1 if it is within endpoint acceptable limit */
	ok_latency = (devcap & PCI_EXP_DEVCAP_L1) >> 9;
	exit_latency = pciexp_aspm_latency(root, root_cap, endp, endp_cap,
					   PCIE_ASPM_L1);
	if (exit_latency >= 0 && exit_latency <= ok_latency)
		apmc |= PCIE_ASPM_L1;

	if (apmc != PCIE_ASPM_NONE) {
		/* Set APMC in root port first */
		lnkctl = pci_read_config16(root, root_cap + PCI_EXP_LNKCTL);
		lnkctl |= apmc;
		pci_write_config16(root, root_cap + PCI_EXP_LNKCTL, lnkctl);

		/* Set APMC in endpoint device next */
		lnkctl = pci_read_config16(endp, endp_cap + PCI_EXP_LNKCTL);
		lnkctl |= apmc;
		pci_write_config16(endp, endp_cap + PCI_EXP_LNKCTL, lnkctl);
	}

	printk(BIOS_INFO, "ASPM: Enabled %s\n", aspm_type_str[apmc]);
	return apmc;
}
#endif /* CONFIG_PCIEXP_ASPM */

static void pciexp_tune_dev(device_t dev)
{
	device_t root = dev->bus->dev;
	unsigned int root_cap, cap;

	cap = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!cap)
		return;

	root_cap = pci_find_capability(root, PCI_CAP_ID_PCIE);
	if (!root_cap)
		return;

#if CONFIG_PCIEXP_COMMON_CLOCK
	/* Check for and enable Common Clock */
	pciexp_enable_common_clock(root, root_cap, dev, cap);
#endif

#if CONFIG_PCIEXP_ASPM
	/* Check for and enable ASPM */
	enum aspm_type apmc = pciexp_enable_aspm(root, root_cap, dev, cap);

	if (apmc != PCIE_ASPM_NONE) {
		/* Enable ASPM role based error reporting. */
		u32 reg32 = pci_read_config32(dev, cap + PCI_EXP_DEVCAP);
		reg32 |= PCI_EXP_DEVCAP_RBER;
		pci_write_config32(dev, cap + PCI_EXP_DEVCAP, reg32);
	}
#endif
}

unsigned int pciexp_scan_bus(struct bus *bus, unsigned int min_devfn,
			     unsigned int max_devfn, unsigned int max)
{
	device_t child;

	max = pci_scan_bus(bus, min_devfn, max_devfn, max);

	for (child = bus->children; child; child = child->sibling) {
		if ((child->path.pci.devfn < min_devfn) ||
		    (child->path.pci.devfn > max_devfn)) {
			continue;
		}
		pciexp_tune_dev(child);
	}
	return max;
}

unsigned int pciexp_scan_bridge(device_t dev, unsigned int max)
{
	return do_pci_scan_bridge(dev, max, pciexp_scan_bus);
}

/** Default device operations for PCI Express bridges */
static struct pci_operations pciexp_bus_ops_pci = {
	.set_subsystem = 0,
};

struct device_operations default_pciexp_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = pciexp_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &pciexp_bus_ops_pci,
};
