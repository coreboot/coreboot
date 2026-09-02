/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <soc/romstage.h>
#include <soc/meminit.h>

static const struct mb_cfg lp5_mem_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  10,  11,  8,  9, 13,  15,  14,  12, },
			.dq1 = {  7,  4,  5,  6,  3,  2,  0,  1 },
		},
		.ddr1 = {
			.dq0 = {  8,  11,  10,  9,  15,  12,  13,  14, },
			.dq1 = {  6,  7,  5,  4,  1,  3,  0,  2 },
		},
		.ddr2 = {
			.dq0 = {  3,  2,  1,  0,  6,  7,  5,  4 },
			.dq1 = {  9,  8,  10,  11,  15,  12,  13,  14, },
		},
		.ddr3 = {
			.dq0 = {  8,  11,  10,  9,  15,  13,  12,  14, },
			.dq1 = {  4,  5,  6,  7,  1,  3,  2,  0 },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 1, .dqs1 = 0 },
	},

	.ect = true, /* Early Command Training */

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};

static const struct mb_cfg lp5_mem_lga_config = {
	.type = MEM_TYPE_LP5X,

	.lpx_dq_map = {
		.ddr0 = {
			.dq0 = {  10,  11,  8,  9,  13,  15,  14,  12, },
			.dq1 = {  7,  4,  5,  6,  3,  2,  0,  1 },
		},
		.ddr1 = {
			.dq0 = {  0,  3,  2,  1,  5,  4,  6,  7, },
			.dq1 = {  13,  14,  9,  8,  15,  11,  12,  10 },
		},
		.ddr2 = {
			.dq0 = {  3,  2,  1,  0,  6,  7,  5,  4 },
			.dq1 = {  9,  8,  10,  11,  15,  12,  13,  14, },
		},
		.ddr3 = {
			.dq0 = {  0,  3,  2,  1,  5,  6,  4,  7, },
			.dq1 = {  8,  9,  13,  14,  15,  11,  10,  12 },
		},
	},

	.lpx_dqs_map = {
		.ddr0 = { .dqs0 = 1, .dqs1 = 0 },
		.ddr1 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr2 = { .dqs0 = 0, .dqs1 = 1 },
		.ddr3 = { .dqs0 = 0, .dqs1 = 1 },
	},

	.ect = true, /* Early Command Training */

	.user_bd = BOARD_TYPE_ULT_ULX,

	.lp5x_config = {
		.ccc_config = 0xFF,
	},
};


const struct mb_cfg *variant_memory_params(void)
{
	/*
	 * GPP_D08 Low -> use LGA RAM
	 * GPP_D08 High  -> use board RAM
	 */
	if (gpio_get(GPP_D08)) {
		return &lp5_mem_config;
	}
	return &lp5_mem_lga_config;
}

int variant_memory_sku(void)
{
	/*
	 * Memory configuration board straps
	 * HW_ID0	GPP_E20
	 * HW_ID1	GPP_E19
	 * HW_ID2	GPP_D06
	 * HW_ID3	GPP_D05
	 */
	gpio_t spd_gpios[] = {
		GPP_E20,
		GPP_E19,
		GPP_D06,
		GPP_D05,
	};

	return gpio_base2_value(spd_gpios, ARRAY_SIZE(spd_gpios));
}

void variant_get_spd_info(struct mem_spd *spd_info)
{
	spd_info->topo = MEM_TOPO_MEMORY_DOWN;
	spd_info->cbfs_index = variant_memory_sku();
}

void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	/*
	 * GPP_D08 High -> keep baseboard settings (up to 7467 MT/s)
	 * GPP_D08 Low  -> limit max DRAM speed to 6400 MT/s
	 */
	if (gpio_get(GPP_D08))
		return;

	/*
	 * Override FSP-M SaGv frequency and gear to cap DRAM speed at
	 * 6400 MT/s. This hook runs after the baseboard devicetree settings
	 * have been applied to FSP-M, so every value written below
	 * overwrites the baseboard default.
	 */
	m_cfg->SaGvFreq[0] = 2400;
	m_cfg->SaGvGear[0] = GEAR_4;

	m_cfg->SaGvFreq[1] = 3200;
	m_cfg->SaGvGear[1] = GEAR_4;

	m_cfg->SaGvFreq[2] = 6000;
	m_cfg->SaGvGear[2] = GEAR_4;

	m_cfg->SaGvFreq[3] = 6400;
	m_cfg->SaGvGear[3] = GEAR_4;

	/* Override baseboard max_dram_speed_mts (7467) as well */
	m_cfg->DdrFreqLimit = 6400;
}
