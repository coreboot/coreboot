/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>

void enable_dfs_hw_mode_clk(void)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_dvfsctl3,
			MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_DESTI, 0x3,
			MISC_DVFSCTL3_RG_DVFS_MEM_CK_SEL_SOURCE, 0x1);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_clk_ctrl,
			MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN, 0x1,
			MISC_CLK_CTRL_DVFS_CLK_MEM_SEL, 0x1,
			MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL_MODE, 0x0,
			MISC_CLK_CTRL_DVFS_MEM_CK_MUX_SEL, 0x1);
	}
}

void dramc_dfs_direct_jump_rg_mode(const struct ddr_cali *cali, u8 shu_level)
{
	u8 shu_ack = 0;
	u8 tmp_level;
	u8 pll_mode = *(cali->pll_mode);
	u32 *shu_ack_reg = &mtk_dpm->status_4;

	if (pll_mode == PHYPLL_MODE) {
		dramc_dbg("Disable CLRPLL\n");
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
			SET32_BITFIELDS(&ch[chn].phy_ao.clrpll0, CLRPLL0_RG_RCLRPLL_EN, 0);
	} else {
		dramc_dbg("Disable PHYPLL\n");
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
			SET32_BITFIELDS(&ch[chn].phy_ao.phypll0, PHYPLL0_RG_RPHYPLL_EN, 0);
	}

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		shu_ack |= (0x1 << chn);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
			MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN, 1);

	if (shu_level == DRAM_DFS_SHU0)
		tmp_level = shu_level;
	else
		tmp_level = 1;

	if (pll_mode == PHYPLL_MODE) {
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
			SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
				MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN, 0);
			SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
				MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL, tmp_level);
			SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
				MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN, 1);
		}
		dramc_dbg("Enable CLRPLL\n");
	} else {
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
			SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
				MISC_RG_DFS_CTRL_RG_PHYPLL2_SHU_EN, 0);
			SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
				MISC_RG_DFS_CTRL_RG_DR_SHU_LEVEL, tmp_level);
			SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
				MISC_RG_DFS_CTRL_RG_PHYPLL_SHU_EN, 1);
		}
		dramc_dbg("Enable PHYPLL\n");
	}
	udelay(1);

	if (pll_mode == PHYPLL_MODE)
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
			SET32_BITFIELDS(&ch[chn].phy_ao.clrpll0, CLRPLL0_RG_RCLRPLL_EN, 1);
	else
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
			SET32_BITFIELDS(&ch[chn].phy_ao.phypll0, PHYPLL0_RG_RPHYPLL_EN, 1);

	udelay(20);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
			MISC_RG_DFS_CTRL_RG_DR_SHU_EN, 1);
	while ((READ32_BITFIELD(shu_ack_reg, LPIF_STATUS_4_SHU_EN_ACK) & shu_ack) != shu_ack)
		dramc_dbg("Waiting shu_en ack\n");

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
			MISC_RG_DFS_CTRL_RG_DR_SHU_EN, 0);

	if (pll_mode == PHYPLL_MODE)
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
			SET32_BITFIELDS(&ch[chn].phy_ao.phypll0, PHYPLL0_RG_RPHYPLL_EN, 0);
	else
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
			SET32_BITFIELDS(&ch[chn].phy_ao.clrpll0, CLRPLL0_RG_RCLRPLL_EN, 0);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_rg_dfs_ctrl,
			MISC_RG_DFS_CTRL_RG_DDRPHY_FB_CK_EN, 0);

	dramc_dbg("Shuffle flow completed\n");

	pll_mode = !pll_mode;
	*(cali->pll_mode) = pll_mode;
}
