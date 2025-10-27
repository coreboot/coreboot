/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/dsi.h>
#include <timer.h>

void mtk_dsi_configure_mipi_tx(u32 data_rate, u32 lanes, bool is_cphy)
{
	u32 txdiv0, txdiv1;
	u64 pcw;
	u32 reg;
	int i;

	reg = read32(&mipi_tx0->dsi_bg_con);

	reg = (reg & (~RG_DSI_V02_SEL)) | (4 << 20);
	reg = (reg & (~RG_DSI_V032_SEL)) | (4 << 17);
	reg = (reg & (~RG_DSI_V04_SEL)) | (4 << 14);
	reg = (reg & (~RG_DSI_V072_SEL)) | (4 << 11);
	reg = (reg & (~RG_DSI_V10_SEL)) | (4 << 8);
	reg = (reg & (~RG_DSI_V12_SEL)) | (4 << 5);
	reg |= RG_DSI_BG_CKEN;
	reg |= RG_DSI_BG_CORE_EN;
	write32(&mipi_tx0->dsi_bg_con, reg);
	udelay(30);

	clrsetbits32(&mipi_tx0->dsi_top_con, RG_DSI_LNT_IMP_CAL_CODE,
		     8 << 4 | RG_DSI_LNT_HS_BIAS_EN);

	setbits32(&mipi_tx0->dsi_con,
		  RG_DSI0_CKG_LDOOUT_EN | RG_DSI0_LDOCORE_EN);

	clrsetbits32(&mipi_tx0->dsi_pll_pwr, RG_DSI_MPPLL_SDM_ISO_EN,
		     RG_DSI_MPPLL_SDM_PWR_ON);

	clrbits32(&mipi_tx0->dsi_pll_con0, RG_DSI0_MPPLL_PLL_EN);

	if (data_rate > 500 * MHz) {
		txdiv0 = 0;
		txdiv1 = 0;
	} else if (data_rate >= 250 * MHz) {
		txdiv0 = 1;
		txdiv1 = 0;
	} else if (data_rate >= 125 * MHz) {
		txdiv0 = 2;
		txdiv1 = 0;
	} else if (data_rate >= 62 * MHz) {
		txdiv0 = 2;
		txdiv1 = 1;
	} else {
		/* MIN = 50 */
		assert(data_rate >= MTK_DSI_DATA_RATE_MIN_MHZ * MHz);
		txdiv0 = 2;
		txdiv1 = 2;
	}

	clrsetbits32(&mipi_tx0->dsi_pll_con0,
		     RG_DSI0_MPPLL_TXDIV1 | RG_DSI0_MPPLL_TXDIV0 |
		     RG_DSI0_MPPLL_PREDIV, txdiv1 << 5 | txdiv0 << 3);

	/**
	 * PLL PCW config
	 * PCW bit 24~30 = integer part of pcw
	 * PCW bit 0~23 = fractional part of pcw
	 * pcw = data_Rate*4*txdiv/(Ref_clk*2);
	 * Post DIV =4, so need data_Rate*4
	 * Ref_clk is 26MHz
	 */
	pcw = (u64)(data_rate * (1 << txdiv0) * (1 << txdiv1)) << 24;
	pcw /= 13 * MHz;
	write32(&mipi_tx0->dsi_pll_con2, pcw);

	setbits32(&mipi_tx0->dsi_pll_con1, RG_DSI0_MPPLL_SDM_FRA_EN);

	setbits32(&mipi_tx0->dsi_clock_lane, LDOOUT_EN);

	for (i = 0; i < lanes; i++)
		setbits32(&mipi_tx0->dsi_data_lane[i], LDOOUT_EN);

	setbits32(&mipi_tx0->dsi_pll_con0, RG_DSI0_MPPLL_PLL_EN);

	udelay(40);

	clrbits32(&mipi_tx0->dsi_pll_con1, RG_DSI0_MPPLL_SDM_SSC_EN);
	clrbits32(&mipi_tx0->dsi_top_con, RG_DSI_PAD_TIE_LOW_EN);
}

void mtk_dsi_reset(void)
{
	setbits32(&dsi0->dsi_con_ctrl, 3);
	clrbits32(&dsi0->dsi_con_ctrl, 1);
}

void mtk_dsi_override_phy_timing(struct mtk_phy_timing *timing)
{
	int lpx = 5;
	timing->lpx = lpx;
	timing->da_hs_prepare = 6;
	timing->da_hs_zero = 10;
	timing->da_hs_trail = 8;

	timing->ta_go = 4 * lpx;
	timing->ta_sure = 3 * lpx / 2;
	timing->ta_get = 5 * lpx;
	timing->da_hs_exit = 7;

	timing->da_hs_sync = 0;
	timing->clk_hs_exit = 2 * lpx;

	timing->d_phy = 12;
}

void mtk_dsi_pin_drv_ctrl(void)
{
	struct stopwatch sw;
	uint32_t pwr_ack;

	setbits32(&lvds_tx1->vopll_ctl3, RG_DA_LVDSTX_PWR_ON);

	stopwatch_init_usecs_expire(&sw, 1000);

	do {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "enable lvdstx_power failed!!!\n");
			return;
		}
		pwr_ack = read32(&lvds_tx1->vopll_ctl3) & RG_AD_LVDSTX_PWR_ACK;
	} while (pwr_ack == 0);

	clrbits32(&lvds_tx1->vopll_ctl3, RG_DA_LVDS_ISO_EN);
}
