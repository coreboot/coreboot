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

/*
 * Every STACK can have multiple PCI domains with an unique domain type.
 * This is only of cosmetic nature and generates more readable ACPI code,
 * but isn't technical necessary.
 */
#define DOMAIN_TYPE_CPM0       "PM"
#define DOMAIN_TYPE_CPM1       "PN"
#define DOMAIN_TYPE_DINO       "DI"
#define DOMAIN_TYPE_HQM0       "HQ"
#define DOMAIN_TYPE_HQM1       "HR"
#define DOMAIN_TYPE_PCIE       "PC"
#define DOMAIN_TYPE_UBX0       "UC"
#define DOMAIN_TYPE_UBX1       "UD"

void iio_pci_domain_read_resources(struct device *dev);
void iio_pci_domain_scan_bus(struct device *dev);
void attach_iio_stacks(struct device *dev);

void soc_create_ioat_domains(union xeon_domain_path path, struct bus *bus, const STACK_RES *sr);
struct device *dev_find_device_on_socket(uint8_t socket, u16 vendor, u16 device);
int iio_pci_domain_socket_from_dev(struct device *dev);
int iio_pci_domain_stack_from_dev(struct device *dev);

#endif /* _CHIP_COMMON_H_ */
