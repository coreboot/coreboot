#ifndef DEVICE_HYPERTRANSPORT_H
#define DEVICE_HYPERTRANSPORT_H

#include <device/hypertransport_def.h>

unsigned int hypertransport_scan_chain(struct bus *bus, unsigned int max);

#define HT_IO_HOST_ALIGN 4096
#define HT_MEM_HOST_ALIGN (1024*1024)

#endif /* DEVICE_HYPERTRANSPORT_H */
