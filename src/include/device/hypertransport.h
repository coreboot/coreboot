#ifndef DEVICE_HYPERTRANSPORT_H
#define DEVICE_HYPERTRANSPORT_H

#include <device/hypertransport_def.h>

/* TODO: Check HT specs for better names for these. */
#define LinkConnected		(1 << 0)
#define InitComplete		(1 << 1)
#define NonCoherent			(1 << 2)
#define ConnectionPending	(1 << 4)
bool ht_is_non_coherent_link(struct bus *link);

unsigned int hypertransport_scan_chain(struct bus *bus);
void ht_scan_bridge(struct device *dev);

extern struct device_operations default_ht_ops_bus;

#define HT_IO_HOST_ALIGN 4096
#define HT_MEM_HOST_ALIGN (1024*1024)

#endif /* DEVICE_HYPERTRANSPORT_H */
