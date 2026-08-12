/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <types.h>

static const struct cnl_mb_cfg memcfg = {
	.spd = {
		[0] = { READ_SPD_CBFS, { .spd_index = 0 } },
		[2] = { READ_SPD_CBFS, { .spd_index = 0 } },
	},
	.rcomp_resistor = {121, 81, 100},
	.rcomp_targets = {100, 40, 20, 20, 26},
	.dq_pins_interleaved = 0,
	.vref_ca_config = 2,
	.ect = true,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const uint8_t vtd = get_uint_option("vtd", 1);
	memupd->FspmTestConfig.VtdDisable = !vtd;

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
