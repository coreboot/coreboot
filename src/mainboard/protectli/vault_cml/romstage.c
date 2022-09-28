/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>

static const struct cnl_mb_cfg board_memcfg_cfg = {
	/* Access memory info through SMBUS. */
	.spd[0] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xA0}
	},
	.spd[1] = {
		.read_type = NOT_EXISTING,
	},
	.spd[2] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xA4}
	},
	.spd[3] = {
		.read_type = NOT_EXISTING,
	},

	/* Baseboard uses 121, 81 and 100 rcomp resistors */
	.rcomp_resistor = {121, 81, 100},

	/*
	 * Baseboard Rcomp target values.
	 */
	.rcomp_targets = {100, 40, 20, 20, 26},

	/* Baseboard is an interleaved design */
	.dq_pins_interleaved = 1,

	/* Baseboard is using config 2 for vref_ca */
	.vref_ca_config = 2,

	/* Enable Early Command Training */
	.ect = 1,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	cannonlake_memcfg_init(&memupd->FspmConfig, &board_memcfg_cfg);
}
