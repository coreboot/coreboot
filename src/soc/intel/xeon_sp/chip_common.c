/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/soc_util.h>
#include <soc/util.h>

/**
 * Find all device of a given vendor and type for the specified socket.
 * The function iterates over all PCI domains of the specified socket
 * and matches the PCI vendor and device ID.
 *
 * @param socket The socket where to search for the device.
 * @param vendor A PCI vendor ID (e.g. 0x8086 for Intel).
 * @param device A PCI device ID.
 * @param from The device pointer to start search from.
 *
 * @return Pointer to the device struct. When there are multiple device
 * instances, the caller should continue search upon a non-NULL match.
 */
struct device *dev_find_all_devices_on_socket(uint8_t socket, u16 vendor, u16 device,
	struct device *from)
{
	return dev_find_all_devices_on_stack(socket, XEONSP_STACK_MAX, vendor, device, from);
}

/*
 * Find device of a given vendor and type for the specified socket.
 * The function will return at the 1st match.
 */
struct device *dev_find_device_on_socket(uint8_t socket, u16 vendor, u16 device)
{
	return dev_find_all_devices_on_socket(socket, vendor, device, NULL);
}

static int filter_device_on_stack(struct device *dev, uint8_t socket, uint8_t stack,
	u16 vendor, u16 device)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return 0;

	const struct device *domain = dev_get_domain(dev);
	if (!domain)
		return 0;

	union xeon_domain_path dn;
	dn.domain_path = dev_get_domain_id(domain);

	if (socket != XEONSP_SOCKET_MAX && dn.socket != socket)
		return 0;
	if (stack != XEONSP_STACK_MAX && dn.stack != stack)
		return 0;
	if (vendor != XEONSP_VENDOR_MAX && dev->vendor != vendor)
		return 0;
	if (device != XEONSP_DEVICE_MAX && dev->device != device)
		return 0;

	return 1;
};

/**
 * Find all device of a given vendor and type for the specified socket and stack.
 *
 * @param socket The socket where to search for the device.
 *              XEONSP_SOCKET_MAX indicates any socket.
 * @param stack The stack where to search for the device.
 *              XEONSP_STACK_MAX indicates any stack.
 * @param vendor A PCI vendor ID (e.g. 0x8086 for Intel).
 *              XEONSP_VENDOR_MAX indicates any vendor.
 * @param device A PCI device ID.
 *              XEONSP_DEVICE_MAX indicates any device.
 * @param from The device pointer to start search from.
 *
 * @return Pointer to the device struct. When there are multiple device
 * instances, the caller should continue search upon a non-NULL match.
 */
struct device *dev_find_all_devices_on_stack(uint8_t socket, uint8_t stack,
	u16 vendor, u16 device, struct device *from)
{
	if (!from)
		from = all_devices;
	else
		from = from->next;

	while (from && (!filter_device_on_stack(from, socket, stack,
		vendor, device)))
		from = from->next;

	return from;
}

/**
 * Find all device of a given vendor and type for the specific domain
 * Only the direct child of the input domain is iterated
 *
 * @param domain Pointer to the input domain
 * @param vendor A PCI vendor ID
 *              XEONSP_VENDOR_MAX indicates any vendor
 * @param vendor A PCI device ID
 *              XEONSP_DEVICE_MAX indicates any vendor
 * @param from The device pointer to start search from.
 *
 * @return Pointer to the device struct. When there are multiple device
 * instances, the caller should continue search upon a non-NULL match.
 */
struct device *dev_find_all_devices_on_domain(struct device *domain, u16 vendor,
	u16 device, struct device *from)
{
	struct device *dev = from;
	while ((dev = dev_bus_each_child(domain->downstream, dev))) {
		if (vendor != XEONSP_VENDOR_MAX && dev->vendor != vendor)
			continue;
		if (device != XEONSP_DEVICE_MAX && dev->device != device)
			continue;
		break;
	}

	return dev;
}

/**
 * Returns the socket ID where the specified device is connected to.
 * This is an integer in the range [0, CONFIG_MAX_SOCKET).
 *
 * @param dev The device to look up
 *
 * @return Socket ID the device is attached to, negative number on error.
 */
int iio_pci_domain_socket_from_dev(const struct device *dev)
{
	const struct device *domain;
	union xeon_domain_path dn;

	domain = dev_get_domain(dev);
	if (!domain)
		return -1;

	dn.domain_path = dev_get_domain_id(domain);

	return dn.socket;
}

/**
 * Returns the stack ID where the specified device is connected to.
 * This is an integer in the range [0, MAX_IIO_STACK).
 *
 * @param dev The device to look up
 *
 * @return Stack ID the device is attached to, negative number on error.
 */
int iio_pci_domain_stack_from_dev(const struct device *dev)
{
	const struct device *domain;
	union xeon_domain_path dn;

	domain = dev_get_domain(dev);
	if (!domain)
		return -1;

	dn.domain_path = dev_get_domain_id(domain);

	return dn.stack;
}

void create_domain(const union xeon_domain_path dp, struct bus *upstream,
			       int bus_base, int bus_limit, const char *type,
			       struct device_operations *ops,
			       const size_t pci_segment_group)
{
	struct device_path path;
	init_xeon_domain_path(&path, dp.socket, dp.stack, bus_base);

	struct device *const domain = alloc_find_dev(upstream, &path);
	if (!domain)
		die("%s: out of memory.\n", __func__);

	domain->ops = ops;
	iio_domain_set_acpi_name(domain, type);

	struct bus *const bus = alloc_bus(domain);
	bus->secondary = bus_base;
	bus->subordinate = bus_base;
	bus->max_subordinate = bus_limit;
	bus->segment_group = pci_segment_group;
}

/* Attach stack as domains */
void attach_iio_stacks(void)
{
	const IIO_UDS *hob = get_iio_uds();
	union xeon_domain_path dn = { .domain_path = 0 };
	if (!hob)
		return;

	struct bus *root_bus = dev_root.downstream;
	for (int s = 0; s < CONFIG_MAX_SOCKET; ++s) {
		if (!soc_cpu_is_enabled(s))
			continue;
		for (int x = 0; x < MAX_LOGIC_IIO_STACK; ++x) {
			const xSTACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			const size_t seg = hob->PlatformData.CpuQpiInfo[s].PcieSegment;

			if (ri->BusBase > ri->BusLimit)
				continue;

			/* Prepare domain path */
			dn.socket = s;
			dn.stack = x;

			create_xeonsp_domains(dn, root_bus, ri, seg);
		}
	}
}

bool is_pcie_domain(const struct device *dev)
{
	if ((!dev) || (dev->path.type != DEVICE_PATH_DOMAIN))
		return false;

	return strstr(dev->name, DOMAIN_TYPE_PCIE);
}

bool is_ioat_domain(const struct device *dev)
{
	if ((!dev) || (dev->path.type != DEVICE_PATH_DOMAIN))
		return false;

	return (strstr(dev->name, DOMAIN_TYPE_CPM0) ||
		strstr(dev->name, DOMAIN_TYPE_CPM1) ||
		strstr(dev->name, DOMAIN_TYPE_DINO) ||
		strstr(dev->name, DOMAIN_TYPE_HQM0) ||
		strstr(dev->name, DOMAIN_TYPE_HQM1));
}

bool is_ubox_domain(const struct device *dev)
{
	if ((!dev) || (dev->path.type != DEVICE_PATH_DOMAIN))
		return false;

	return (strstr(dev->name, DOMAIN_TYPE_UBX0) ||
		strstr(dev->name, DOMAIN_TYPE_UBX1));
}

bool is_cxl_domain(const struct device *dev)
{
	if ((!dev) || (dev->path.type != DEVICE_PATH_DOMAIN))
		return false;

	return strstr(dev->name, DOMAIN_TYPE_CXL);
}
