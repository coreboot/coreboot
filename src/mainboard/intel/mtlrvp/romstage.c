/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/intel/board_id.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	int board_id = get_rvp_board_id();
	const bool half_populated = false;

	const struct mem_spd dimm_module_spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = {
				.addr_dimm[0] = 0x50,
				.addr_dimm[1] = 0x0,
			},
			[1] = {
				.addr_dimm[0] = 0x50,
				.addr_dimm[1] = 0x0,
			},
			[2] = {
				.addr_dimm[0] = 0x52,
				.addr_dimm[1] = 0x0,
			},
			[3] = {
				.addr_dimm[0] = 0x52,
				.addr_dimm[1] = 0x0,
			},
		},
	};

	switch (board_id) {
	case MTLP_DDR5_RVP:
		memcfg_init(memupd, mem_config, &dimm_module_spd_info, half_populated);
		break;
	default:
		die("Unknown board id = 0x%x\n", board_id);
		break;
	}
}
