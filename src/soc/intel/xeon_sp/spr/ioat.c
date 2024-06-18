/* SPDX-License-Identifier: GPL-2.0-only */
#include <acpi/acpigen_pci.h>
#include <stdbool.h>

#include <console/console.h>
#include <device/device.h>

#include <defs_iio.h>
#include <hob_iiouds.h>
#include <intelblocks/acpi.h>
#include <intelblocks/vtd.h>
#include <soc/acpi.h>
#include <IioPcieConfigUpd.h>

#include <soc/chip_common.h>

/*
 * Used for IIO stacks for accelerators and other functionality (IOAT).
 * Those have only integrated PCI endpoints (no bridges) behind the host bridge.
 */

static struct device_operations ioat_domain_ops = {
	.read_resources = noop_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = pci_host_bridge_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name        = soc_acpi_name,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt	   = pci_domain_fill_ssdt,
#endif
};

static struct device *const create_ioat_domain(const union xeon_domain_path dp, struct bus *const upstream,
				const unsigned int bus_base, const unsigned int bus_limit,
				const resource_t mem32_base, const resource_t mem32_limit,
				const resource_t mem64_base, const resource_t mem64_limit,
				const char *prefix, const size_t pci_segment_group)
{
	union xeon_domain_path new_path = {
		.domain_path = dp.domain_path
	};
	new_path.bus = bus_base;

	struct device_path path = {
		.type = DEVICE_PATH_DOMAIN,
		.domain = {
			.domain = new_path.domain_path,
		},
	};
	struct device *const domain = alloc_find_dev(upstream, &path);
	if (!domain)
		die("%s: out of memory.\n", __func__);

	domain->ops = &ioat_domain_ops;
	iio_domain_set_acpi_name(domain, prefix);

	struct bus *const bus = alloc_bus(domain);
	bus->secondary = bus_base;
	bus->subordinate = bus->secondary;
	bus->max_subordinate = bus_limit;
	bus->segment_group = pci_segment_group;

	unsigned int index = 0;

	if (mem32_base <= mem32_limit)
		domain_mem_window_from_to(domain, index++, mem32_base, mem32_limit + 1);

	if (mem64_base <= mem64_limit)
		domain_mem_window_from_to(domain, index++, mem64_base, mem64_limit + 1);

	return domain;
}

void create_ioat_domains(const union xeon_domain_path path,
			struct bus *const bus,
			const STACK_RES *const sr,
			const size_t pci_segment_group)
{
	if (sr->BusLimit < sr->BusBase + HQM_BUS_OFFSET + HQM_RESERVED_BUS) {
		printk(BIOS_WARNING,
			"Ignoring IOAT domain with limited bus range.\n");
		return;
	}

	if (sr->PciResourceMem64Limit - sr->PciResourceMem64Base + 1
	    < 2 * CPM_MMIO_SIZE + 2 * HQM_MMIO_SIZE) {
		printk(BIOS_WARNING,
		       "Ignoring IOAT domain with limited 64-bit MMIO window.\n");
		return;
	}

	/* The FSP HOB doesn't provide accurate information about the
	   resource allocation. Hence use pre-defined offsets. Based
	   on ACPI code in create_dsdt_ioat_resource(), soc_acpi.c: */
	resource_t mem64_base, mem64_limit, bus_base, bus_limit;

	/* CPM0 */
	mem64_base = sr->PciResourceMem64Base;
	mem64_limit = mem64_base + CPM_MMIO_SIZE - 1;
	bus_base = sr->BusBase + CPM_BUS_OFFSET;
	bus_limit = bus_base + CPM_RESERVED_BUS;
	create_ioat_domain(path, bus, bus_base, bus_limit, -1, 0, mem64_base, mem64_limit,
			   DOMAIN_TYPE_CPM0, pci_segment_group);

	/* HQM0 */
	mem64_base = mem64_limit + 1;
	mem64_limit = mem64_base + HQM_MMIO_SIZE - 1;
	bus_base = sr->BusBase + HQM_BUS_OFFSET;
	bus_limit = bus_base + HQM_RESERVED_BUS;
	create_ioat_domain(path, bus, bus_base, bus_limit, -1, 0, mem64_base, mem64_limit,
			   DOMAIN_TYPE_HQM0, pci_segment_group);

	/* CPM1 (optional) */
	mem64_base = mem64_limit + 1;
	mem64_limit = mem64_base + CPM_MMIO_SIZE - 1;
	bus_base = sr->BusBase + CPM1_BUS_OFFSET;
	bus_limit = bus_base + CPM_RESERVED_BUS;
	if (bus_limit <= sr->BusLimit)
		create_ioat_domain(path, bus, bus_base, bus_limit, -1, 0, mem64_base, mem64_limit,
				   DOMAIN_TYPE_CPM1, pci_segment_group);

	/* HQM1 (optional) */
	mem64_base = mem64_limit + 1;
	mem64_limit = mem64_base + HQM_MMIO_SIZE - 1;
	bus_base = sr->BusBase + HQM1_BUS_OFFSET;
	bus_limit = bus_base + HQM_RESERVED_BUS;
	if (bus_limit <= sr->BusLimit)
		create_ioat_domain(path, bus, bus_base, bus_limit, -1, 0, mem64_base, mem64_limit,
				   DOMAIN_TYPE_HQM1, pci_segment_group);

	/* DINO */
	mem64_base = mem64_limit + 1;
	mem64_limit = sr->PciResourceMem64Limit;
	bus_base = sr->BusBase;
	bus_limit = bus_base;
	struct device *const dev = create_ioat_domain(path, bus, bus_base, bus_limit,
				sr->PciResourceMem32Base, sr->PciResourceMem32Limit,
				mem64_base, mem64_limit, DOMAIN_TYPE_DINO, pci_segment_group);

	/* Declare domain reserved MMIO */
	uint64_t reserved_mmio = sr->VtdBarAddress + vtd_probe_bar_size(pcidev_on_root(0, 0));
	if ((reserved_mmio >= sr->PciResourceMem32Base) &&
	    (reserved_mmio <= sr->PciResourceMem32Limit)) {
		int index = 0;
		for (struct resource *res = dev->resource_list; res; res = res->next)
			index++;
		mmio_range(dev, index, reserved_mmio,
			sr->PciResourceMem32Limit - reserved_mmio + 1);
	}
}
