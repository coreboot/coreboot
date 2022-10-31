/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_DATA_FABRIC_H
#define AMD_CEZANNE_DATA_FABRIC_H

#include <types.h>

/* SoC-specific bits in D18F0_MMIO_CTRL0 */
#define   DF_MMIO_NP			BIT(16)

#define IOMS0_FABRIC_ID			10

#define NUM_NB_MMIO_REGS		8

union df_mmio_control {
	struct {
		uint32_t re        :  1; /* [ 0.. 0] */
		uint32_t we        :  1; /* [ 1.. 1] */
		uint32_t           :  2; /* [ 3.. 2] */
		uint32_t fabric_id : 10; /* [13.. 4] */
		uint32_t           :  2; /* [15..14] */
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

#endif /* AMD_CEZANNE_DATA_FABRIC_H */
