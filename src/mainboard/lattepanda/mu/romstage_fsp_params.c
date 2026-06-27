/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/api.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <baseboard/variants.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	const bool half_populated = true;

	const struct mem_spd memory_down_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = 0,
	};

	memcfg_init(memupd, mem_config, &memory_down_spd_info, half_populated);
}
