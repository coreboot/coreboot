/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP4X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,

		/* Baseboard Rcomp target values */
		.targets = {40, 30, 30, 30, 30},
	},

	/* DQ byte map as per doc #573387 */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  3,  0,  2,  1,  4,  6,  5,  7, },
			.dq1 = {  12, 13, 14, 15, 8,  9,  10, 11, },
		},
		.ddr1 = {
			.dq0 = { 13, 14, 11, 12,  10, 8, 15,  9, },
			.dq1 = {  5,  2,  4,  3,  1,  6,  0,  7, },
		},
		.ddr2 = {
			.dq0 = {  2,  3,  1,  0,  7,  6,  5,  4, },
			.dq1 = { 12,  13, 14, 15, 8,  9, 10, 11, },
		},
		.ddr3 = {
			.dq0 = { 13, 14, 12, 15, 11,  9,  8, 10, },
			.dq1 = {  5,  2,  1,  4,  7,  0,  3,  6, },
		},
		.ddr4 = {
			.dq0 = { 11, 10,  8,  9, 14, 15, 13, 12, },
			.dq1 = {  3,  0,  2,  1,  5,  4,  6,  7, },
		},
		.ddr5 = {
			.dq0 = { 11, 15, 13, 12, 10,  9, 14,  8, },
			.dq1 = {  3,  0,  2,  1,  6,  7,  5,  4, },
		},
		.ddr6 = {
			.dq0 = { 11, 13, 10, 12, 15,  9, 14,  8, },
			.dq1 = {  4,  3,  5,  2,  7,  0,  1,  6, },
		},
		.ddr7 = {
			.dq0 = { 12, 13, 15, 14, 11,  9, 10,  8, },
			.dq1 = {  4,  5,  1,  2,  6,  3,  0,  7, },
		},
	},

	/* DQS CPU<>DRAM map as per doc #573387 */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.LpDdrDqDqsReTraining = 1,

	.ect = 1, /* Enable Early Command Training */
};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * GPIO_MEM_CONFIG_0	GPP_F16
	 * GPIO_MEM_CONFIG_1	GPP_F12
	 * GPIO_MEM_CONFIG_2	GPP_F13
	 * GPIO_MEM_CONFIG_3	GPP_F15
	 */
	gpio_t spd_gpios[] = {
		GPP_F16,
		GPP_F12,
		GPP_F13,
		GPP_F15,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool variant_is_half_populated(void)
{
	/* GPIO_MEM_CH_SEL GPP_F11 */
	return gpio_get(GPP_F11);
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
