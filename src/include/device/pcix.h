#ifndef DEVICE_PCIX_H
#define DEVICE_PCIX_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

unsigned int pcix_scan_bus(struct bus *bus,
	unsigned min_devfn, unsigned max_devfn, unsigned int max);
unsigned int pcix_scan_bridge(device_t dev, unsigned int max);
const char *pcix_speed(unsigned sstatus);

extern struct device_operations default_pcix_ops_bus;

#endif /* DEVICE_PCIX_H */
