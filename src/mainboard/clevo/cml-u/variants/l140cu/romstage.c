/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>

static const struct cnl_mb_cfg memcfg = {
	.spd[0] = {
		.read_type = READ_SPD_CBFS,
		.spd_spec = {.spd_index = 0},
	},
	.spd[2] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa4},
	},

	/* Values of the resistors connected to the DDR_RCOMP_[2:0] pins of the CPU */
	.rcomp_resistor = { 121, 81, 100 },

	/* Rcomp target values */
	.rcomp_targets = { 100, 40, 20, 20, 26 },

	/* Interleaved ("back-to-back") pin mapping */
	.dq_pins_interleaved = 1,

	/*
	 * DDR4 Vref mapping
	 * VREF_CA goes to CH_A and VREF_DQ_B (DDR1_VREF_DQ) goes to CH_B.
	 */
	.vref_ca_config = 2,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
