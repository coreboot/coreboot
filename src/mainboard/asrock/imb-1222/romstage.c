/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>

static const struct cnl_mb_cfg memcfg = {
	.spd[0] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa0},
	},
	.spd[2] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa4},
	},
	.rcomp_resistor = { 121, 75, 100 },
	.rcomp_targets = { 50, 26, 20, 20, 26 },
	.dq_pins_interleaved = 1,
	.vref_ca_config = 2,
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	/* Allow memory clocks higher than 2933 MHz */
	memupd->FspmConfig.SaOcSupport = 1;

	/* Set primary display to PCIe graphics */
	memupd->FspmConfig.PrimaryDisplay = 1;

	cannonlake_memcfg_init(&memupd->FspmConfig, &memcfg);
}
