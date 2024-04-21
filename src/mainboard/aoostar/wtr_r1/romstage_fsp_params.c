/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

#include "gpio.h"

static const struct mb_cfg ddr4_mem_config = {
	.type = MEM_TYPE_DDR4,
	/* According to DOC #573387 rcomp values no longer have to be provided */
	/* DDR DIMM configuration does not need to set DQ/DQS maps */

	.ect = true, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,

	.LpDdrDqDqsReTraining = 1,

	.ddr_config = {
		.dq_pins_interleaved = false,
	},
};

static const struct mem_spd dimm_module_spd_info = {
	.topo = MEM_TOPO_DIMM_MODULE,
	.smbus[0] = { .addr_dimm[0] = 0x50, },
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	/*
	 * Alder Lake common meminit block driver considers bus width to be 128-bit and
	 * populates the meminit data accordingly. Alder Lake-N has single memory controller
	 * with 64-bit bus width. By setting half_populated to true, only the bottom half is
	 * populated.
	 * TODO: Implement __weak variant_is_half_populated(void) function.
	 */
	const bool half_populated = true;

	memcfg_init(memupd, &ddr4_mem_config, &dimm_module_spd_info, half_populated);

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}
