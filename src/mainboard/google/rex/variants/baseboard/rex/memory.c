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
			.dq0 = {  7,  1,  2,  0,  3,  6,  5,  4 },
			.dq1 = { 13, 12, 14, 15,  8,  9, 10, 11 },
		},
		.ddr1 = {
			.dq0 = { 10,  9, 11,  8, 15, 13, 14, 12 },
			.dq1 = {  6,  0,  7,  4,  3,  1,  2,  5 },
		},
		.ddr2 = {
			.dq0 = { 11, 10,  8,  9, 12, 15, 14, 13 },
			.dq1 = {  7,  0,  6,  5,  3,  2,  1,  4 },
		},
		.ddr3 = {
			.dq0 = { 13, 10,  8,  9, 14, 11, 15, 12 },
			.dq1 = {  1,  6,  4,  7,  0,  5,  2,  3 },
		},
		.ddr4 = {
			.dq0 = { 10, 11,  9, 12, 13, 14, 15,  8 },
			.dq1 = {  6,  7,  4,  5,  1,  0,  3,  2 },
		},
		.ddr5 = {
			.dq0 = {  0,  5,  3,  6,  1,  4,  2,  7 },
			.dq1 = {  8, 11, 10,  9, 15, 14, 13, 12 },
		},
		.ddr6 = {
			.dq0 = {  1,  3,  0,  2,  6,  5,  7,  4 },
			.dq1 = { 13, 15, 14, 12, 11, 10,  8,  9 },
		},
		.ddr7 = {
			.dq0 = { 10,  9, 11,  8, 12, 15, 14, 13 },
			.dq1 = {  6,  4,  7,  5,  2,  1,  0,  3 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.lp5x_config = {
		.ccc_config = 0x66,
	},

	.ect = 1, /* Early Command Training */
};

const struct mb_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
{
	gpio_t spd_gpios[] = {
		GPIO_MEM_CONFIG_0,
		GPIO_MEM_CONFIG_1,
		GPIO_MEM_CONFIG_2,
		GPIO_MEM_CONFIG_3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool __weak variant_is_half_populated(void)
{
	return gpio_get(GPIO_MEM_CH_SEL);
}

void __weak variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
