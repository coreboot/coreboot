/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  8,  10,  9,  11, 15,  12,  13,  14, },
			.dq1 = {  6,  5,  7,  4,  3,  1,  0,  2 },
		},
		.ddr1 = {
			.dq0 = {  0,  1,  2,  3,  4,  7,  5,  6, },
			.dq1 = {  14,  13,  12,  15,  8,  9,  11,  10 },
		},
		.ddr2 = {
			.dq0 = {  10,  9,  11,  8,  15,  12,  14,  13 },
			.dq1 = {  6,  5,  4,  7,  1,  3,  0,  2, },
		},
		.ddr3 = {
			.dq0 = {  5,  7,  4,  6,  3,  1,  0,  2, },
			.dq1 = {  13,  14,  11,  12,  9,  8,  15,  10 },
		},
		.ddr4 = {
			.dq0 = {  8,  9,  10,  11,  15,  12,  14,  13 },
			.dq1 = {  6,  5,  4,  7,  2,  1,  3,  0, },
		},
		.ddr5 = {
			.dq0 = {  0,  1,  3,  2,  5,  4,  6,  7, },
			.dq1 = {  9,  8,  11,  10,  13,  15,  14,  12 },
		},
		.ddr6 = {
			.dq0 = {  10,  11,  9,  8,  15,  12,  14, 13, },
			.dq1 = {  6,  5,  7,  4,  2,  3,  0,  1 },
		},
		.ddr7 = {
			.dq0 = {  0,  2,  3,  1,  7,  4,  5,  6, },
			.dq1 = {  15,  14,  13,  12,  10,  11,  9,  8 },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr4 = { .dqs0 = 1, .dqs1 = 0 },
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
	 * MEM_STRAP_0	GPP_A13
	 * MEM_STRAP_3	GPP_B24
	 * DIMM_SEL0	GPP_D25
	 * DIMM_SEL1	GPP_D24
	 * DIMM_SEL2	GPP_B25
	 */
	gpio_t spd_gpios[] = {
		GPP_A13,
		GPP_B24,
		GPP_D25,
		GPP_D24,
		GPP_B25,
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
