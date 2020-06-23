/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/regulator.h>

static void set_vcore_voltage(const struct ddr_cali *cali)
{
	u32 vcore = get_vcore_value(cali);

	dramc_info("Set DRAM vcore voltage to %u\n", vcore);
	mainboard_set_regulator_vol(MTK_REGULATOR_VCORE, vcore);
}

static void dramc_calibration_all_channels(struct ddr_cali *cali)
{
}

void init_dram(const struct dramc_data *dparam)
{
	u32 bc_bak;
	u8 k_shuffle, k_shuffle_end;
	u8 pll_mode = 0;
	bool first_freq_k = true;

	struct ddr_cali cali = {0};
	struct mr_values mr_value;
	const struct ddr_base_info *ddr_info = &dparam->ddr_info;

	cali.pll_mode = &pll_mode;
	cali.mr_value = &mr_value;
	cali.support_ranks = ddr_info->support_ranks;
	cali.cbt_mode[RANK_0] = ddr_info->cbt_mode[RANK_0];
	cali.cbt_mode[RANK_1] = ddr_info->cbt_mode[RANK_1];
	cali.emi_config = &ddr_info->emi_config;

	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	global_option_init(&cali);
	bc_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	dramc_set_broadcast(bc_bak);

	if (ddr_info->config_dvfs == DRAMC_ENABLE_DVFS)
		k_shuffle_end = CALI_SEQ_MAX;
	else
		k_shuffle_end = CALI_SEQ1;

	for (k_shuffle = CALI_SEQ0; k_shuffle < k_shuffle_end; k_shuffle++) {
		set_cali_datas(&cali, dparam, k_shuffle);
		set_vcore_voltage(&cali);
		dfs_init_for_calibration(&cali);

		dramc_calibration_all_channels(&cali);

		first_freq_k = false;
	}
}
