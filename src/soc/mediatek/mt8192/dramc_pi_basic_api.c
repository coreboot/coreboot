/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/gpio.h>

static const u8 mrr_o1_pinmux_mapping[PINMUX_MAX][CHANNEL_MAX][DQ_DATA_WIDTH] = {
	[PINMUX_DSC] = {
		[CHANNEL_A] = {0, 1, 7, 6, 4, 5, 2, 3, 9, 8, 11, 10, 14, 15, 13, 12},
		[CHANNEL_B] = {1, 0, 5, 6, 3, 2, 7, 4, 8, 9, 11, 10, 12, 14, 13, 15},
	},
	[PINMUX_LPBK] = {
	},
	[PINMUX_EMCP] = {
		[CHANNEL_A] = {1, 0, 3, 2, 4, 7, 6, 5, 8, 9, 10, 14, 11, 15, 13, 12},
		[CHANNEL_B] = {0, 1, 4, 7, 3, 5, 6, 2, 9, 8, 10, 12, 11, 14, 13, 15}
	},
};

static void set_rank_info_to_conf(const struct ddr_cali *cali)
{
	u8 value = ((cali->emi_config->cona_val >> 17) & 0x1) ? 0 : 1;

	SET32_BITFIELDS(&ch[0].ao.sa_reserve,
		SA_RESERVE_MODE_RK0, cali->cbt_mode[RANK_0],
		SA_RESERVE_MODE_RK1, cali->cbt_mode[RANK_1],
		SA_RESERVE_SINGLE_RANK, value);
}

static void get_dram_pinmux_sel(struct ddr_cali *cali)
{
	u32 value = (read32(&mtk_gpio->dram_pinmux_trapping) >> 19) & 0x1;

	if (value)
		cali->pinmux_type = PINMUX_DSC;
	else
		cali->pinmux_type = PINMUX_EMCP;
}

static void set_mrr_pinmux_mapping(const struct ddr_cali *cali)
{
	const u8 *map;
	u32 bc_bak = dramc_get_broadcast();

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		map = mrr_o1_pinmux_mapping[get_pinmux_type(cali)][chn];
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux1,
			MRR_BIT_MUX1_MRR_BIT0_SEL, map[0],
			MRR_BIT_MUX1_MRR_BIT1_SEL, map[1],
			MRR_BIT_MUX1_MRR_BIT2_SEL, map[2],
			MRR_BIT_MUX1_MRR_BIT3_SEL, map[3]);
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux2,
			MRR_BIT_MUX2_MRR_BIT4_SEL, map[4],
			MRR_BIT_MUX2_MRR_BIT5_SEL, map[5],
			MRR_BIT_MUX2_MRR_BIT6_SEL, map[6],
			MRR_BIT_MUX2_MRR_BIT7_SEL, map[7]);
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux3,
			MRR_BIT_MUX3_MRR_BIT8_SEL, map[8],
			MRR_BIT_MUX3_MRR_BIT9_SEL, map[9],
			MRR_BIT_MUX3_MRR_BIT10_SEL, map[10],
			MRR_BIT_MUX3_MRR_BIT11_SEL, map[11]);
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux4,
			MRR_BIT_MUX4_MRR_BIT12_SEL, map[12],
			MRR_BIT_MUX4_MRR_BIT13_SEL, map[13],
			MRR_BIT_MUX4_MRR_BIT14_SEL, map[14],
			MRR_BIT_MUX4_MRR_BIT15_SEL, map[15]);
	}
	dramc_set_broadcast(bc_bak);
}

static void set_dqo1_pinmux_mapping(const struct ddr_cali *cali)
{
	const u8 *map;
	u32 bc_bak = dramc_get_broadcast();

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		map = mrr_o1_pinmux_mapping[get_pinmux_type(cali)][chn];
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_dq_se_pinmux_ctrl0,
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ0, map[0],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ1, map[1],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ2, map[2],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ3, map[3],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ4, map[4],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ5, map[5],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ6, map[6],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ7, map[7]);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_dq_se_pinmux_ctrl1,
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ8, map[8],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ9, map[9],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ10, map[10],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ11, map[11],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ12, map[12],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ13, map[13],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ14, map[14],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ15, map[15]);
	}

	dramc_set_broadcast(bc_bak);
}

void global_option_init(struct ddr_cali *cali)
{
	set_rank_info_to_conf(cali);
	get_dram_pinmux_sel(cali);
	set_mrr_pinmux_mapping(cali);
	set_dqo1_pinmux_mapping(cali);
}

void dfs_init_for_calibration(const struct ddr_cali *cali)
{
}
