/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <fsp/api.h>
#include <gpio.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();

	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = variant_memory_sku(),
	};

	memcfg_init(&memupd->FspmConfig, mem_config, &spd_info, half_populated);
}
