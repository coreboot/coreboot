/* (c) 2005 Linux Networx GPL see COPYING for details */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/agp.h>

static void agp_tune_dev(device_t dev)
{
	unsigned cap;
	cap = pci_find_capability(dev, PCI_CAP_ID_AGP);
	if (!cap) {
		return;
	}
	/* The OS is responsible for AGP tuning so do nothing here */
}

unsigned int agp_scan_bus(struct bus *bus,
	unsigned min_devfn, unsigned max_devfn, unsigned int max)
{
	device_t child;
	max = pci_scan_bus(bus, min_devfn, max_devfn, max);
	for(child = bus->children; child; child = child->sibling) {
		if (	(child->path.u.pci.devfn < min_devfn) ||
			(child->path.u.pci.devfn > max_devfn))
		{
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

/** Default device operations for AGP bridges */
static struct pci_operations agp_bus_ops_pci = {
	.set_subsystem = 0,
};

struct device_operations default_agp_ops_bus = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init		  = 0,
	.scan_bus	  = agp_scan_bridge,
	.enable           = 0,
	.reset_bus        = pci_bus_reset,
	.ops_pci          = &agp_bus_ops_pci,
};
