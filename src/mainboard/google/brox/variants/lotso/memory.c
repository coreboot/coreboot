/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP5X,

	/* Leave Rcomp unspecified to use the FSP optimized defaults */

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 13, 15, 14, 12, 11, 9, 10, 8 },
			.dq1 = {  3,  0,  2,  1,  6, 7,  5, 4 },
		},
		.ddr1 = {
			.dq0 = {  2,  0,  1,  3,  6,  4, 7, 5 },
			.dq1 = { 13, 15, 12, 14, 10, 11, 8, 9 },
		},
		.ddr2 = {
			.dq0 = { 14, 13, 12, 15, 9, 10, 11, 8 },
			.dq1 = {  4,  6,  7,  5, 1,  2,  0, 3 },
		},
		.ddr3 = {
			.dq0 = { 14, 13, 15, 12, 8, 11, 9, 10 },
			.dq1 = {  0,  2,  1,  3, 6,  5, 7,  4 },
		},
		.ddr4 = {
			.dq0 = { 8, 11, 10, 9, 14, 15, 13, 12 },
			.dq1 = { 3,  0,  2, 1,  5,  7,  4,  6 },
		},
		.ddr5 = {
			.dq0 = {  2,  1,  3,  0,  6, 4, 7,  5 },
			.dq1 = { 12, 13, 15, 14, 10, 9, 8, 11 },
		},
		.ddr6 = {
			.dq0 = {  1,  0,  3,  2, 5,  7,  6, 4 },
			.dq1 = { 15, 13, 12, 14, 8, 11, 10, 9 },
		},
		.ddr7 = {
			.dq0 = {  3,  2, 1,  0,  7, 4,  5,  6 },
			.dq1 = { 14, 15, 9, 11, 12, 8, 10, 13 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.lp5x_config = {
		.ccc_config = 0xff,
	},

	.LpDdrDqDqsReTraining = 1,

	.ect = 1, /* Early Command Training */
};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * MEM_STRAP_0	GPP_S4
	 * MEM_STRAP_1	GPP_S5
	 * MEM_STRAP_2	GPP_S6
	 * MEM_STRAP_3	GPP_S7
	 */
	gpio_t spd_gpios[] = {
		GPP_S4,
		GPP_S5,
		GPP_S6,
		GPP_S7,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool variant_is_half_populated(void)
{
	/* MEM_CH_SEL GPP_S0 */
	return gpio_get(GPP_S0);
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
