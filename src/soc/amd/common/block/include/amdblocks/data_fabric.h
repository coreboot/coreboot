/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_DATA_FABRIC_H
#define AMD_BLOCK_DATA_FABRIC_H

#include <amdblocks/data_fabric_defs.h>
#include <amdblocks/pci_devs.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#define BROADCAST_FABRIC_ID		0xff

/* Index of IOAPIC resource associated with IOMMU */
#define IOMMU_IOAPIC_IDX                0x20000120

#define DF_MMIO_REG_OFFSET(instance) ((instance) * DF_MMIO_REG_SET_SIZE * sizeof(uint32_t))

/* The number of data fabric MMIO registers is SoC-specific */
#define DF_MMIO_BASE(reg)	(DF_MMIO_BASE0 + DF_MMIO_REG_OFFSET(reg))
#define DF_MMIO_LIMIT(reg)	(DF_MMIO_LIMIT0 + DF_MMIO_REG_OFFSET(reg))
#define DF_MMIO_CONTROL(reg)	(DF_MMIO_CTRL0 + DF_MMIO_REG_OFFSET(reg))

/* Last 12GB of the usable address space are reserved */
#define DF_RESERVED_TOP_12GB_MMIO_SIZE		(12ULL * GiB)

uint32_t data_fabric_read32(uint16_t fn_reg, uint8_t instance_id);
void data_fabric_write32(uint16_t fn_reg, uint8_t instance_id, uint32_t data);

static __always_inline
uint32_t data_fabric_broadcast_read32(uint16_t fn_reg)
{
	/* No bit masking required. Macros will apply mask to values. */
	return pci_read_config32(_SOC_DEV(DF_DEV, DF_REG_FN(fn_reg)), DF_REG_OFFSET(fn_reg));
}

static __always_inline
void data_fabric_broadcast_write32(uint16_t fn_reg, uint32_t data)
{
	/* No bit masking required. Macros will apply mask to values. */
	pci_write_config32(_SOC_DEV(DF_DEV, DF_REG_FN(fn_reg)), DF_REG_OFFSET(fn_reg), data);
}

void data_fabric_print_mmio_conf(void);
void data_fabric_disable_mmio_reg(unsigned int reg);
int data_fabric_find_unused_mmio_reg(void);
void data_fabric_set_mmio_np(void);

/* Inform the resource allocator about the usable IO and MMIO regions and PCI bus numbers */
void amd_pci_domain_read_resources(struct device *domain);
void amd_pci_domain_scan_bus(struct device *domain);

void amd_pci_domain_fill_ssdt(const struct device *domain);

#endif /* AMD_BLOCK_DATA_FABRIC_H */
