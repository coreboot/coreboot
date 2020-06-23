/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/infracfg.h>

struct dfs_frequency_table {
	u32 frequency;
	dram_freq_grp freq_group;
	dram_div_mode div_mode;
	dram_dfs_shu shuffle_saved;
	vref_cali_mode vref_cali;
	u32 vcore;
};

static const struct dfs_frequency_table freq_shuffle_table[DRAM_DFS_SHU_MAX] = {
	[CALI_SEQ0] = {800, DDRFREQ_800, DIV8_MODE, DRAM_DFS_SHU4, VREF_CALI_ON, 650000},
	[CALI_SEQ1] = {1200, DDRFREQ_1200, DIV8_MODE, DRAM_DFS_SHU2, VREF_CALI_ON, 662500},
	[CALI_SEQ2] = {600, DDRFREQ_600, DIV8_MODE, DRAM_DFS_SHU5, VREF_CALI_OFF, 650000},
	[CALI_SEQ3] = {933, DDRFREQ_933, DIV8_MODE, DRAM_DFS_SHU3, VREF_CALI_OFF, 662500},
	[CALI_SEQ4] = {400, DDRFREQ_400, DIV4_MODE, DRAM_DFS_SHU6, VREF_CALI_OFF, 650000},
	[CALI_SEQ5] = {2133, DDRFREQ_2133, DIV8_MODE, DRAM_DFS_SHU0, VREF_CALI_ON, 725000},
	[CALI_SEQ6] = {1600, DDRFREQ_1600, DIV8_MODE, DRAM_DFS_SHU1, VREF_CALI_OFF, 687500},
};

void dramc_set_broadcast(u32 onoff)
{
	write32(&mt8192_infracfg->dramc_wbr, onoff);
}

u32 dramc_get_broadcast(void)
{
	return read32(&mt8192_infracfg->dramc_wbr);
}

u8 get_fsp(const struct ddr_cali *cali)
{
	return cali->fsp;
}

dram_div_mode get_div_mode(const struct ddr_cali *cali)
{
	return cali->div_mode;
}

dram_freq_grp get_freq_group(const struct ddr_cali *cali)
{
	return cali->freq_group;
}

dbi_mode get_write_dbi(const struct ddr_cali *cali)
{
	return cali->w_dbi[get_fsp(cali)];
}

dram_odt_state get_odt_state(const struct ddr_cali *cali)
{
	return cali->odt_onoff;
}

dram_dfs_shu get_shu(const struct ddr_cali *cali)
{
	return cali->shu;
}

dram_cbt_mode get_cbt_mode(const struct ddr_cali *cali)
{
	return cali->cbt_mode[cali->rank];
}

u32 get_vcore_value(const struct ddr_cali *cali)
{
	return cali->vcore_voltage;
}

u32 get_frequency(const struct ddr_cali *cali)
{
	return cali->frequency;
}

vref_cali_mode get_vref_cali(const struct ddr_cali *cali)
{
	return cali->vref_cali;
}

dram_pinmux_type get_pinmux_type(const struct ddr_cali *cali)
{
	return cali->pinmux_type;
}

u8 get_mck2ui_div_shift(const struct ddr_cali *cali)
{
	if (get_div_mode(cali) == DIV4_MODE)
		return 2;
	else
		return 3;
}

dram_dfs_shu get_shu_save_by_k_shu(dram_cali_seq k_shu)
{
	return freq_shuffle_table[k_shu].shuffle_saved;
}

dram_freq_grp get_freq_group_by_shu_save(dram_dfs_shu shu)
{
	for (u8 k_shu = CALI_SEQ0; k_shu < CALI_SEQ_MAX; k_shu++)
		if (freq_shuffle_table[k_shu].shuffle_saved == shu)
			return freq_shuffle_table[k_shu].freq_group;

	dramc_err("Invalid shuffle: %d\n", shu);
	return DDRFREQ_800;
}

u32 get_frequency_by_shu(dram_dfs_shu shu)
{
	for (u8 k_shu = CALI_SEQ0; k_shu < CALI_SEQ_MAX; k_shu++)
		if (freq_shuffle_table[k_shu].shuffle_saved == shu)
			return freq_shuffle_table[k_shu].frequency;

	dramc_err("Invalid shuffle: %d, using k_shu = %d\n", shu, CALI_SEQ0);
	return freq_shuffle_table[CALI_SEQ0].frequency;
}

dram_freq_grp get_highest_freq_group(void)
{
	dram_freq_grp highest_freq = DDRFREQ_800;

	for (u8 k_shu = CALI_SEQ0; k_shu < CALI_SEQ_MAX; k_shu++)
		if (freq_shuffle_table[k_shu].freq_group > highest_freq)
			highest_freq = freq_shuffle_table[k_shu].freq_group;

	dramc_dbg("Highest freq is: %d\n", highest_freq);
	return highest_freq;
}

void set_cali_datas(struct ddr_cali *cali, const struct dramc_data *dparam, dram_cali_seq k_shu)
{
	const struct dfs_frequency_table *shuffle = &freq_shuffle_table[k_shu];

	cali->chn = CHANNEL_A;
	cali->rank = RANK_0;
	cali->fsp = (shuffle->freq_group < DDRFREQ_1600) ? FSP_0 : FSP_1;
	cali->w_dbi[FSP_0] = DBI_OFF;
	cali->w_dbi[FSP_1] = DBI_ON;
	cali->frequency = shuffle->frequency;
	cali->freq_group = shuffle->freq_group;
	cali->div_mode = shuffle->div_mode;
	cali->shu = shuffle->shuffle_saved;
	cali->vref_cali = shuffle->vref_cali;
	cali->vcore_voltage = shuffle->vcore;
	cali->odt_onoff = (cali->fsp == FSP_0) ? ODT_OFF : ODT_ON;
	cali->params = &dparam->freq_params[cali->shu];

	dramc_dbg("cali data (size: %ld) fsp: %d, freq_group: %d, div_mode: %d, "
		  "shu: %d, vref_cali: %d, odt_onoff: %d, vcore: %d\n",
		  sizeof(*cali), cali->fsp, cali->freq_group, cali->div_mode, cali->shu,
		  cali->vref_cali, cali->odt_onoff, cali->vcore_voltage);
}

void dramc_auto_refresh_switch(u8 chn, bool enable)
{
	SET32_BITFIELDS(&ch[chn].ao.refctrl0, REFCTRL0_REFDIS, enable ? 0 : 1);

	if (!enable)
		udelay(READ32_BITFIELD(&ch[chn].nao.misc_statusa,
		       MISC_STATUSA_REFRESH_QUEUE_CNT));
}
