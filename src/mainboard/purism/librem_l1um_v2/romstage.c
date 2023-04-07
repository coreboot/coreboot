/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <spd_cache.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg memcfg = {
		/* Access memory info through SMBUS. */
		.spd[0] = {
			.read_type = READ_SMBUS,
			.spd_spec = {.spd_smbus_address = 0xa0}
		},
		.spd[1] = {
			.read_type = READ_SMBUS,
			.spd_spec = {.spd_smbus_address = 0xa2}
		},
		.spd[2] = {
			.read_type = READ_SMBUS,
			.spd_spec = {.spd_smbus_address = 0xa4}
		},
		.spd[3] = {
			.read_type = READ_SMBUS,
			.spd_spec = {.spd_smbus_address = 0xa6}
		},

		/* Rcomp resistors on CFL-S are located on the CPU itself */
		.rcomp_resistor = {121, 75, 100},

		/* Rcomp target values for CFL-S, DDR4 and 2 DIMMs per channel */
		.rcomp_targets = {60, 26, 20, 20, 26},

		/* Baseboard is an interleaved design */
		.dq_pins_interleaved = 1,

		/* Baseboard is using config 2 for vref_ca */
		.vref_ca_config = 2,

		/* Disable Early Command Training */
		.ect = 0,
	};

	memupd->FspmConfig.UserBd = BOARD_TYPE_SERVER;
	/*
	 * Disabling SendDidMsg is necessary because the IFD for this board
	 * sets the HAP bit.  Otherwise, attempting to shut down causes a global
	 * reset, and occasionally FSP-S freezes during boot.
	 */
	memupd->FspmTestConfig.SendDidMsg = 0;
	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
