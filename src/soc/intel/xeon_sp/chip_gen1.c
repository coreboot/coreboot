/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <intelblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/soc_util.h>
#include <soc/util.h>

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

static void iio_pci_domain_read_resources(struct device *dev)
{
	const STACK_RES *sr = domain_to_stack_res(dev);

	if (!sr)
		return;

	int index = 0;

	if (is_domain0(dev)) {
		/* The 0 - 0xfff IO range is not reported by the HOB but still gets decoded */
		struct resource *res = new_resource(dev, index++);
		res->base = 0;
		res->size = 0x1000;
		res->limit = 0xfff;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
	}

	if (sr->PciResourceIoBase < sr->PciResourceIoLimit)
		domain_io_window_from_to(dev, index++,
				sr->PciResourceIoBase, sr->PciResourceIoLimit + 1);

	if (sr->PciResourceMem32Base < sr->PciResourceMem32Limit)
		domain_mem_window_from_to(dev, index++,
				sr->PciResourceMem32Base, sr->PciResourceMem32Limit + 1);

	if (sr->PciResourceMem64Base < sr->PciResourceMem64Limit)
		domain_mem_window_from_to(dev, index++,
				sr->PciResourceMem64Base, sr->PciResourceMem64Limit + 1);
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

static void create_pcie_domains(const union xeon_domain_path dp, struct bus *upstream,
				    const STACK_RES *sr, const size_t pci_segment_group)
{
	create_domain(dp, upstream, sr->BusBase, sr->BusLimit, DOMAIN_TYPE_PCIE,
			   &iio_pcie_domain_ops, pci_segment_group);
}

/*
 * On the first Xeon-SP generations there are no separate UBOX stacks,
 * and the UBOX devices reside on the first and second IIO. Starting
 * with 3rd gen Xeon-SP the UBOX devices are located on their own IIO.
 */
static void create_ubox_domains(const union xeon_domain_path dp, struct bus *upstream,
				    const STACK_RES *sr, const size_t pci_segment_group)
{
	/* Only expect 2 UBOX buses here */
	assert(sr->BusBase + 1 == sr->BusLimit);

	create_domain(dp, upstream, sr->BusBase, sr->BusBase, DOMAIN_TYPE_UBX0,
			   &ubox_pcie_domain_ops, pci_segment_group);
	create_domain(dp, upstream, sr->BusLimit, sr->BusLimit, DOMAIN_TYPE_UBX1,
			   &ubox_pcie_domain_ops, pci_segment_group);
}

void create_cxl_domains(const union xeon_domain_path dp, struct bus *bus,
			const STACK_RES *sr, const size_t pci_segment_group);

#if CONFIG(SOC_INTEL_HAS_CXL)
static void iio_cxl_domain_read_resources(struct device *dev)
{
	const STACK_RES *sr = domain_to_stack_res(dev);

	if (!sr)
		return;

	int index = 0;

	if (sr->IoBase < sr->PciResourceIoBase)
		domain_io_window_from_to(dev, index++,
				sr->IoBase, sr->PciResourceIoBase);

	if (sr->Mmio32Base < sr->PciResourceMem32Base)
		domain_mem_window_from_to(dev, index++,
				sr->Mmio32Base, sr->PciResourceMem32Base);

	if (sr->Mmio64Base < sr->PciResourceMem64Base)
		domain_mem_window_from_to(dev, index++,
				sr->Mmio64Base, sr->PciResourceMem64Base);
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

void create_cxl_domains(const union xeon_domain_path dp, struct bus *bus,
			    const STACK_RES *sr, const size_t pci_segment_group)
{
	assert(sr->BusBase + 1 <= sr->BusLimit);

	/* 1st domain contains PCIe RCiEPs */
	create_domain(dp, bus, sr->BusBase, sr->BusBase, DOMAIN_TYPE_PCIE,
			   &iio_pcie_domain_ops, pci_segment_group);
	/* 2nd domain contains CXL 1.1 end-points */
	create_domain(dp, bus, sr->BusBase + 1, sr->BusLimit, DOMAIN_TYPE_CXL,
			   &iio_cxl_domain_ops, pci_segment_group);
}
#endif //CONFIG(SOC_INTEL_HAS_CXL)

void create_xeonsp_domains(const union xeon_domain_path dp, struct bus *bus,
				const STACK_RES *sr, const size_t pci_segment_group)
{
	if (is_ubox_stack_res(sr))
		create_ubox_domains(dp, bus, sr, pci_segment_group);
	else if (CONFIG(SOC_INTEL_HAS_CXL) && is_iio_cxl_stack_res(sr))
		create_cxl_domains(dp, bus, sr, pci_segment_group);
	else if (is_pcie_iio_stack_res(sr))
		create_pcie_domains(dp, bus, sr, pci_segment_group);
	else if (CONFIG(HAVE_IOAT_DOMAINS) && is_ioat_iio_stack_res(sr))
		create_ioat_domains(dp, bus, sr, pci_segment_group);
}

/*
 * Route PAM segment access to DRAM
 * Only call this code from socket0!
 */
void unlock_pam_regions(void)
{
	uint32_t pam0123_unlock_dram = 0x33333330;
	uint32_t pam456_unlock_dram = 0x00333333;
	/* Get UBOX(1) for socket0 */
	uint32_t bus1 = socket0_get_ubox_busno(PCU_IIO_STACK);

	/* Assume socket0 owns PCI segment 0 */
	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM0123_CSR, pam0123_unlock_dram);
	pci_io_write_config32(PCI_DEV(bus1, SAD_ALL_DEV, SAD_ALL_FUNC),
		SAD_ALL_PAM456_CSR, pam456_unlock_dram);

	uint32_t reg1 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM0123_CSR);
	uint32_t reg2 = pci_io_read_config32(PCI_DEV(bus1, SAD_ALL_DEV,
		SAD_ALL_FUNC), SAD_ALL_PAM456_CSR);
	printk(BIOS_DEBUG, "%s:%s pam0123_csr: 0x%x, pam456_csr: 0x%x\n",
		__FILE__, __func__, reg1, reg2);
}
