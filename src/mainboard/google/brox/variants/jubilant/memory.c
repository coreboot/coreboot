/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP5X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  0,  3,  2,  1,  6,  4,  5,  7 },
			.dq1 = { 14, 12, 13, 15, 8,  11, 9,  10 },
		},
		.ddr1 = {
			.dq0 = {  1,  0,  2,  3,  6,  4,  7,  5 },
			.dq1 = { 11,  8, 10,  9, 15, 14, 13, 12 },
		},
		.ddr2 = {
			.dq0 = {  6,  4,  7,  5,  3,  1,  2,  0 },
			.dq1 = { 14, 12, 13, 15,  9, 10, 11,  8 },
		},
		.ddr3 = {
			.dq0 = {  1,  2,  3,  0,  6,  4,  7,  5 },
			.dq1 = { 13, 15, 12, 14,  8,  11, 10, 9 },
		},
		.ddr4 = {
			.dq0 = {  2,  3,  0,  1,  6,  5,  7,  4 },
			.dq1 = { 14, 15, 13, 12, 10,  8,  9, 11 },
		},
		.ddr5 = {
			.dq0 = {  1,  2,  3,  0,  6,  4,  7,  5 },
			.dq1 = { 15, 13, 12, 14, 10,  9,  8, 11 },
		},
		.ddr6 = {
			.dq0 = {  2,  1,  3,  0,  7,  5,  4,  6 },
			.dq1 = { 15, 13, 14, 12, 11,  9, 10,  8 },
		},
		.ddr7 = {
			.dq0 = {  3,  1,  2,  0,  5,  4,  7,  6 },
			.dq1 = { 14, 15,  9, 11,  13, 8, 10, 12 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.lp5x_config = {
		.ccc_config = 0xff,
	},

	.LpDdrDqDqsReTraining = 1,

	.ect = 1, /* Early Command Training */

	.UserBd = BOARD_TYPE_ULT_ULX,
};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * MEM_STRAP_0	GPP_E15
	 * MEM_STRAP_1	GPP_E12
	 * MEM_STRAP_2	GPP_E13
	 * MEM_STRAP_3	GPP_E10
	 */
	gpio_t spd_gpios[] = {
		GPP_E15,
		GPP_E12,
		GPP_E13,
		GPP_E10,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool variant_is_half_populated(void)
{
	/* MEM_CH_SEL GPP_S7 */
	return gpio_get(GPP_S7);
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
