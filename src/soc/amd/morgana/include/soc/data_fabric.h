/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Update for Morgana */

#ifndef AMD_MORGANA_DATA_FABRIC_H
#define AMD_MORGANA_DATA_FABRIC_H

#include <types.h>

/* SoC-specific bits in D18F0_MMIO_CTRL0 */
#define   DF_MMIO_NP			BIT(3)

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

#endif /* AMD_MORGANA_DATA_FABRIC_H */
