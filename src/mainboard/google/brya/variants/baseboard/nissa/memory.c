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
			.dq0 = { 12,  9, 10, 11, 14, 13,  8, 15 },
			.dq1 = {  3,  1,  2,  0,  4,  7,  5,  6 },
		},
		.ddr1 = {
			.dq0 = {  3,  1,  2,  0,  4,  7,  5,  6 },
			.dq1 = { 13,  9,  8, 11, 10, 14, 15, 12 },
		},
		.ddr2 = {
			.dq0 = {  2,  1,  3,  0,  4,  6,  5,  7 },
			.dq1 = {  8,  9, 10, 11, 13, 14, 12, 15 },
		},
		.ddr3 = {
			.dq0 = {  3,  0,  1,  2,  5,  6,  4,  7 },
			.dq1 = { 13,  9, 11,  8, 14, 15, 10, 12 },
		},
		.ddr4 = {
			.dq0 = { 12,  9, 10, 11, 14, 13,  8, 15 },
			.dq1 = {  3,  1,  2,  0,  4,  7,  5,  6 },
		},
		.ddr5 = {
			.dq0 = {  3,  1,  2,  0,  4,  7,  5,  6 },
			.dq1 = { 13,  9,  8, 11, 10, 14, 15, 12 },
		},
		.ddr6 = {
			.dq0 = {  2,  1,  3,  0,  4,  6,  5,  7 },
			.dq1 = {  8,  9, 10, 11, 13, 14, 12, 15 },
		},
		.ddr7 = {
			.dq0 = {  3,  0,  1,  2,  5,  6,  4,  7 },
			.dq1 = { 13,  9, 11,  8, 14, 15, 10, 12 },
		},
	},

	/* DQS CPU<>DRAM map */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.lp5x_config = {
		.ccc_config = 0xff,
	},

	.ect = 1, /* Early Command Training */

	.UserBd = BOARD_TYPE_MOBILE,
};

const struct mb_cfg *__weak variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int __weak variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * GPIO_MEM_CONFIG_0	GPP_E1
	 * GPIO_MEM_CONFIG_1	GPP_E2
	 * GPIO_MEM_CONFIG_2	GPP_E3
	 */
	gpio_t spd_gpios[] = {
		GPP_E1,
		GPP_E2,
		GPP_E3,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool __weak variant_is_half_populated(void)
{
	/*
	 * Ideally half_populated is used in platforms with multiple channels to
	 * enable only one half of the channel. Alder Lake N has single channel,
	 * and it would require for new structures to be defined in meminit block
	 * driver for LPx memory configurations. In order to avoid adding new
	 * structures, set half_populated to true. This has the same effect as
	 * having single channel with 64-bit width.
	*/
	return true;
}

void __weak variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
