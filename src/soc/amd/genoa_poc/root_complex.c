/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/ioapic.h>
#include <amdblocks/root_complex.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

uint32_t get_iohc_misc_smn_base(struct device *domain)
{
	switch (domain->path.domain.domain) {
	case 0:
		return SMN_IOHC_MISC_BASE_13C1;
	case 1:
		return SMN_IOHC_MISC_BASE_13B1;
	case 2:
		return SMN_IOHC_MISC_BASE_13E1;
	case 3:
		return SMN_IOHC_MISC_BASE_13D1;
	default:
		printk(BIOS_ERR, "Invalid domain 0x%x with no corresponding IOHC device.\n",
		       domain->path.domain.domain);
		return 0;
	}
}

static const struct non_pci_mmio_reg non_pci_mmio[] = {
	{ 0x2d8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x2e0, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x2e8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	/* The hardware has a 256 byte alignment requirement for the IOAPIC MMIO base, but we
	   tell the FSP to configure a 4k-aligned base address and this is reported as 4 KiB
	   resource. */
	{ 0x2f0, 0xffffffffff00ull,   4 * KiB, IOMMU_IOAPIC_IDX },
	{ 0x2f8, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x300, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x308, 0xfffffffff000ull,   4 * KiB, NON_PCI_RES_IDX_AUTO },
	{ 0x310, 0xfffffff00000ull,   1 * MiB, NON_PCI_RES_IDX_AUTO },
	{ 0x318, 0xfffffff80000ull, 512 * KiB, NON_PCI_RES_IDX_AUTO },
};

const struct non_pci_mmio_reg *get_iohc_non_pci_mmio_regs(size_t *count)
{
	*count = ARRAY_SIZE(non_pci_mmio);
	return non_pci_mmio;
}

signed int get_iohc_fabric_id(struct device *domain)
{
	switch (domain->path.domain.domain) {
	case 0:
		return 0x22;
	case 1:
		return 0x23;
	case 2:
		return 0x21;
	case 3:
		return 0x20;
	default:
		return -1;
	}
}
