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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/agp.h>

static void agp_tune_dev(device_t dev)
{
	unsigned int cap;

	cap = pci_find_capability(dev, PCI_CAP_ID_AGP);
	if (!cap)
		return;

	/* The OS is responsible for AGP tuning so do nothing here. */
}

unsigned int agp_scan_bus(struct bus *bus, unsigned int min_devfn,
			  unsigned int max_devfn, unsigned int max)
{
	device_t child;

	max = pci_scan_bus(bus, min_devfn, max_devfn, max);

	for (child = bus->children; child; child = child->sibling) {
		if ((child->path.pci.devfn < min_devfn) ||
		    (child->path.pci.devfn > max_devfn)) {
			continue;
		}
		agp_tune_dev(child);
	}

	return max;
}

unsigned int agp_scan_bridge(device_t dev, unsigned int max)
{
	return do_pci_scan_bridge(dev, max, agp_scan_bus);
}

/** Default device operations for AGP bridges. */
static struct pci_operations agp_bus_ops_pci = {
	.set_subsystem = 0,
};

struct device_operations default_agp_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = 0,
	.scan_bus         = agp_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &agp_bus_ops_pci,
};
