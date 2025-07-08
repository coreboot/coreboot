/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen_pci.h>
#include <amdblocks/acpimmio_map.h>
#include <amdblocks/ioapic.h>
#include <amdblocks/data_fabric.h>
#include <amdblocks/iomap.h>
#include <amdblocks/memmap.h>
#include <amdblocks/root_complex.h>
#include <amdblocks/smn.h>
#include <arch/ioapic.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <drivers/amd/opensil/opensil.h>
#include <soc/iomap.h>
#include <types.h>

#define IOHC_IOAPIC_BASE_ADDR_LO 0x2f0

/*
 * 12GiB of DF reserved space at the top of physical address space. The
 * address is not fixed and depends on SME state, because it can reduce the
 * available physical address space.
 */
#define DATA_FABRIC_RESERVED_BASE \
	(POWER_OF_2(cpu_phys_address_size()) - DF_RESERVED_TOP_12GB_MMIO_SIZE)
#define DATA_FABRIC_RESERVED_SIZE	DF_RESERVED_TOP_12GB_MMIO_SIZE

/*
 * 64KiB of ACPI EINJ buffer, reserve it. Reads return FFs and cause errors in
 * memtest86 if not reserved, and needless to say, crashes in operating
 * systems too.
 */
#define ACPI_EINJ_RESERVED_BASE		(4ull * GiB)
#define ACPI_EINJ_RESERVED_SIZE		(64 * KiB)

void read_soc_memmap_resources(struct device *domain, unsigned long *idx)
{
	read_lower_soc_memmap_resources(domain, idx);

	if (is_domain0(domain)) {
		reserved_ram_range(domain, (*idx)++, ACPI_EINJ_RESERVED_BASE,
						     ACPI_EINJ_RESERVED_SIZE);

		mmio_range(domain, (*idx)++, IOMMU_RESERVED_MMIO_BASE, 12ULL * GiB);
		mmio_range(domain, (*idx)++, DATA_FABRIC_RESERVED_BASE,
					     DATA_FABRIC_RESERVED_SIZE);

		mmio_range(domain, (*idx)++, AMD_SB_ACPI_MMIO_ADDR, 0x2000);
		mmio_range(domain, (*idx)++, ALINK_AHB_ADDRESS, 0x20000);
	}

	amd_opensil_add_memmap(domain, idx);
}

static void turin_domain_set_resources(struct device *domain)
{
	if (domain->downstream->bridge_ctrl & PCI_BRIDGE_CTL_VGA) {
		printk(BIOS_DEBUG, "Setting VGA decoding for domain 0x%x\n",
		       dev_get_domain_id(domain));
		const union df_vga_en vga_en = {
			.ve = 1,
			.dst_fabric_id = get_iohc_fabric_id(domain),
		};
		data_fabric_broadcast_write32(DF_VGA_EN, vga_en.raw);
	}

	pci_domain_set_resources(domain);

	/* Enable IOAPIC memory decoding */
	struct resource *res = probe_resource(domain, IOMMU_IOAPIC_IDX);
	if (res) {
		const uint32_t iohc_misc_base = get_iohc_misc_smn_base(domain);
		uint32_t ioapic_base = smn_read32(iohc_misc_base | IOHC_IOAPIC_BASE_ADDR_LO);
		ioapic_base |= (1 << 0);
		smn_write32(iohc_misc_base | IOHC_IOAPIC_BASE_ADDR_LO, ioapic_base);
	}
}

static const char *turin_domain_acpi_name(const struct device *domain)
{
	const unsigned int domain_id = dev_get_domain_id(domain);
	const char *domain_acpi_names[8] = {
		"S0B0",
		"S0B1",
		"S0B2",
		"S0B3",
		"S0B4",
		"S0B5",
		"S0B6",
		"S0B7",
	};

	if (domain_id < ARRAY_SIZE(domain_acpi_names))
		return domain_acpi_names[domain_id];

	return NULL;
}

struct device_operations turin_pci_domain_ops = {
	.read_resources	= amd_pci_domain_read_resources,
	.set_resources	= turin_domain_set_resources,
	.scan_bus	= amd_pci_domain_scan_bus,
	.init		= amd_pci_domain_init,
	.acpi_name	= turin_domain_acpi_name,
	.acpi_fill_ssdt	= pci_domain_fill_ssdt,
};
