#ifndef DEVICE_PCIEXP_H
#define DEVICE_PCIEXP_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

enum aspm_type {
	PCIE_ASPM_NONE = 0,
	PCIE_ASPM_L0S  = 1,
	PCIE_ASPM_L1   = 2,
	PCIE_ASPM_BOTH = 3,
};

void pciexp_scan_bus(struct bus *bus, unsigned int min_devfn,
			     unsigned int max_devfn);

void pciexp_scan_bridge(device_t dev);

extern struct device_operations default_pciexp_ops_bus;

unsigned int pciexp_find_extended_cap(device_t dev, unsigned int cap);
#endif /* DEVICE_PCIEXP_H */
