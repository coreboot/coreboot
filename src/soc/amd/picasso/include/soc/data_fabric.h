/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_DATA_FABRIC_H
#define AMD_PICASSO_DATA_FABRIC_H

#include <types.h>

/* D18F0 - Fabric Configuration registers */
#define D18F0_MMIO_BASE0		0x200
#define D18F0_MMIO_LIMIT0		0x204
#define   D18F0_MMIO_SHIFT		16
#define D18F0_MMIO_CTRL0		0x208

#define DF_FICAA_BIOS			0x5C
#define DF_FICAD_LO			0x98
#define DF_FICAD_HI			0x9C

#define IOMS0_FABRIC_ID			9

#define NUM_NB_MMIO_REGS		8

union df_mmio_control {
	struct {
		uint32_t re        :  1; /* [ 0.. 0] */
		uint32_t we        :  1; /* [ 1.. 1] */
		uint32_t           :  2; /* [ 3.. 2] */
		uint32_t fabric_id :  8; /* [11.. 4] */
		uint32_t np        :  1; /* [12..12] */
		uint32_t           : 19; /* [31..13] */
	};
	uint32_t raw;
};

union df_ficaa {
	struct {
		uint32_t cfg_inst_acc_en : 1; /* [ 0.. 0] */
		uint32_t                 : 1; /* [ 1.. 1] */
		uint32_t reg_num         : 9; /* [10.. 2] */
		uint32_t func_num        : 3; /* [13..11] */
		uint32_t b64_en          : 1; /* [14..14] */
		uint32_t                 : 1; /* [15..15] */
		uint32_t inst_id         : 8; /* [23..16] */
		uint32_t                 : 8; /* [31..24] */
	};
	uint32_t raw;
};


#define D18F0_VGAEN			0x80
#define   VGA_ADDR_ENABLE		BIT(0)

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

#endif /* AMD_PICASSO_DATA_FABRIC_H */
