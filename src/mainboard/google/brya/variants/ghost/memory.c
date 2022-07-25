/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>

static const struct mb_cfg baseboard_memcfg = {
	.type = MEM_TYPE_LP5X,

	/*
	 * DQ byte map
	 *
	 * To calculate from schematics, reference
	 * ADL_LP5_DqMapCpu2Dram sheet of this spreadsheet:
	 * https://cdrdv2.intel.com/v1/dl/getContent/573387
	 */
	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  5,  0,  4,  1,  2,  6,  7,  3 },
			.dq1 = {  11, 15, 13, 12, 10, 14,  8, 9 },
		},
		.ddr1 = {
			.dq0 = {  9, 10, 11,  8, 13, 14, 12, 15 },
			.dq1 = {  0,  2,  1,  3,  7,  5,  6,  4 },
		},
		.ddr2 = {
			.dq0 = {  3,  7,  2,  6,  4,  1,  5,  0 },
			.dq1 = {  12, 14, 15, 13, 11, 10,  8,  9 },
		},
		.ddr3 = {
			.dq0 = {  15, 14, 12, 13,  10,  9, 11, 8 },
			.dq1 = {  7,  6,  4,  5,  0,  3,  1,  2 },
		},
		.ddr4 = {
			.dq0 = {  15, 14, 12, 13, 10,  9,  8, 11 },
			.dq1 = {  1,  3,  0,  2,  5,  6,  7,  4 },
		},
		.ddr5 = {
			.dq0 = {  9, 10, 11,  8, 12, 15, 13, 14 },
			.dq1 = {  3,  7,  2,  6,  0,  4,  5,  1 },
		},
		.ddr6 = {
			.dq0 = { 11,  8, 10,  9, 12, 14, 13, 15 },
			.dq1 = {  0,  7,  1,  2,  6,  4,  3,  5 },
		},
		.ddr7 = {
			.dq0 = {  1,  2,  3,  0,  7,  5,  6,  4 },
			.dq1 = {  15, 14, 11,  13, 8, 9,  12, 10 },
		},
	},

	/*
	 * DQS CPU<>DRAM map
	 *
	 * To calculate from schematics, reference
	 * MTL_RPL_ADL_LP5_DqsMapCpu2Dram sheet of this spreadsheet:
	 * https://cdrdv2.intel.com/v1/dl/getContent/573387
	 */
	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr5 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 },
	},

	/* Enable Early Command Training */
	.ect = true,

	.UserBd = BOARD_TYPE_MOBILE,

	.lp5x_config = {
		/*
		 * CA and CS signals are in descending order.
		 *
		 * Reference the MTL_RPL_ADL_LP5_CccConfig sheet of
		 * this spreadsheet for instructions:
		 * https://cdrdv2.intel.com/v1/dl/getContent/573387
		 */
		.ccc_config = 0xff,
	},
};

const struct mb_cfg *variant_memory_params(void)
{
	return &baseboard_memcfg;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * - MEM_STRAP_0: GPP_E3
	 * - MEM_STRAP_1: GPP_E2
	 * - MEM_STRAP_2: GPP_E1
	 * - MEM_STRAP_3: GPP_E7
	 *
	 * MEM_STRAP_0 is LSB, and MEM_STRAP_3 is MSB.
	 */
	gpio_t spd_gpios[] = {
		GPP_E3,
		GPP_E2,
		GPP_E1,
		GPP_E7,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

bool variant_is_half_populated(void)
{
	/* GPIO_MEM_CH_SEL is GPP_B3 */
	return gpio_get(GPP_B3);
}
