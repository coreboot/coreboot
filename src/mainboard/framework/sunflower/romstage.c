/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg board_cfg = {
		.type = MEM_TYPE_DDR5,
		.ect = true,
		.UserBd = BOARD_TYPE_MOBILE,
		.ddr_config = {
			.dq_pins_interleaved = false,
		},
	};

	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = { .addr_dimm[0] = 0x50, },
		},
	};

	/* Single SODIMM slot on one physical channel */
	const bool half_populated = true;

	memcfg_init(mupd, &board_cfg, &spd_info, half_populated);
}
