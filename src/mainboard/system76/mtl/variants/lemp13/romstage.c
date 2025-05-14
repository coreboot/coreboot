/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

static size_t get_spd_index(void)
{
	// BOARD_ID1 is high if 5600 MT/s and low if 4800 MT/s
	if (gpio_get(GPP_E11)) {
		return 1;
	} else {
		return 0;
	}
}

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR5,
		.ect = true,
		.LpDdrDqDqsReTraining = 1,
	};
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_MIXED,
		.cbfs_index = get_spd_index(),
		.smbus[1] = { .addr_dimm[0] = 0x52, },
	};
	const bool half_populated = false;

	mupd->FspmConfig.DmiMaxLinkSpeed = 4;
	mupd->FspmConfig.GpioOverride = 0;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
