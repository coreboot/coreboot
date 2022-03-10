/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/util.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR4,
	};
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_MIXED,
		.cbfs_index = 0,
		.smbus = {
			[1] = { .addr_dimm[0] = 0x52, },
		},
	};
	const bool half_populated = false;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
