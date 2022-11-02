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
			.dq1 = { 14, 12, 13, 15, 11,  8, 10,  9 },
		},
		.ddr1 = {
			.dq0 = {  1,  0,  2,  3,  6,  4,  7,  5 },
			.dq1 = { 15, 13, 12, 14,  8, 11, 10,  9 },
		},
		.ddr2 = {
			.dq0 = {  2,  1,  3,  0,  7,  6,  4,  5 },
			.dq1 = { 14, 12, 13, 15,  8,  9, 10, 11 },
		},
		.ddr3 = {
			.dq0 = {  1,  2,  3,  0,  6,  4,  7,  5 },
			.dq1 = { 13, 15, 12, 14,  8,  9, 10, 11 },
		},
		.ddr4 = {
			.dq0 = {  2,  3,  0,  1,  6,  5,  7,  4 },
			.dq1 = { 14, 12, 13, 15,  8, 10,  9, 11 },
		},
		.ddr5 = {
			.dq0 = {  1,  0,  3,  2,  6,  4,  7,  5 },
			.dq1 = { 15, 13, 12, 14, 11,  8, 10,  9 },
		},
		.ddr6 = {
			.dq0 = {  0,  3,  1,  2,  4,  5,  7,  6 },
			.dq1 = { 15, 13, 14, 12,  9, 10,  8, 11 },
		},
		.ddr7 = {
			.dq0 = {  0,  3,  2,  1,  5,  6,  7,  4 },
			.dq1 = { 14, 10, 13, 12,  8, 11, 15,  9 },
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
	 * GPIO_MEM_CONFIG_0	GPP_E11
	 * GPIO_MEM_CONFIG_1	GPP_E2
	 * GPIO_MEM_CONFIG_2	GPP_E1
	 */
	gpio_t spd_gpios[] = {
		GPP_E11,
		GPP_E2,
		GPP_E1,
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
	/* GPIO_MEM_CH_SEL GPP_E13 */
	return gpio_get(GPP_E13);
}

void __weak variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}
