/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  0,  2,  1,  3, 5,  4,  7, 6, },
			.dq1 = { 12, 14, 15, 13, 9, 11, 10, 8, },
		},
		.ddr1 = {
			.dq0 = { 11, 9, 8, 10, 12, 13, 14, 15, },
			.dq1 = {  6, 5, 7,  4,  0,  3,  1,  2, },
		},
		.ddr2 = {
			.dq0 = {  0,  2,  1,  3, 5,  7,  4, 6, },
			.dq1 = { 13, 14, 15, 12, 9, 11, 10, 8, },
		},
		.ddr3 = {
			.dq0 = { 10, 8, 9, 11, 15, 12, 13, 14, },
			.dq1 = {  7, 5, 6,  4,  1,  3,  0,  2, },
		},
		.ddr4 = {
			.dq0 = {  4, 6, 5,  7,  3,  1,  2,  0, },
			.dq1 = { 10, 8, 9, 11, 15, 13, 12, 14, },
		},
		.ddr5 = {
			.dq0 = { 15, 13, 12, 14, 11, 9, 10, 8, },
			.dq1 = {  1,  2,  3,  0,  5, 4,  7, 6, },
		},
		.ddr6 = {
			.dq0 = {  4, 6, 5,  7,  1,  3,  0,  2, },
			.dq1 = { 11, 8, 9, 10, 12, 14, 15, 13, },
		},
		.ddr7 = {
			.dq0 = { 12, 13, 15, 14, 9, 11, 8, 10, },
			.dq1 = {  2,  3,  0,  1, 5,  4, 7,  6, },
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
		.ccc_config = 0x55,
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
	 * GPIO_MEM_CONFIG_0	GPP_B02
	 * GPIO_MEM_CONFIG_1	GPP_B03
	 * GPIO_MEM_CONFIG_2	GPP_B04
	 * GPIO_MEM_CONFIG_3    GPP_B05
	 */
	gpio_t spd_gpios[] = {
		GPP_B02,
		GPP_B03,
		GPP_B04,
		GPP_B05,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}

bool variant_is_half_populated(void)
{
	/* GPIO_MEM_CH_SEL GPP_C07 */
	return gpio_get(GPP_C07);
}
