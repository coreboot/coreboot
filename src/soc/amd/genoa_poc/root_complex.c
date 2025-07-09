/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/ioapic.h>
#include <amdblocks/root_complex.h>
#include <types.h>

/*
 * The order of IOHCs here is not random. They are sorted so that:
 * 1. The First IOHC is the one with primary FCH. We want the LPC/SMBUS
 *    devices be on bus 0.
 * 2. The rest of IOHCs are listed in an order so that ECAM MMIO region is one
 *    continuous block for all domains.
 *
 * AGESA/OpenSIL sets up the PCI configuration decoding ranges in line with
 * this.
 */
static const struct domain_iohc_info iohc_info[] = {
	[0] = {
		.fabric_id = 0x22,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13C1,
	},
	[1] = {
		.fabric_id = 0x23,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13B1,
	},
	[2] = {
		.fabric_id = 0x21,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13E1,
	},
	[3] = {
		.fabric_id = 0x20,
		.misc_smn_base = SMN_IOHC_MISC_BASE_13D1,
	},
};

const struct domain_iohc_info *get_iohc_info(size_t *count)
{
	*count = ARRAY_SIZE(iohc_info);
	return iohc_info;
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
