/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

#define SMBUS_ADDR_DIMM	0x50

static const struct mb_cfg ddr5_mem_config = {
	.type = MEM_TYPE_DDR5,

	.ect = true, /* Early Command Training */

	.user_bd = BOARD_TYPE_ULT_ULX,

	.ddr_config = {
		.dq_pins_interleaved = false,
	},
};
const struct mb_cfg *variant_memory_params(void)
{
	return &ddr5_mem_config;
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_DIMM_MODULE;
	spd_info->smbus[0].addr_dimm[0] = SMBUS_ADDR_DIMM;
	spd_info->smbus[1].addr_dimm[0] = SMBUS_ADDR_DIMM;
}
