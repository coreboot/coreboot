/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

#define SMBUS_ADDR_DIMM	0x50

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  10,  11,  8,  9,  15,  14,  12,  13, },
			.dq1 = {  7,  4,  5,  6,  3,  0,  2,  1 },
		},
		.ddr1 = {
			.dq0 = {  8,  11,  10,  9,  15,  12,  13,  14, },
			.dq1 = {  6,  7,  5,  4,  1,  3,  0,  2 },
		},
		.ddr2 = {
			.dq0 = {  3,  2,  1,  0,  6,  7,  5,  4, },
			.dq1 = {  9,  8,  10,  11,  15,  12,  13,  14 },
		},
		.ddr3 = {
			.dq0 = {  8,  11,  10,  9,  15,  13,  12,  14, },
			.dq1 = {  4,  5,  6,  7,  1,  3,  2,  0 },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.ect = true, /* Early Command Training */

	.lp_ddr_dq_dqs_re_training = 1,

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &lp5_mem_config;
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = 0;
}
