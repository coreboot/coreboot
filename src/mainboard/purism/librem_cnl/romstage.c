/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include "variant.h"

static const struct cnl_mb_cfg memcfg = {

	/* Parameters required to access SPD for CH0D0/CH0D1/CH1D0/CH1D1. */
	.spd[0] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa0},
	},
	.spd[1] = {.read_type = NOT_EXISTING},
	.spd[2] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa4},
	},
	.spd[3] = {.read_type = NOT_EXISTING},

	/*
	 * Rcomp resistor values.  These values represent the resistance in
	 * ohms of the three rcomp resistors attached to the DDR_COMP_0,
	 * DDR_COMP_1, and DDR_COMP_2 pins on the DRAM.
	 */
	.rcomp_resistor = { 121, 81, 100 },

	/* Rcomp target values */
	.rcomp_targets = { 100, 40, 20, 20, 26 },

	/*
	 * Indicates whether memory is interleaved.
	 * Set to 1 for an interleaved design,
	 * set to 0 for non-interleaved design.
	 */
	.dq_pins_interleaved = 1,

	/*
	 * VREF_CA configuration.
	 * Set to 0 VREF_CA goes to both CH_A and CH_B,
	 * set to 1 VREF_CA goes to CH_A and VREF_DQ_A goes to CH_B,
	 * set to 2 VREF_CA goes to CH_A and VREF_DQ_B goes to CH_B.
	 */
	.vref_ca_config = 2,

	/* Early Command Training */
	.ect = 0,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;
	cannonlake_memcfg_init(mem_cfg, &memcfg);
	variant_memory_init_params(mem_cfg);
}
