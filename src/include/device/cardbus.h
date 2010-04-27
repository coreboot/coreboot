#ifndef DEVICE_CARDBUS_H
#define DEVICE_CARDBUS_H
/* (c) 2005 Linux Networx GPL see COPYING for details */

void cardbus_read_resources(device_t dev);
unsigned int cardbus_scan_bus(struct bus *bus,
	unsigned min_devfn, unsigned max_devfn, unsigned int max);
unsigned int cardbus_scan_bridge(device_t dev, unsigned int max);
void cardbus_enable_resources(device_t dev);

extern struct device_operations default_cardbus_ops_bus;

#endif /* DEVICE_CARDBUS_H */
