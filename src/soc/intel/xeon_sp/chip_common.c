/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <post.h>
#include <device/pci.h>
#include <soc/chip_common.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <stdlib.h>

static const STACK_RES *domain_to_stack_res(const struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_DOMAIN);
	const unsigned int dn = dev->path.domain.domain;

	const IIO_UDS *hob = get_iio_uds();
	assert(hob != NULL);

	return &hob->PlatformData.IIO_resource[dn / MAX_LOGIC_IIO_STACK].StackRes[dn % MAX_LOGIC_IIO_STACK];
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

void iio_pci_domain_scan_bus(struct device *dev)
{
	const STACK_RES *sr = domain_to_stack_res(dev);
	if (!sr)
		return;

	if (!dev->link_list) {
		dev->link_list = calloc(1, sizeof(struct bus));
		if (!dev->link_list)
			die("%s: out of memory.\n", __func__);
	}

	struct bus *bus = dev->link_list;
	bus->dev = dev;
	bus->secondary = sr->BusBase;
	bus->subordinate = sr->BusBase;
	bus->max_subordinate = sr->BusLimit;

	printk(BIOS_SPEW, "Scanning IIO stack %d: busses %x-%x\n", dev->path.domain.domain,
	       dev->link_list->secondary, dev->link_list->max_subordinate);
	pci_host_bridge_scan_bus(dev);
}

/*
 * Used by IIO stacks for PCIe bridges. Those contain 1 PCI host bridges,
 *  all the bus numbers on the IIO stack can be used for this bridge
 */
static struct device_operations iio_pcie_domain_ops = {
	.read_resources = iio_pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = iio_pci_domain_scan_bus,
};

/* Attach IIO stack as domains */
void attach_iio_stacks(struct device *dev)
{
	const IIO_UDS *hob = get_iio_uds();
	if (!hob)
		return;

	for (int s = 0; s < hob->PlatformData.numofIIO; ++s) {
		for (int x = 0; x < MAX_LOGIC_IIO_STACK; ++x) {
			if (s == 0 && x == 0)
				continue;

			const STACK_RES *ri = &hob->PlatformData.IIO_resource[s].StackRes[x];
			if (!stack_needs_resource_alloc(ri))
				continue;

			if (!is_pcie_iio_stack_res(ri)) {
				if (CONFIG(HAVE_IOAT_DOMAINS))
					soc_create_ioat_domains(dev->bus, ri);
				continue;
			}

			struct device_path path;
			path.type = DEVICE_PATH_DOMAIN;
			path.domain.domain = s * MAX_LOGIC_IIO_STACK + x;
			struct device *iio_domain = alloc_dev(dev->bus, &path);
			if (iio_domain == NULL)
				die("%s: out of memory.\n", __func__);
			iio_domain->ops = &iio_pcie_domain_ops;
		}
	}
}
