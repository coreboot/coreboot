/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_DATA_FABRIC_H
#define AMD_BLOCK_DATA_FABRIC_H

#include <amdblocks/pci_devs.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#define BROADCAST_FABRIC_ID		0xff

/* D18F0 - Fabric Configuration registers */
#define D18F0_MMIO_BASE0		0x200
#define D18F0_MMIO_LIMIT0		0x204
#define   D18F0_MMIO_SHIFT		16
#define D18F0_MMIO_CTRL0		0x208
/* The MMIO_NP bit is SoC-specific */
#define   DF_MMIO_DST_FABRIC_ID_SHIFT	4
#define   DF_MMIO_WE			BIT(1)
#define   DF_MMIO_RE			BIT(0)

/* The number of data fabric MMIO registers is SoC-specific */
#define NB_MMIO_BASE(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_BASE0)
#define NB_MMIO_LIMIT(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_LIMIT0)
#define NB_MMIO_CONTROL(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_CTRL0)

uint32_t data_fabric_read32(uint8_t function, uint16_t reg, uint8_t instance_id);
void data_fabric_write32(uint8_t function, uint16_t reg, uint8_t instance_id, uint32_t data);

static __always_inline
uint32_t data_fabric_broadcast_read32(uint8_t function, uint16_t reg)
{
	/* No bit masking required. Macros will apply mask to values. */
	return pci_read_config32(_SOC_DEV(DF_DEV, function), reg);
}

static __always_inline
void data_fabric_broadcast_write32(uint8_t function, uint16_t reg, uint32_t data)
{
	/* No bit masking required. Macros will apply mask to values. */
	pci_write_config32(_SOC_DEV(DF_DEV, function), reg, data);
}

void data_fabric_print_mmio_conf(void);
void data_fabric_disable_mmio_reg(unsigned int reg);
int data_fabric_find_unused_mmio_reg(void);
void data_fabric_set_mmio_np(void);

#endif /* AMD_BLOCK_DATA_FABRIC_H */
