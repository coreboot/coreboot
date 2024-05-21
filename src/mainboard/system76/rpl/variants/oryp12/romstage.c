/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR5,
		.ect = true,
		.LpDdrDqDqsReTraining = 1,
		.ddr_config = {
			.dq_pins_interleaved = true,
		},
	};
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
			[1] = { .addr_dimm[0] = 0x52, },
		},
	};
	const bool half_populated = false;

	// Set primary display to hybrid graphics
	mupd->FspmConfig.PrimaryDisplay = 4;

	mupd->FspmConfig.DmiMaxLinkSpeed = 4;
	mupd->FspmConfig.GpioOverride = 0;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
