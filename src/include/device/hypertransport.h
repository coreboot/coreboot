#ifndef DEVICE_HYPERTRANSPORT_H
#define DEVICE_HYPERTRANSPORT_H

unsigned int hypertransport_scan_chain(struct bus *bus, unsigned min_unitid, unsigned int max);

#define HT_IO_HOST_ALIGN 4096
#define HT_MEM_HOST_ALIGN (1024*1024)

#endif /* DEVICE_HYPERTRANSPORT_H */
