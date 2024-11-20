/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = { 13, 14, 12, 15, 11, 10,  8,  9 },
			.dq1 = {  7,  5,  4,  6,  0,  3,  1,  2 },
		},
		.ddr1 = {
			.dq0 = {  1,  3,  0,  2,  7,  4,  6,  5 },
			.dq1 = { 12, 13, 14, 15, 11, 10,  9,  8 },
		},
		.ddr2 = {
			.dq0 = {  0,  2,  1,  3,  6,  4,  7,  5 },
			.dq1 = { 14, 13, 15, 12,  8, 11, 10,  9 },
		},
		.ddr3 = {
			.dq0 = {  6,  5,  7,  4,  2,  3,  1,  0 },
			.dq1 = { 10,  8, 11,  9, 12, 15, 13, 14 },
		},
		.ddr4 = {
			.dq0 = {  2,  1,  3,  0,  4,  7,  5,  6 },
			.dq1 = { 15, 14, 12, 13,  9, 11, 10,  8 },
		},
		.ddr5 = {
			.dq0 = {  6,  5,  7,  4,  3,  1,  0,  2 },
			.dq1 = { 10,  9, 11,  8, 13, 14, 12, 15 },
		},
		.ddr6 = {
			.dq0 = {  9, 10, 11,  8, 14, 12, 13, 15 },
			.dq1 = {  0,  1,  2,  3,  5,  7,  4,  6 },
		},
		.ddr7 = {
			.dq0 = {  0,  1,  2,  3,  7,  5,  6,  4 },
			.dq1 = { 14, 13, 15, 12, 10,  8, 11,  9 },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr5 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr6 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr7 = { .dqs0 = 0, .dqs1 = 1 }
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

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * GPIO_MEM_CONFIG_0	GPP_D08
	 * GPIO_MEM_CONFIG_1	GPP_D07
	 * GPIO_MEM_CONFIG_2	NC
	 * GPIO_MEM_CONFIG_3    NC
	 */
	gpio_t spd_gpios[] = {
		GPP_D08,
		GPP_D07,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = 0;
}
