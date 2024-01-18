/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CHIP_COMMON_H_
#define _CHIP_COMMON_H_

#include <hob_iiouds.h>

union xeon_domain_path {
	unsigned int domain_path;
	struct {
		u8 bus;
		u8 stack;
		u8 socket;
		u8 unused;
	};
};

void iio_pci_domain_read_resources(struct device *dev);
void iio_pci_domain_scan_bus(struct device *dev);
void attach_iio_stacks(struct device *dev);

void soc_create_ioat_domains(union xeon_domain_path path, struct bus *bus, const STACK_RES *sr);
struct device *dev_find_device_on_socket(uint8_t socket, u16 vendor, u16 device);
int iio_pci_domain_socket_from_dev(struct device *dev);
int iio_pci_domain_stack_from_dev(struct device *dev);

#endif /* _CHIP_COMMON_H_ */
