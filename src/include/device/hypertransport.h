#ifndef DEVICE_HYPERTRANSPORT_H
#define DEVICE_HYPERTRANSPORT_H

#include <device/hypertransport_def.h>

unsigned int hypertransport_scan_chain(struct bus *bus,
	unsigned min_devfn, unsigned max_devfn, unsigned int max, unsigned *ht_unit_base, unsigned offset_unitid);
unsigned int ht_scan_bridge(struct device *dev, unsigned int max);
extern struct device_operations default_ht_ops_bus;

#define HT_IO_HOST_ALIGN 4096
#define HT_MEM_HOST_ALIGN (1024*1024)

#endif /* DEVICE_HYPERTRANSPORT_H */
