/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_DATA_FABRIC_H
#define AMD_BLOCK_DATA_FABRIC_H

#include <amdblocks/pci_devs.h>
#include <device/pci_ops.h>
#include <soc/data_fabric.h>
#include <soc/pci_devs.h>
#include <stdint.h>

#define BROADCAST_FABRIC_ID		0xff

#define DF_MMIO_REG_OFFSET(instance) ((instance) * DF_MMIO_REG_SET_SIZE * sizeof(uint32_t))

/* The number of data fabric MMIO registers is SoC-specific */
#define DF_MMIO_BASE(reg)	(D18F0_MMIO_BASE0 + DF_MMIO_REG_OFFSET(reg))
#define DF_MMIO_LIMIT(reg)	(D18F0_MMIO_LIMIT0 + DF_MMIO_REG_OFFSET(reg))
#define DF_MMIO_CONTROL(reg)	(D18F0_MMIO_CTRL0 + DF_MMIO_REG_OFFSET(reg))

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
