/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_PHOENIX_DATA_FABRIC_H
#define AMD_PHOENIX_DATA_FABRIC_H

#include <types.h>

/* D18F0 - Fabric Configuration registers */
#define D18F0_MMIO_BASE0		0xD80
#define D18F0_MMIO_LIMIT0		0xD84
#define   D18F0_MMIO_SHIFT		16
#define D18F0_MMIO_CTRL0		0xD88

#define DF_FICAA_BIOS			0x8C
#define DF_FICAD_LO			0xB8
#define DF_FICAD_HI			0xBC

#define IOMS0_FABRIC_ID			14

#define NUM_NB_MMIO_REGS		8

union df_mmio_control {
	struct {
		uint32_t re        :  1; /* [ 0.. 0] */
		uint32_t we        :  1; /* [ 1.. 1] */
		uint32_t           :  1; /* [ 2.. 2] */
		uint32_t np        :  1; /* [ 3.. 3] */
		uint32_t           : 12; /* [15.. 4] */
		uint32_t fabric_id :  6; /* [21..16] */
		uint32_t           : 10; /* [31..22] */
	};
	uint32_t raw;
};

union df_ficaa {
	struct {
		uint32_t cfg_inst_acc_en :  1; /* [ 0.. 0] */
		uint32_t reg_num         : 10; /* [10.. 1] */
		uint32_t func_num        :  3; /* [13..11] */
		uint32_t b64_en          :  1; /* [14..14] */
		uint32_t                 :  1; /* [15..15] */
		uint32_t inst_id         :  8; /* [23..16] */
		uint32_t                 :  8; /* [31..24] */
	};
	uint32_t raw;
};

#endif /* AMD_PHOENIX_DATA_FABRIC_H */
