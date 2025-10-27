/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/dsi.h>
#include <soc/pll.h>

void mtk_dsi_configure_mipi_tx(u32 data_rate, u32 lanes, bool is_cphy)
{
	unsigned int txdiv0;
	u64 pcw;

	if (data_rate >= 2000 * MHz) {
		txdiv0 = 0;
	} else if (data_rate >= 1000 * MHz) {
		txdiv0 = 1;
	} else if (data_rate >= 500 * MHz) {
		txdiv0 = 2;
	} else if (data_rate > 250 * MHz) {
		/* (data_rate == 250MHz) is a special case that should go to the
		   else-block below (txdiv0 = 4) */
		txdiv0 = 3;
	} else {
		/* MIN = 125 */
		assert(data_rate >= MTK_DSI_DATA_RATE_MIN_MHZ * MHz);
		txdiv0 = 4;
	}

	if (CONFIG(MEDIATEK_DSI_CPHY) && is_cphy)
		mtk_dsi_cphy_lane_sel_setting();

	clrbits32(&mipi_tx->pll_con4, BIT(11) | BIT(10));
	setbits32(&mipi_tx->pll_pwr, AD_DSI_PLL_SDM_PWR_ON);
	udelay(30);
	clrbits32(&mipi_tx->pll_pwr, AD_DSI_PLL_SDM_ISO_EN);

	pcw = (u64)data_rate * (1 << txdiv0);
	pcw <<= 24;
	pcw /= CLK26M_HZ;

	write32(&mipi_tx->pll_con0, pcw);
	clrsetbits32(&mipi_tx->pll_con1, RG_DSI_PLL_POSDIV, txdiv0 << 8);
	udelay(30);
	setbits32(&mipi_tx->pll_con1, RG_DSI_PLL_EN);

	/* BG_LPF_EN / BG_CORE_EN */
	write32(&mipi_tx->lane_con, 0x3FFF0180);
	udelay(40);
	write32(&mipi_tx->lane_con, 0x3FFF00C0);

	if (CONFIG(MEDIATEK_DSI_CPHY) && is_cphy)
		mtk_dsi_cphy_enable();

	/* Switch OFF each Lane */
	clrbits32(&mipi_tx->d0_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->d1_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->d2_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->d3_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->ck_sw_ctl_en, DSI_SW_CTL_EN);

	if (CONFIG(MEDIATEK_DSI_CPHY) && is_cphy)
		mtk_dsi_cphy_disable_ck_mode();
	else
		mtk_dsi_dphy_disable_ck_mode();
}

void mtk_dsi_reset(void)
{
	write32(&dsi0->dsi_force_commit,
		DSI_FORCE_COMMIT_USE_MMSYS | DSI_FORCE_COMMIT_ALWAYS);
	write32(&dsi0->dsi_con_ctrl, 1);
	write32(&dsi0->dsi_con_ctrl, 0);
}
