/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CHIP_COMMON_H_
#define _CHIP_COMMON_H_

void xeonsp_pci_domain_set_resources(struct device *dev);
void xeonsp_pci_domain_scan_bus(struct device *dev);
void attach_iio_stacks(struct device *dev);

#endif /* _CHIP_COMMON_H_ */
