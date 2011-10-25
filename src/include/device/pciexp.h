#ifndef DEVICE_PCIEXP_H
#define DEVICE_PCIEXP_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

enum aspm_type {
	PCIE_ASPM_NONE = 0,
	PCIE_ASPM_L0S  = 1,
	PCIE_ASPM_L1   = 2,
	PCIE_ASPM_BOTH = 3,
};

unsigned int pciexp_scan_bus(struct bus *bus, unsigned int min_devfn,
			     unsigned int max_devfn, unsigned int max);
unsigned int pciexp_scan_bridge(device_t dev, unsigned int max);

extern struct device_operations default_pciexp_ops_bus;

#endif /* DEVICE_PCIEXP_H */
