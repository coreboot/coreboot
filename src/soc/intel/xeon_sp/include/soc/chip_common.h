/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CHIP_COMMON_H_
#define _CHIP_COMMON_H_

#include <hob_iiouds.h>

void iio_pci_domain_read_resources(struct device *dev);
void iio_pci_domain_scan_bus(struct device *dev);
void attach_iio_stacks(struct device *dev);

void soc_create_ioat_domains(struct bus *bus, const STACK_RES *sr);

#endif /* _CHIP_COMMON_H_ */
