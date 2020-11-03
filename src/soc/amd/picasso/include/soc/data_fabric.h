/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_DATA_FABRIC_H
#define AMD_PICASSO_DATA_FABRIC_H

#include <types.h>

/* D18F0 - Fabric Configuration registers */
#define IOMS0_FABRIC_ID			9
#define BROADCAST_FABRIC_ID		0xff

#define D18F0_VGAEN			0x80
#define   VGA_ADDR_ENABLE		BIT(0)

#define D18F0_MMIO_BASE0		0x200
#define D18F0_MMIO_LIMIT0		0x204
#define   D18F0_MMIO_SHIFT		16
#define D18F0_MMIO_CTRL0		0x208
#define   MMIO_NP			BIT(12)
#define   MMIO_DST_FABRIC_ID_SHIFT	4
#define   MMIO_WE			BIT(1)
#define   MMIO_RE			BIT(0)
#define NUM_NB_MMIO_REGS		8
#define NB_MMIO_BASE(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_BASE0)
#define NB_MMIO_LIMIT(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_LIMIT0)
#define NB_MMIO_CONTROL(reg)		((reg) * 4 * sizeof(uint32_t) + D18F0_MMIO_CTRL0)

#define DF_FICAA_BIOS				0x5C
#define DF_FICAD_LO				0x98
#define DF_FICAD_HI				0x9C

#define DF_IND_CFG_INST_ACC_EN			(1 << 0)
#define DF_IND_CFG_ACC_REG_SHIFT		2
#define DF_IND_CFG_ACC_REG_MASK			(0x1ff << DF_IND_CFG_ACC_REG_SHIFT)
#define DF_IND_CFG_ACC_FUN_SHIFT		11
#define DF_IND_CFG_ACC_FUN_MASK			(0x7 << DF_IND_CFG_ACC_REG_SHIFT)
#define DF_IND_CFG_64B_EN_SHIFT			14
#define DF_IND_CFG_64B_EN			(0x1 << DF_IND_CFG_ACC_REG_SHIFT)
#define DF_IND_CFG_INST_ID_SHIFT		16
#define DF_IND_CFG_INST_ID_MASK			(0xff << DF_IND_CFG_ACC_REG_SHIFT)

void data_fabric_set_mmio_np(void);
uint32_t data_fabric_read_reg32(uint8_t function, uint16_t reg, uint8_t  instance_id);

#endif /* AMD_PICASSO_DATA_FABRIC_H */
