/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/romstage.h>

static const struct mb_cfg ddr4_mem_config = {
	.type = MEM_TYPE_DDR4,

	.rcomp = {
		.resistor = 100,
		.targets = {50, 20, 25, 25, 25},
	},

	.LpDdrDqDqsReTraining = 1,

	.ect = 1,

	.UserBd = BOARD_TYPE_MOBILE,

	.ddr_config = {
		.dq_pins_interleaved = 0,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &ddr4_mem_config;
}

bool variant_is_half_populated(void)
{
	return false;
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_DIMM_MODULE;
	spd_info->smbus[0].addr_dimm[0] = 0x50;
	spd_info->smbus[0].addr_dimm[1] = 0x51;
	spd_info->smbus[1].addr_dimm[0] = 0x52;
	spd_info->smbus[1].addr_dimm[1] = 0x53;
}
