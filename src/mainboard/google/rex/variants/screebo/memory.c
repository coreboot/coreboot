/* SPDX-License-Identifier: GPL-3.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP5X,

	.rcomp = {
		/* Baseboard uses only 100ohm Rcomp resistors */
		.resistor = 100,
	},

	/* DQ byte map */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  9, 13, 12,  14, 8, 10, 11, 15 },
			.dq1 = {  6,  5,  7,   4, 3,  2,  0,  1 },
		},
		.ddr1 = {
			.dq0 = {  2,  0,  1,  3,  4,  5,  6,  7 },
			.dq1 = { 14,  8, 12, 15, 11, 10,  9, 13 },
		},
		.ddr2 = {
			.dq0 = {  9, 11, 10,  8, 15, 13, 14, 12 },
			.dq1 = {  0,  6,  2,  3,  1,  4,  5,  7 },
		},
		.ddr3 = {
			.dq0 = {  7,  2,  0,  3,  4,  1,  5,  6 },
			.dq1 = {  8, 14, 12, 13, 11, 15,  9, 10 },
		},
		.ddr4 = {
			.dq0 = { 14,  9, 12, 15, 11, 10,  8, 13 },
			.dq1 = {  5,  7,  4,  6,  2,  1,  0,  3 },
		},
		.ddr5 = {
			.dq0 = {  1,  5,  3,  6,  2,  7,  0,  4 },
			.dq1 = { 10,  8, 11,  9, 12, 15, 14, 13 },
		},
		.ddr6 = {
			.dq0 = { 10,  8,  9, 11, 13, 14, 15, 12 },
			.dq1 = {  3,  7,  2,  0,  1,  4,  6,  5 },
		},
		.ddr7 = {
			.dq0 = {  1,  2,  3,  0,  5,  7,  4,  6 },
			.dq1 = { 14,  8, 12, 13, 15, 11, 10,  9 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.lp5x_config = {
		.ccc_config = 0xaa,
	},

	.ect = 1, /* Early Command Training */
};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool variant_is_half_populated(void)
{
	return gpio_get(GPIO_MEM_CH_SEL);
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
