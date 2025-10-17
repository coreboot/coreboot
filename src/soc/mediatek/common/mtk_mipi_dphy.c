/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <device/mmio.h>
#include <delay.h>
#include <soc/dsi.h>
#include <soc/pll.h>
#include <types.h>

void mtk_dsi_configure_mipi_tx(u32 data_rate, u32 lanes)
{
	unsigned int txdiv0, txdiv1;
	u64 pcw;

	if (data_rate >= 2000 * MHz) {
		txdiv0 = 0;
		txdiv1 = 0;
	} else if (data_rate >= 1000 * MHz) {
		txdiv0 = 1;
		txdiv1 = 0;
	} else if (data_rate >= 500 * MHz) {
		txdiv0 = 2;
		txdiv1 = 0;
	} else if (data_rate > 250 * MHz) {
		/* (data_rate == 250MHz) is a special case that should go to the
		   else-block below (txdiv0 = 4) */
		txdiv0 = 3;
		txdiv1 = 0;
	} else {
		/* MIN = 125 */
		assert(data_rate >= MTK_DSI_DATA_RATE_MIN_MHZ * MHz);
		txdiv0 = 4;
		txdiv1 = 0;
	}

	clrbits32(&mipi_tx->pll_con4, BIT(11) | BIT(10));
	setbits32(&mipi_tx->pll_pwr, AD_DSI_PLL_SDM_PWR_ON);
	udelay(30);
	clrbits32(&mipi_tx->pll_pwr, AD_DSI_PLL_SDM_ISO_EN);

	pcw = (u64)data_rate * (1 << txdiv0) * (1 << txdiv1);
	pcw <<= 24;
	pcw /= CLK26M_HZ;

	write32(&mipi_tx->pll_con0, pcw);
	clrsetbits32(&mipi_tx->pll_con1, RG_DSI_PLL_POSDIV, txdiv0 << 8);
	udelay(30);
	setbits32(&mipi_tx->pll_con1, RG_DSI_PLL_EN);

	/* BG_LPF_EN / BG_CORE_EN */
	write32(&mipi_tx->lane_con, 0x3fff0180);
	udelay(40);
	write32(&mipi_tx->lane_con, 0x3fff00c0);

	/* Switch OFF each Lane */
	clrbits32(&mipi_tx->d0_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->d1_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->d2_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->d3_sw_ctl_en, DSI_SW_CTL_EN);
	clrbits32(&mipi_tx->ck_sw_ctl_en, DSI_SW_CTL_EN);

	setbits32(&mipi_tx->ck_ckmode_en, DSI_CK_CKMODE_EN);
}
