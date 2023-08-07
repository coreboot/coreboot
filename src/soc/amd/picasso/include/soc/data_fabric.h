/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PICASSO_DATA_FABRIC_H
#define AMD_PICASSO_DATA_FABRIC_H

#include <amdblocks/data_fabric_defs.h>
#include <types.h>

#define IOMS0_FABRIC_ID			9

#define D18F0_VGAEN			DF_REG_ID(0, 0x80)
#define   VGA_ADDR_ENABLE		BIT(0)

#define DF_PCI_CFG_MAP0			DF_REG_ID(0, 0xa0)

#define DF_PCI_CFG_MAP_COUNT		8

#define DF_PCI_CFG_MAP(reg)		(DF_PCI_CFG_MAP0 + (reg) * sizeof(uint32_t))

union df_pci_cfg_map {
	struct {
		uint32_t re		: 1; /* [ 0.. 0] */
		uint32_t we		: 1; /* [ 1.. 1] */
		uint32_t		: 2; /* [ 2.. 3] */
		uint32_t dst_fabric_id	: 8; /* [ 4..11] */
		uint32_t		: 4; /* [12..15] */
		uint32_t bus_num_base	: 8; /* [16..23] */
		uint32_t bus_num_limit	: 8; /* [24..31] */
	};
	uint32_t raw;
};

#define DF_IO_BASE0			DF_REG_ID(0, 0xc0)
#define DF_IO_LIMIT0			DF_REG_ID(0, 0xc4)

#define DF_IO_REG_COUNT			8

#define DF_IO_REG_OFFSET(instance)	((instance) * 2 * sizeof(uint32_t))
#define DF_IO_BASE(reg)			(DF_IO_BASE0 + DF_IO_REG_OFFSET(reg))
#define DF_IO_LIMIT(reg)		(DF_IO_LIMIT0 + DF_IO_REG_OFFSET(reg))

union df_io_base {
	struct {
		uint32_t re		:  1; /* [ 0.. 0] */
		uint32_t we		:  1; /* [ 1.. 1] */
		uint32_t		:  3; /* [ 2.. 4] */
		uint32_t ie		:  1; /* [ 5.. 5] */
		uint32_t		:  6; /* [ 6..11] */
		uint32_t io_base	: 13; /* [12..24] */
		uint32_t		:  7; /* [25..31] */
	};
	uint32_t raw;
};

union df_io_limit {
	struct {
		uint32_t dst_fabric_id	:  8; /* [ 0.. 7] */
		uint32_t		:  4; /* [ 8..11] */
		uint32_t io_limit	: 13; /* [12..24] */
		uint32_t		:  7; /* [25..31] */
	};
	uint32_t raw;
};

#define DF_IO_ADDR_SHIFT		12

#define DF_DRAM_HOLE_CTL		DF_REG_ID(0, 0x104)
#define   DRAM_HOLE_CTL_VALID		BIT(0)
#define   DRAM_HOLE_CTL_BASE_SHFT	24
#define   DRAM_HOLE_CTL_BASE		(0xff << DRAM_HOLE_CTL_BASE_SHFT)

#define DF_DRAM_BASE0			DF_REG_ID(0, 0x110)
#define   DRAM_BASE_REG_VALID		BIT(0)
#define   DRAM_BASE_HOLE_EN		BIT(1)
#define   DRAM_BASE_INTLV_CH_SHFT	4
#define   DRAM_BASE_INTLV_CH		(0xf << DRAM_BASE_INTLV_CH_SHFT)
#define   DRAM_BASE_INTLV_SEL_SHFT	8
#define   DRAM_BASE_INTLV_SEL		(0x7 << DRAM_BASE_INTLV_SEL_SHFT)
#define   DRAM_BASE_ADDR_SHFT		12
#define   DRAM_BASE_ADDR		(0xfffff << DRAM_BASE_ADDR_SHFT)

#define DF_DRAM_LIMIT0			DF_REG_ID(0, 0x114)
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
						+ DF_DRAM_BASE0)
#define DF_DRAM_LIMIT(dram_map_pair)	((dram_map_pair) * 2 * sizeof(uint32_t) \
						+ DF_DRAM_LIMIT0)

#define DF_MMIO_BASE0			DF_REG_ID(0, 0x200)
#define DF_MMIO_LIMIT0			DF_REG_ID(0, 0x204)
#define   DF_MMIO_SHIFT			16
#define DF_MMIO_CTRL0			DF_REG_ID(0, 0x208)

#define DF_MMIO_REG_SET_SIZE		4
#define DF_MMIO_REG_SET_COUNT		8

union df_mmio_control {
	struct {
		uint32_t re		:  1; /* [ 0.. 0] */
		uint32_t we		:  1; /* [ 1.. 1] */
		uint32_t		:  2; /* [ 3.. 2] */
		uint32_t dst_fabric_id	:  8; /* [11.. 4] */
		uint32_t np		:  1; /* [12..12] */
		uint32_t		: 19; /* [31..13] */
	};
	uint32_t raw;
};

#define DF_FICAA_BIOS			DF_REG_ID(4, 0x5C)
#define DF_FICAD_LO			DF_REG_ID(4, 0x98)
#define DF_FICAD_HI			DF_REG_ID(4, 0x9C)

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

#endif /* AMD_PICASSO_DATA_FABRIC_H */
