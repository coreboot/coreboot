/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  1,  0,  3,  2,  7,  4,  5,  6, },
			.dq1 = { 13, 12, 14, 15, 11, 10,  8,  9, },
		},
		.ddr1 = {
			.dq0 = { 11,  8, 10,  9, 15, 12, 13, 14, },
			.dq1 = {  6,  7,  5,  4,  0,  1,  3,  2, },
		},
		.ddr2 = {
			.dq0 = {  1,  0,  3,  2,  4,  7,  5,  6, },
			.dq1 = { 13, 12, 14, 15, 10, 11,  8,  9, },
		},
		.ddr3 = {
			.dq0 = {  8, 10,  9, 11, 12, 13, 14, 15, },
			.dq1 = {  6,  5,  4,  7,  3,  2,  1,  0, },
		},
		.ddr4 = {
			.dq0 = {  5,  6,  4,  7,  3,  2,  1,  0, },
			.dq1 = {  8,  9, 10, 11, 14, 15, 13, 12, },
		},
		.ddr5 = {
			.dq0 = { 12, 15, 13, 14,  9, 11, 10,  8, },
			.dq1 = {  3,  2,  1,  0,  4,  7,  6,  5, },
		},
		.ddr6 = {
			.dq0 = {  7,  3,  2,  0,  6,  1,  4,  5, },
			.dq1 = { 10, 14, 15, 11, 13, 12,  9,  8, },
		},
		.ddr7 = {
			.dq0 = {  8, 12, 11,  9, 10, 15, 14, 13, },
			.dq1 = {  5,  4,  0,  1,  6,  3,  2,  7, },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1, },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0, },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1, },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0, },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1, },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0, },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1, },
		.ddr7 = { .dqs0 = 1, .dqs1 = 0, }
	},

	.ect = true, /* Early Command Training */

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};

const struct mb_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
{
	return 0;
}

bool __weak variant_is_half_populated(void)
{
	return 0;
}

void __weak variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
