/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_MENDOCINO_DATA_FABRIC_H
#define AMD_MENDOCINO_DATA_FABRIC_H

#include <amdblocks/data_fabric_defs.h>
#include <types.h>

#if CONFIG(SOC_AMD_REMBRANDT)
#define IOMS0_FABRIC_ID			0xd
#else
#define IOMS0_FABRIC_ID			0x9
#endif

#define DF_PCI_CFG_MAP0			DF_REG_ID(0, 0xa0)

#define DF_PCI_CFG_MAP_COUNT		8

#define DF_PCI_CFG_MAP(reg)		(DF_PCI_CFG_MAP0 + (reg) * sizeof(uint32_t))

union df_pci_cfg_map {
	struct {
		uint32_t re		: 1; /* [ 0.. 0] */
		uint32_t we		: 1; /* [ 1.. 1] */
		uint32_t		: 2; /* [ 2.. 3] */
		uint32_t dst_fabric_id	: 4; /* [ 4.. 7] */
		uint32_t		: 8; /* [ 8..15] */
		uint32_t bus_num_base	: 8; /* [16..23] */
		uint32_t bus_num_limit	: 8; /* [24..31] */
	};
	uint32_t raw;
};

#define DF_IO_BASE0			DF_REG_ID(0, 0xc0)
#define DF_IO_LIMIT0			DF_REG_ID(0, 0xc4)

#define DF_IO_REG0_OFFSET(instance)	((instance) * 2 * sizeof(uint32_t))
#define DF_IO_BASE_0_7(reg)		(DF_IO_BASE0 + DF_IO_REG0_OFFSET(reg))
#define DF_IO_LIMIT_0_7(reg)		(DF_IO_LIMIT0 + DF_IO_REG0_OFFSET(reg))

#if CONFIG(SOC_AMD_REMBRANDT)
#define DF_IO_BASE8			DF_REG_ID(6, 0xbc)
#define DF_IO_LIMIT8			DF_REG_ID(6, 0xcc)
#define DF_IO_REG_COUNT			12
#define DF_IO_BASE(reg)			((reg) < 8 ? DF_IO_BASE_0_7(reg) : \
						DF_IO_BASE8 + ((reg) - 8) * sizeof(uint32_t))
#define DF_IO_LIMIT(reg)		((reg) < 8 ? DF_IO_LIMIT_0_70(reg) : \
						DF_IO_LIMIT8 + ((reg) - 8) * sizeof(uint32_t))
#else
#define DF_IO_REG_COUNT			8
#define DF_IO_BASE(reg)			DF_IO_BASE_0_7(reg)
#define DF_IO_LIMIT(reg)		DF_IO_LIMIT_0_7(reg)
#endif

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
		uint32_t dst_fabric_id	:  4; /* [ 0.. 3] */
		uint32_t		:  8; /* [ 4..11] */
		uint32_t io_limit	: 13; /* [12..24] */
		uint32_t		:  7; /* [25..31] */
	};
	uint32_t raw;
};

#define DF_IO_ADDR_SHIFT		12

#define DF_MMIO_BASE0			DF_REG_ID(0, 0x200)
#define DF_MMIO_LIMIT0			DF_REG_ID(0, 0x204)
#define   DF_MMIO_SHIFT			16
#define DF_MMIO_CTRL0			DF_REG_ID(0, 0x208)

#if CONFIG(SOC_AMD_REMBRANDT)
#define DF_MMIO_REG_SET_SIZE		3
#else
#define DF_MMIO_REG_SET_SIZE		4
#endif

#define DF_MMIO_REG_SET_COUNT		8

union df_mmio_control {
	struct {
		uint32_t re		:  1; /* [ 0.. 0] */
		uint32_t we		:  1; /* [ 1.. 1] */
		uint32_t		:  2; /* [ 3.. 2] */
		uint32_t dst_fabric_id	:  4; /* [ 7.. 4] */
		uint32_t		:  8; /* [15.. 8] */
		uint32_t np		:  1; /* [16..16] */
		uint32_t		: 15; /* [31..17] */
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


#endif /* AMD_MENDOCINO_DATA_FABRIC_H */
