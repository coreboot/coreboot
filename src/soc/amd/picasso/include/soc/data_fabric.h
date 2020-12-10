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

#define D18F0_DRAM_HOLE_CTL		0x104
#define   DRAM_HOLE_CTL_VALID		BIT(0)
#define   DRAM_HOLE_CTL_BASE_SHFT	24
#define   DRAM_HOLE_CTL_BASE		(0xff << DRAM_HOLE_CTL_BASE_SHFT)

#define D18F0_DRAM_BASE0		0x110
#define   DRAM_BASE_REG_VALID		BIT(0)
#define   DRAM_BASE_HOLE_EN		BIT(1)
#define   DRAM_BASE_INTLV_CH_SHFT	4
#define   DRAM_BASE_INTLV_CH		(0xf << DRAM_BASE_INTLV_CH_SHFT)
#define   DRAM_BASE_INTLV_SEL_SHFT	8
#define   DRAM_BASE_INTLV_SEL		(0x7 << DRAM_BASE_INTLV_SEL_SHFT)
#define   DRAM_BASE_ADDR_SHFT		12
#define   DRAM_BASE_ADDR		(0xfffff << DRAM_BASE_ADDR_SHFT)

#define D18F0_DRAM_LIMIT0			0x114
#define   DRAM_LIMIT_DST_ID_SHFT		0
#define   DRAM_LIMIT_DST_ID			(0xff << DRAM_LIMIT_DST_ID_SHFT)
#define   DRAM_LIMIT_INTLV_NUM_SOCK_SHFT	8
#define   DRAM_LIMIT_INTLV_NUM_SOCK		(0x1 << DRAM_LIMIT_INTLV_NUM_SOCK_SHFT)
#define   DRAM_LIMIT_INTLV_NUM_DIE_SHFT		10
#define   DRAM_LIMIT_INTLV_NUM_DIE		(0x3 << DRAM_LIMIT_INTLV_NUM_DIE_SHFT)
#define   DRAM_LIMIT_ADDR_SHFT			12
#define   DRAM_LIMIT_ADDR			(0xfffff << DRAM_LIMIT_ADDR_SHFT)

#define PICASSO_NUM_DRAM_REG		2

#define DF_DRAM_BASE(dram_map_pair)	((dram_map_pair) * 2 * sizeof(uint32_t) \
						+ D18F0_DRAM_BASE0)
#define DF_DRAM_LIMIT(dram_map_pair)	((dram_map_pair) * 2 * sizeof(uint32_t) \
						+ D18F0_DRAM_LIMIT0)

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
