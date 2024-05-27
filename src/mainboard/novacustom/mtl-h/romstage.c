/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const bool half_populated = false;

	static const struct mb_cfg mem_config = {
		.type = MEM_TYPE_DDR5,

		.ect = true, /* Early Command Training */

		.UserBd = BOARD_TYPE_ULT_ULX,
	};
	const struct mem_spd dimm_module_spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,

		.smbus = {
			[0] = {
				.addr_dimm[0] = 0x50,
			},
			[1] = {
				.addr_dimm[0] = 0x52,
			},
		},
	};

	memcfg_init(mupd, &mem_config, &dimm_module_spd_info, half_populated);
}
