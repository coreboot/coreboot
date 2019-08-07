/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <edid.h>
#include <soc/dsi.h>
#include <timer.h>

int mtk_dsi_phy_clk_setting(u32 bits_per_pixel, u32 lanes,
			    const struct edid *edid)
{
	u32 txdiv0, txdiv1;
	u64 pcw;
	u32 reg;
	int i, data_rate, mipi_tx_rate;

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

	clrsetbits_le32(&mipi_tx0->dsi_top_con, RG_DSI_LNT_IMP_CAL_CODE,
			8 << 4 | RG_DSI_LNT_HS_BIAS_EN);

	setbits_le32(&mipi_tx0->dsi_con,
		     RG_DSI0_CKG_LDOOUT_EN | RG_DSI0_LDOCORE_EN);

	clrsetbits_le32(&mipi_tx0->dsi_pll_pwr, RG_DSI_MPPLL_SDM_ISO_EN,
			RG_DSI_MPPLL_SDM_PWR_ON);

	clrbits_le32(&mipi_tx0->dsi_pll_con0, RG_DSI0_MPPLL_PLL_EN);

	/**
	 * data_rate = pixel_clock / 1000 * bits_per_pixel * mipi_ratio / lanes
	 * pixel_clock unit is Khz, data_rata unit is MHz, so need divide 1000.
	 * mipi_ratio is mipi clk coefficient for balance the pixel clk in mipi.
	 * we set mipi_ratio is 1.02.
	 */
	data_rate = edid->mode.pixel_clock * 102 * bits_per_pixel /
			(lanes * 1000 * 100);
	mipi_tx_rate = data_rate;

	if (data_rate > 500) {
		txdiv0 = 0;
		txdiv1 = 0;
	} else if (data_rate >= 250) {
		txdiv0 = 1;
		txdiv1 = 0;
	} else if (data_rate >= 125) {
		txdiv0 = 2;
		txdiv1 = 0;
	} else if (data_rate >= 62) {
		txdiv0 = 2;
		txdiv1 = 1;
	} else if (data_rate >= 50) {
		txdiv0 = 2;
		txdiv1 = 2;
	} else {
		printk(BIOS_ERR, "data rate (%u) must be >=50. "
		       "Please check pixel clock (%u), bits per pixel (%u), "
		       "and number of lanes (%u)\n",
		       data_rate, edid->mode.pixel_clock, bits_per_pixel,
		       lanes);
		return -1;
	}

	clrsetbits_le32(&mipi_tx0->dsi_pll_con0,
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
	pcw /= 13;
	write32(&mipi_tx0->dsi_pll_con2, pcw);

	setbits_le32(&mipi_tx0->dsi_pll_con1, RG_DSI0_MPPLL_SDM_FRA_EN);

	setbits_le32(&mipi_tx0->dsi_clock_lane, LDOOUT_EN);

	for (i = 0; i < lanes; i++)
		setbits_le32(&mipi_tx0->dsi_data_lane[i], LDOOUT_EN);

	setbits_le32(&mipi_tx0->dsi_pll_con0, RG_DSI0_MPPLL_PLL_EN);

	udelay(40);

	clrbits_le32(&mipi_tx0->dsi_pll_con1, RG_DSI0_MPPLL_SDM_SSC_EN);
	clrbits_le32(&mipi_tx0->dsi_top_con, RG_DSI_PAD_TIE_LOW_EN);

	return mipi_tx_rate;
}

void mtk_dsi_reset(void)
{
	setbits_le32(&dsi0->dsi_con_ctrl, 3);
	clrbits_le32(&dsi0->dsi_con_ctrl, 1);
}

void mtk_dsi_pin_drv_ctrl(void)
{
	struct stopwatch sw;
	uint32_t pwr_ack;

	setbits_le32(&lvds_tx1->vopll_ctl3, RG_DA_LVDSTX_PWR_ON);

	stopwatch_init_usecs_expire(&sw, 1000);

	do {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_ERR, "enable lvdstx_power failed!!!\n");
			return;
		}
		pwr_ack = read32(&lvds_tx1->vopll_ctl3) & RG_AD_LVDSTX_PWR_ACK;
	} while (pwr_ack == 0);

	clrbits_le32(&lvds_tx1->vopll_ctl3, RG_DA_LVDS_ISO_EN);
}
