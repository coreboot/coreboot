/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/api.h>
#include <gpio.h>
#include <soc/romstage.h>
#include <spd_bin.h>
#include <soc/meminit.h>
#include <baseboard/variants.h>
#include <baseboard/gpio.h>


static size_t get_spd_index(void)
{
	size_t spd_index = (gpio_get(SPD_IDX_GPIO_0)) |
			   (gpio_get(SPD_IDX_GPIO_1) << 1) |
			   (gpio_get(SPD_IDX_GPIO_2) << 2);

	return spd_index;
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();

	const struct mem_spd memory_down_spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = get_spd_index(),
	};

	/*
	 * Alder Lake common meminit block driver considers bus width to be 128-bit and
	 * populates the meminit data accordingly. Alder Lake-N has single memory controller
	 * with 64-bit bus width. By setting half_populated to true, only the bottom half is
	 * populated. Setting it here hard-coded to true because there is no support for
	 * the upper 64 bit of DRAM at all on Amston Lake (which is based on Alder Lake N).
	 */
	memcfg_init(memupd, mem_config, &memory_down_spd_info, true);
}
