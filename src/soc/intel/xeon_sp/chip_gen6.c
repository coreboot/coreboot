/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <post.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/numa.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <stdlib.h>

static const UDS_PCIROOT_RES *domain_to_pciroot_res(const struct device *dev)
{
	assert(dev->path.type == DEVICE_PATH_DOMAIN);
	const union xeon_domain_path dn = {
		.domain_path = dev->path.domain.domain
	};

	const IIO_UDS *hob = get_iio_uds();
	assert(hob != NULL);

	const UDS_STACK_RES *sr = &hob->PlatformData.IIO_resource[dn.socket].StackRes[dn.stack];
	for (unsigned int index = 0; index < sr->PciRootBridgeNum; index++) {
		if (sr->PciRoot[index].BusBase == dev->downstream->secondary)
			return &sr->PciRoot[index];
	}

	return NULL;
}

static void iio_pci_domain_read_resources(struct device *dev)
{
	int index = 0;
	const UDS_PCIROOT_RES *pr = domain_to_pciroot_res(dev);

	/* Initialize the system-wide I/O space constraints. */
	if (pr->IoBase <= pr->IoLimit)
		domain_io_window_from_to(dev, index++,
			pr->IoBase, pr->IoLimit + 1);

	/* The 0 - 0xfff IO range is not reported by the HOB but still gets decoded */
	if (is_domain0(dev)) {
		struct resource *res = new_resource(dev, index++);
		res->base = 0;
		res->limit = 0xfff;
		res->size = 0x1000;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
	}

	/* Initialize the system-wide memory resources constraints. */
	if (pr->Mmio32Base <= pr->Mmio32Limit)
		domain_mem_window_from_to(dev, index++,
			pr->Mmio32Base, pr->Mmio32Limit + 1);

	/* Initialize the system-wide memory resources constraints. */
	if (pr->Mmio64Base <= pr->Mmio64Limit)
		domain_mem_window_from_to(dev, index++,
			pr->Mmio64Base, pr->Mmio64Limit + 1);
}

static void iio_pci_domain_fill_ssdt(const struct device *domain)
{
	soc_pci_domain_fill_ssdt(domain);
	pci_domain_fill_ssdt(domain);
}

static struct device_operations iio_pcie_domain_ops = {
	.read_resources = iio_pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.scan_bus = pci_host_bridge_scan_bus,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = soc_acpi_name,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_fill_ssdt	= iio_pci_domain_fill_ssdt,
#endif
};

void create_xeonsp_domains(const union xeon_domain_path dp, struct bus *bus,
				const xSTACK_RES *sr, const size_t pci_segment_group)
{
	for (unsigned int index = 0; index < sr->PciRootBridgeNum; index++) {
		const UDS_PCIROOT_RES *pr = &sr->PciRoot[index];
		create_domain(dp, bus,
			pr->BusBase,
			pr->BusLimit,
			pciroot_res_to_domain_type(sr, pr),
			&iio_pcie_domain_ops,
			pci_segment_group);
	}
}
