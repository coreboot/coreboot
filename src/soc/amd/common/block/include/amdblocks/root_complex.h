/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_ROOT_COMPLEX_H
#define AMD_BLOCK_ROOT_COMPLEX_H

#include <device/device.h>
#include <device/pci_type.h>
#include <types.h>

#define SMN_IOHC_MISC_BASE_13B1	0x13b10000
#define SMN_IOHC_MISC_BASE_13C1	0x13c10000
#define SMN_IOHC_MISC_BASE_13D1	0x13d10000
#define SMN_IOHC_MISC_BASE_13E1	0x13e10000

#define SMN_IOHC_MISC_BASE_1D41	0x1d410000
#define SMN_IOHC_MISC_BASE_1D51	0x1d510000
#define SMN_IOHC_MISC_BASE_1D61	0x1d610000
#define SMN_IOHC_MISC_BASE_1D71	0x1d710000

#define IOHC_MMIO_EN	BIT(0)

#define NON_PCI_RES_IDX_AUTO	0

struct domain_iohc_info {
	uint16_t fabric_id;
	/* Zero for an absent/undecoded domain in a sparse table. */
	uint32_t misc_smn_base;
	/* Zero initialization preserves the existing socket 0, bus 0 SMN endpoint. */
	pci_devfn_t smn_dev;
	uint8_t socket;
};

struct non_pci_mmio_reg {
	uint32_t iohc_misc_offset;
	uint64_t mask;
	uint64_t size;
	unsigned long res_idx; /* Use NON_PCI_RES_IDX_AUTO or a specific resource index */
};

void read_non_pci_resources(struct device *domain, unsigned long *idx);

void read_soc_memmap_resources(struct device *domain, unsigned long *idx);

uint32_t get_iohc_misc_smn_base(struct device *domain);
/* Look up the IOHC and SMN endpoint for this domain. */
const struct domain_iohc_info *get_domain_iohc_info(const struct device *domain);

/* Domain-indexed table: absent domains may have a zero misc_smn_base. */
const struct domain_iohc_info *get_iohc_info(size_t *count);
const struct non_pci_mmio_reg *get_iohc_non_pci_mmio_regs(size_t *count);

signed int get_iohc_fabric_id(struct device *domain);

void read_fsp_resources(struct device *dev, unsigned long *idx);

void amd_pci_domain_init(struct device *domain);

#endif /* AMD_BLOCK_ROOT_COMPLEX_H */
