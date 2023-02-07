/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_MENDOCINO_DATA_FABRIC_H
#define AMD_MENDOCINO_DATA_FABRIC_H

#include <types.h>

/* D18F0 - Fabric Configuration registers */
#define D18F0_MMIO_BASE0		0x200
#define D18F0_MMIO_LIMIT0		0x204
#define   D18F0_MMIO_SHIFT		16
#define D18F0_MMIO_CTRL0		0x208

#if CONFIG(SOC_AMD_REMBRANDT)
#define DF_MMIO_REG_SET_SIZE		3
#else
#define DF_MMIO_REG_SET_SIZE		4
#endif

#define DF_MMIO_REG_SET_COUNT		8

#define DF_FICAA_BIOS			0x5C
#define DF_FICAD_LO			0x98
#define DF_FICAD_HI			0x9C

#define IOMS0_FABRIC_ID			9

union df_mmio_control {
	struct {
		uint32_t re        :  1; /* [ 0.. 0] */
		uint32_t we        :  1; /* [ 1.. 1] */
		uint32_t           :  2; /* [ 3.. 2] */
		uint32_t fabric_id :  4; /* [ 7.. 4] */
		uint32_t           :  8; /* [15.. 8] */
		uint32_t np        :  1; /* [16..16] */
		uint32_t           : 15; /* [31..17] */
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


#endif /* AMD_MENDOCINO_DATA_FABRIC_H */
