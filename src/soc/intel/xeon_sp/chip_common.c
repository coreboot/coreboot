/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <post.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <stdlib.h>

static const STACK_RES *domain_to_stack_res(const struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_DOMAIN);
	const union xeon_domain_path dn = {
		.domain_path = dev->path.domain.domain
	};

	const IIO_UDS *hob = get_iio_uds();
	assert(hob != NULL);

	return &hob->PlatformData.IIO_resource[dn.socket].StackRes[dn.stack];
}

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
	struct device *domain = dev_get_pci_domain(dev);
	if (!domain)
		return 0;
	if (dev->path.type != DEVICE_PATH_PCI)
		return 0;

	union xeon_domain_path dn;
	dn.domain_path = domain->path.domain.domain;

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
int iio_pci_domain_socket_from_dev(struct device *dev)
{
	struct device *domain;
	union xeon_domain_path dn;

	if (dev->path.type == DEVICE_PATH_DOMAIN)
		domain = dev;
	else
		domain = dev_get_pci_domain(dev);

	if (!domain)
		return -1;

	dn.domain_path = domain->path.domain.domain;

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
int iio_pci_domain_stack_from_dev(struct device *dev)
{
	struct device *domain;
	union xeon_domain_path dn;

	if (dev->path.type == DEVICE_PATH_DOMAIN)
		domain = dev;
	else
		domain = dev_get_pci_domain(dev);

	if (!domain)
		return -1;

	dn.domain_path = domain->path.domain.domain;

	return dn.stack;
}

void iio_pci_domain_read_resources(struct device *dev)
{
	struct resource *res;
	const STACK_RES *sr = domain_to_stack_res(dev);

	if (!sr)
		return;

	int index = 0;

	if (dev->path.domain.domain == 0) {
		/* The 0 - 0xfff IO range is not reported by the HOB but still gets decoded */
		res = new_resource(dev, index++);
		res->base = 0;
		res->size = 0x1000;
		res->limit = 0xfff;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
	}

	if (sr->PciResourceIoBase < sr->PciResourceIoLimit) {
		res = new_resource(dev, index++);
		res->base = sr->PciResourceIoBase;
		res->limit = sr->PciResourceIoLimit;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;
	}

	if (sr->PciResourceMem32Base < sr->PciResourceMem32Limit) {
		res = new_resource(dev, index++);
		res->base = sr->PciResourceMem32Base;
		res->limit = sr->PciResourceMem32Limit;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
	}

	if (sr->PciResourceMem64Base < sr->PciResourceMem64Limit) {
		res = new_resource(dev, index++);
		res->base = sr->PciResourceMem64Base;
		res->limit = sr->PciResourceMem64Limit;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
	}
}

/*
 * Used by IIO stacks for PCIe bridges. Those contain 1 PCI host bridges,
 *  all the bus numbers on the IIO stack can be used for this bridge
 */
static struct device_operations iio_pcie_domain_ops = {
	.read_resources = iio_pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = pci_host_bridge_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = soc_acpi_name,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt	   = pci_domain_fill_ssdt,
#endif
};

/*
 * Used by UBOX stacks. Those contain multiple PCI host bridges, each having
 * only one bus with UBOX devices. UBOX devices have no resources.
 */
static struct device_operations ubox_pcie_domain_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = pci_host_bridge_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = soc_acpi_name,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt	   = pci_domain_fill_ssdt,
#endif
};

static void soc_create_domains(const union xeon_domain_path dp, struct bus *upstream,
				int bus_base, int bus_limit, const char *type,
				struct device_operations *ops)
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
}


static void soc_create_pcie_domains(const union xeon_domain_path dp, struct bus *upstream,
				const STACK_RES *sr)
{
	soc_create_domains(dp, upstream, sr->BusBase, sr->BusLimit, DOMAIN_TYPE_PCIE,
				&iio_pcie_domain_ops);
}

/*
 * On the first Xeon-SP generations there are no separate UBOX stacks,
 * and the UBOX devices reside on the first and second IIO. Starting
 * with 3rd gen Xeon-SP the UBOX devices are located on their own IIO.
 */
static void soc_create_ubox_domains(const union xeon_domain_path dp, struct bus *upstream,
				const STACK_RES *sr)
{
	/* Only expect 2 UBOX buses here */
	assert(sr->BusBase + 1 == sr->BusLimit);

	soc_create_domains(dp, upstream, sr->BusBase, sr->BusBase, DOMAIN_TYPE_UBX0,
				&ubox_pcie_domain_ops);
	soc_create_domains(dp, upstream, sr->BusLimit, sr->BusLimit, DOMAIN_TYPE_UBX1,
				&ubox_pcie_domain_ops);
}

#if CONFIG(SOC_INTEL_HAS_CXL)
void iio_cxl_domain_read_resources(struct device *dev)
{
	struct resource *res;
	const STACK_RES *sr = domain_to_stack_res(dev);

	if (!sr)
		return;

	int index = 0;

	if (sr->IoBase < sr->PciResourceIoBase) {
		res = new_resource(dev, index++);
		res->base = sr->IoBase;
		res->limit = sr->PciResourceIoBase - 1;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED;
	}

	if (sr->Mmio32Base < sr->PciResourceMem32Base) {
		res = new_resource(dev, index++);
		res->base = sr->Mmio32Base;
		res->limit = sr->PciResourceMem32Base - 1;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
	}

	if (sr->Mmio64Base < sr->PciResourceMem64Base) {
		res = new_resource(dev, index++);
		res->base = sr->Mmio64Base;
		res->limit = sr->PciResourceMem64Base - 1;
		res->size = res->limit - res->base + 1;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED;
	}
}

static struct device_operations iio_cxl_domain_ops = {
	.read_resources = iio_cxl_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = pci_host_bridge_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = soc_acpi_name,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt	   = pci_domain_fill_ssdt,
#endif
};

void soc_create_cxl_domains(const union xeon_domain_path dp, struct bus *bus,
				const STACK_RES *sr)
{
	assert(sr->BusBase + 1 <= sr->BusLimit);
	/* 1st domain contains PCIe RCiEPs */
	soc_create_domains(dp, bus, sr->BusBase, sr->BusBase, DOMAIN_TYPE_PCIE,
				&iio_pcie_domain_ops);
	/* 2nd domain contains CXL 1.1 end-points */
	soc_create_domains(dp, bus, sr->BusBase + 1, sr->BusLimit, DOMAIN_TYPE_CXL,
				&iio_cxl_domain_ops);
}
#endif //CONFIG(SOC_INTEL_HAS_CXL)

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
			const STACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			if (ri->BusBase > ri->BusLimit)
				continue;

			/* Prepare domain path */
			dn.socket = s;
			dn.stack = x;

			if (is_ubox_stack_res(ri))
				soc_create_ubox_domains(dn, root_bus, ri);
			else if (CONFIG(SOC_INTEL_HAS_CXL) && is_iio_cxl_stack_res(ri))
				soc_create_cxl_domains(dn, root_bus, ri);
			else if (is_pcie_iio_stack_res(ri))
				soc_create_pcie_domains(dn, root_bus, ri);
			else if (CONFIG(HAVE_IOAT_DOMAINS) && is_ioat_iio_stack_res(ri))
				soc_create_ioat_domains(dn, root_bus, ri);
		}
	}
}
