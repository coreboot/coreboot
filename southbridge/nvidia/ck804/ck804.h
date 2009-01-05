#ifndef CK804_H
#define CK804_H

#include <device/device.h>
#include <device/pci.h>

void ck804_enable(struct device *dev);

extern struct pci_operations ck804_ops_pci;

#endif
