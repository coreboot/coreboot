/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <ec/google/wilco/romstage.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>

static const struct cnl_mb_cfg memcfg = {
	/* Access memory info through SMBUS. */
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
	 * The dqs_map arrays map the ddr4 pins to the SoC pins
	 * for both channels.
	 *
	 * the index = pin number on ddr4 part
	 * the value = pin number on SoC
	 */
	.dqs_map[DDR_CH0] = {0, 1, 4, 5, 2, 3, 6, 7},
	.dqs_map[DDR_CH1] = {0, 1, 4, 5, 2, 3, 6, 7},

	/* Baseboard uses 121, 81 and 100 rcomp resistors */
	.rcomp_resistor = {121, 81, 100},

	/*
	 * Baseboard Rcomp target values.
	 */
	.rcomp_targets = {100, 40, 20, 20, 26},

	/* Disable Early Command Training */
	.ect = 0,

	/* Base on board design */
	.vref_ca_config = 2,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	wilco_ec_romstage_init();

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
