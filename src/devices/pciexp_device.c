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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pciexp.h>

static void pciexp_tune_dev(device_t dev)
{
	unsigned int cap;
#if CONFIG_PCIE_TUNING
	u32 reg32;
#endif

	cap = pci_find_capability(dev, PCI_CAP_ID_PCIE);
	if (!cap)
		return;

#if CONFIG_PCIE_TUNING
	printk(BIOS_DEBUG, "PCIe: tuning %s\n", dev_path(dev));

	// TODO make this depending on ASPM.

	/* Enable ASPM role based error reporting. */
	reg32 = pci_read_config32(dev, cap + PCI_EXP_DEVCAP);
	reg32 |= PCI_EXP_DEVCAP_RBER;
	pci_write_config32(dev, cap + PCI_EXP_DEVCAP, reg32);
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
