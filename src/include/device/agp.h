#ifndef DEVICE_AGP_H
#define DEVICE_AGP_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

unsigned int agp_scan_bus(struct bus *bus,
	unsigned min_devfn, unsigned max_devfn, unsigned int max);
unsigned int agp_scan_bridge(device_t dev, unsigned int max);

extern struct device_operations default_agp_ops_bus;


#endif /* DEVICE_AGP_H */
