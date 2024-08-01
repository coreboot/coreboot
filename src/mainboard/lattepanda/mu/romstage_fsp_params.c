/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <soc/meminit.h>
#include <baseboard/variants.h>

static size_t get_spd_index(void)
{
	size_t spd_index = 0;
	printk(BIOS_INFO, "SPD index is 0x%x\n", (unsigned int)spd_index);
	return spd_index;
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	const bool half_populated = (true);

	const struct mem_spd memory_down_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = get_spd_index(),
	};

	memcfg_init(memupd, mem_config, &memory_down_spd_info, half_populated);
}
