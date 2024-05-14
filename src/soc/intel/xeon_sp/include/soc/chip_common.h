/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CHIP_COMMON_H_
#define _CHIP_COMMON_H_

#include <device/device.h>
#include <soc/soc_util.h>

union xeon_domain_path {
	unsigned int domain_path;
	struct {
		u8 bus;
		u8 stack;
		u8 socket;
		u8 unused;
	};
};

#define XEONSP_STACK_MAX	UINT8_MAX
#define XEONSP_SOCKET_MAX	UINT8_MAX
#define XEONSP_DEVICE_MAX	UINT16_MAX
#define XEONSP_VENDOR_MAX	UINT16_MAX

static inline void init_xeon_domain_path(struct device_path *path, int socket,
	int stack, int bus)
{
	union xeon_domain_path dp = {
		.socket = socket,
		.stack = stack,
		.bus = bus,
	};
	path->type = DEVICE_PATH_DOMAIN;
	path->domain.domain = dp.domain_path;
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
#define DOMAIN_TYPE_CXL        "CX"

void attach_iio_stacks(void);

void create_ioat_domains(union xeon_domain_path path,
				struct bus *bus,
				const xSTACK_RES *sr,
				const size_t pci_segment_group);

void create_xeonsp_domains(const union xeon_domain_path dp, struct bus *bus,
				const xSTACK_RES *sr, const size_t pci_segment_group);

void create_domain(const union xeon_domain_path dp, struct bus *upstream,
			int bus_base, int bus_limit, const char *type,
			struct device_operations *ops, const size_t pci_segment_group);

struct device *dev_find_device_on_socket(uint8_t socket, u16 vendor, u16 device);
struct device *dev_find_all_devices_on_socket(uint8_t socket,
						u16 vendor, u16 device, struct device *from);
struct device *dev_find_all_devices_on_stack(uint8_t socket, uint8_t stack,
						u16 vendor, u16 device, struct device *from);
struct device *dev_find_all_devices_on_domain(struct device *domain,
						u16 vendor, u16 device, struct device *from);

int iio_pci_domain_socket_from_dev(const struct device *dev);
int iio_pci_domain_stack_from_dev(const struct device *dev);

bool is_pcie_domain(const struct device *dev);
bool is_ioat_domain(const struct device *dev);
bool is_ubox_domain(const struct device *dev);
bool is_cxl_domain(const struct device *dev);

#define is_dev_on_pcie_domain(dev) is_pcie_domain(dev_get_domain(dev))
#define is_dev_on_ioat_domain(dev) is_ioat_domain(dev_get_domain(dev))
#define is_dev_on_ubox_domain(dev) is_ubox_domain(dev_get_domain(dev))
#define is_dev_on_cxl_domain(dev) is_cxl_domain(dev_get_domain(dev))

#define is_domain0(dev) (dev && dev->path.type == DEVICE_PATH_DOMAIN &&\
		dev->path.domain.domain == 0)
#define is_dev_on_domain0(dev) (is_domain0(dev_get_domain(dev)))
#define is_stack0(socket, stack) (socket == 0 && stack == IioStack0)

void unlock_pam_regions(void);

size_t vtd_probe_bar_size(struct device *dev);

#endif /* _CHIP_COMMON_H_ */
