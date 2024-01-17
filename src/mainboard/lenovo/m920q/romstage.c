/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>

static const struct cnl_mb_cfg baseboard_mem_cfg = {
	/* Access memory info through SMBUS. */
	.spd[0] = {.read_type = READ_SMBUS, .spd_spec = {.spd_smbus_address = 0xa0}},
	.spd[1] = {.read_type = READ_SMBUS, .spd_spec = {.spd_smbus_address = 0xa4}},

	/* Rcomp resistors on CFL-S are located on the CPU itself */
	.rcomp_resistor = {121, 75, 100},

	/* Rcomp target values for CFL-S, DDR4 and 1 DIMM per channel */
	.rcomp_targets = {50, 26, 20, 20, 26},

	/* Baseboard is an interleaved design */
	.dq_pins_interleaved = 1,

	/* Baseboard is using config 2 for vref_ca */
	.vref_ca_config = 2,

	/* Disable Early Command Training */
	.ect = 0,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	cannonlake_memcfg_init(&memupd->FspmConfig, &baseboard_mem_cfg);
}
