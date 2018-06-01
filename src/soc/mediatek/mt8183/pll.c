/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#include <arch/io.h>
#include <delay.h>
#include <stddef.h>

#include <soc/addressmap.h>
#include <soc/infracfg.h>
#include <soc/mcucfg.h>
#include <soc/pll.h>

enum mux_id {
	TOP_AXI_SEL = 0,
	TOP_MM_SEL,
	TOP_IMG_SEL,
	TOP_CAM_SEL,
	TOP_DSP_SEL,
	TOP_DSP1_SEL,
	TOP_DSP2_SEL,
	TOP_IPU_IF_SEL,
	TOP_MFG_SEL,
	TOP_MFG_52M_SEL,
	TOP_CAMTG_SEL,
	TOP_CAMTG2_SEL,
	TOP_CAMTG3_SEL,
	TOP_CAMTG4_SEL,
	TOP_UART_SEL,
	TOP_SPI_SEL,
	TOP_MSDC50_0_HCLK_SEL,
	TOP_MSDC50_0_SEL,
	TOP_MSDC30_1_SEL,
	TOP_MSDC30_2_SEL,
	TOP_AUDIO_SEL,
	TOP_AUD_INTBUS_SEL,
	TOP_PMICSPI_SEL,
	TOP_PWRAP_ULPOSC_SEL,
	TOP_ATB_SEL,
	TOP_PWRMCU_SEL,
	TOP_DPI0_SEL,
	TOP_SCAM_SEL,
	TOP_DISP_PWM_SEL,
	TOP_USB_TOP_SEL,
	TOP_SSUSB_XHCI_SEL,
	TOP_SPM_SEL,
	TOP_I2C_SEL,
	TOP_SCP_SEL,
	TOP_SENINF_SEL,
	TOP_DXCC_SEL,
	TOP_AUD_ENGEN1_SEL,
	TOP_AUD_ENGEN2_SEL,
	TOP_AES_UFSFDE_SEL,
	TOP_UFS_SEL,
	TOP_AUD_1_SEL,
	TOP_AUD_2_SEL,
	TOP_NR_MUX
};

#define MUX(_id, _reg, _mux_shift, _mux_width, _upd_reg, _upd_shift)	\
	[_id] = {							\
		.reg = &mtk_topckgen->_reg,				\
		.mux_shift = _mux_shift,				\
		.mux_width = _mux_width,				\
		.upd_reg = &mtk_topckgen->_upd_reg,			\
		.upd_shift = _upd_shift,				\
	}

static const struct mux muxes[] = {
	/* CLK_CFG_0 */
	MUX(TOP_AXI_SEL, clk_cfg_0, 0, 2, clk_cfg_update, 0),
	MUX(TOP_MM_SEL, clk_cfg_0, 8, 3, clk_cfg_update, 1),
	MUX(TOP_IMG_SEL, clk_cfg_0, 16, 3, clk_cfg_update, 2),
	MUX(TOP_CAM_SEL, clk_cfg_0, 24, 4, clk_cfg_update, 3),
	/* CLK_CFG_1 */
	MUX(TOP_DSP_SEL, clk_cfg_1, 0, 4, clk_cfg_update, 4),
	MUX(TOP_DSP1_SEL, clk_cfg_1, 8, 4, clk_cfg_update, 5),
	MUX(TOP_DSP2_SEL, clk_cfg_1, 16, 4, clk_cfg_update, 6),
	MUX(TOP_IPU_IF_SEL, clk_cfg_1, 24, 4, clk_cfg_update, 7),
	/* CLK_CFG_2 */
	MUX(TOP_MFG_SEL, clk_cfg_2, 0, 2, clk_cfg_update, 8),
	MUX(TOP_MFG_52M_SEL, clk_cfg_2, 8, 2, clk_cfg_update, 9),
	MUX(TOP_CAMTG_SEL, clk_cfg_2, 16, 3, clk_cfg_update, 10),
	MUX(TOP_CAMTG2_SEL, clk_cfg_2, 24, 3, clk_cfg_update, 11),
	/* CLK_CFG_3 */
	MUX(TOP_CAMTG3_SEL, clk_cfg_3, 0, 3, clk_cfg_update, 12),
	MUX(TOP_CAMTG4_SEL, clk_cfg_3, 8, 3, clk_cfg_update, 13),
	MUX(TOP_UART_SEL, clk_cfg_3, 16, 1, clk_cfg_update, 14),
	MUX(TOP_SPI_SEL, clk_cfg_3, 24, 2, clk_cfg_update, 15),
	/* CLK_CFG_4 */
	MUX(TOP_MSDC50_0_HCLK_SEL, clk_cfg_4, 0, 2, clk_cfg_update, 16),
	MUX(TOP_MSDC50_0_SEL, clk_cfg_4, 8, 3, clk_cfg_update, 17),
	MUX(TOP_MSDC30_1_SEL, clk_cfg_4, 16, 3, clk_cfg_update, 18),
	MUX(TOP_MSDC30_2_SEL, clk_cfg_4, 24, 3, clk_cfg_update, 19),
	/* CLK_CFG_5 */
	MUX(TOP_AUDIO_SEL, clk_cfg_5, 0, 2, clk_cfg_update, 20),
	MUX(TOP_AUD_INTBUS_SEL, clk_cfg_5, 8, 2, clk_cfg_update, 21),
	MUX(TOP_PMICSPI_SEL, clk_cfg_5, 16, 2, clk_cfg_update, 22),
	MUX(TOP_PWRAP_ULPOSC_SEL, clk_cfg_5, 24, 2, clk_cfg_update, 23),
	/* CLK_CFG_6 */
	MUX(TOP_ATB_SEL, clk_cfg_6, 0, 2, clk_cfg_update, 24),
	MUX(TOP_PWRMCU_SEL, clk_cfg_6, 8, 3, clk_cfg_update, 25),
	MUX(TOP_DPI0_SEL, clk_cfg_6, 16, 4, clk_cfg_update, 26),
	MUX(TOP_SCAM_SEL, clk_cfg_6, 24, 1, clk_cfg_update, 27),
	/* CLK_CFG_7 */
	MUX(TOP_DISP_PWM_SEL, clk_cfg_7, 0, 3, clk_cfg_update, 28),
	MUX(TOP_USB_TOP_SEL, clk_cfg_7, 8, 2, clk_cfg_update, 29),
	MUX(TOP_SSUSB_XHCI_SEL, clk_cfg_7, 16, 2, clk_cfg_update, 30),
	MUX(TOP_SPM_SEL, clk_cfg_7, 24, 1, clk_cfg_update1, 0),
	/* CLK_CFG_8 */
	MUX(TOP_I2C_SEL, clk_cfg_8, 0, 2, clk_cfg_update1, 1),
	MUX(TOP_SCP_SEL, clk_cfg_8, 8, 3, clk_cfg_update1, 2),
	MUX(TOP_SENINF_SEL, clk_cfg_8, 16, 2, clk_cfg_update1, 3),
	MUX(TOP_DXCC_SEL, clk_cfg_8, 24, 2, clk_cfg_update1, 4),
	/* CLK_CFG_9 */
	MUX(TOP_AUD_ENGEN1_SEL, clk_cfg_9, 0, 2, clk_cfg_update1, 5),
	MUX(TOP_AUD_ENGEN2_SEL, clk_cfg_9, 8, 2, clk_cfg_update1, 6),
	MUX(TOP_AES_UFSFDE_SEL, clk_cfg_9, 16, 3, clk_cfg_update1, 7),
	MUX(TOP_UFS_SEL, clk_cfg_9, 24, 2, clk_cfg_update1, 8),
	/* CLK_CFG_10 */
	MUX(TOP_AUD_1_SEL, clk_cfg_10, 0, 1, clk_cfg_update1, 9),
	MUX(TOP_AUD_2_SEL, clk_cfg_10, 8, 1, clk_cfg_update1, 10),
};

struct mux_sel {
	enum mux_id id;
	u32 sel;
};

static const struct mux_sel mux_sels[] = {
	/* CLK_CFG_0 */
	{ .id = TOP_AXI_SEL, .sel = 2 },		/* 2: mainpll_d7 */
	{ .id = TOP_MM_SEL, .sel = 1 },			/* 1: mmpll_d7 */
	{ .id = TOP_IMG_SEL, .sel = 1 },		/* 1: mmpll_d6 */
	{ .id = TOP_CAM_SEL, .sel = 1 },		/* 1: mainpll_d2 */
	/* CLK_CFG_1 */
	{ .id = TOP_DSP_SEL, .sel = 1 },		/* 1: mmpll_d6 */
	{ .id = TOP_DSP1_SEL, .sel = 1 },		/* 1: mmpll_d6 */
	{ .id = TOP_DSP2_SEL, .sel = 1 },		/* 1: mmpll_d6 */
	{ .id = TOP_IPU_IF_SEL, .sel = 1 },		/* 1: mmpll_d6 */
	/* CLK_CFG_2 */
	{ .id = TOP_MFG_SEL, .sel = 1 },		/* 1: mfgpll_ck */
	{ .id = TOP_MFG_52M_SEL, .sel = 3 },		/* 3: univpll_d3_d8 */
	{ .id = TOP_CAMTG_SEL, .sel = 1 },		/* 1: univ_192m_d8 */
	{ .id = TOP_CAMTG2_SEL, .sel = 1 },		/* 1: univ_192m_d8 */
	/* CLK_CFG_3 */
	{ .id = TOP_CAMTG3_SEL, .sel = 1 },		/* 1: univ_192m_d8 */
	{ .id = TOP_CAMTG4_SEL, .sel = 1 },		/* 1: univ_192m_d8 */
	{ .id = TOP_UART_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_SPI_SEL, .sel = 1 },		/* 1: mainpll_d5_d2 */
	/* CLK_CFG_4 */
	{ .id = TOP_MSDC50_0_HCLK_SEL, .sel = 1 },	/* 1: mainpll_d2_d2 */
	{ .id = TOP_MSDC50_0_SEL, .sel = 1 },		/* 1: msdcpll_ck */
	{ .id = TOP_MSDC30_1_SEL, .sel = 4 },		/* 4: msdcpll_d2 */
	{ .id = TOP_MSDC30_2_SEL, .sel = 1 },		/* 1: univpll_d3_d2 */
	/* CLK_CFG_5 */
	{ .id = TOP_AUDIO_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_AUD_INTBUS_SEL, .sel = 1 },		/* 1: mainpll_d2_d4 */
	{ .id = TOP_PMICSPI_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_PWRAP_ULPOSC_SEL, .sel = 0 },	/* 0: clk26m */
	/* CLK_CFG_6 */
	{ .id = TOP_ATB_SEL, .sel = 1 },		/* 1: mainpll_d2_d2 */
	{ .id = TOP_PWRMCU_SEL, .sel = 2 },		/* 2: mainpll_d2_d2 */
	{ .id = TOP_DPI0_SEL, .sel = 1 },		/* 1: tvdpll_d2 */
	{ .id = TOP_SCAM_SEL, .sel = 1 },		/* 1: mainpll_d5_d2 */
	/* CLK_CFG_7 */
	{ .id = TOP_DISP_PWM_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_USB_TOP_SEL, .sel = 3 },		/* 3: univpll_d5_d2 */
	{ .id = TOP_SSUSB_XHCI_SEL, .sel = 3 },		/* 3: univpll_d5_d2 */
	{ .id = TOP_SPM_SEL, .sel = 1 },		/* 1: mainpll_d2_d8 */
	/* CLK_CFG_8 */
	{ .id = TOP_I2C_SEL, .sel = 2 },		/* 2: univpll_d5_d2 */
	{ .id = TOP_SCP_SEL, .sel = 1 },		/* 1: univpll_d2_d8 */
	{ .id = TOP_SENINF_SEL, .sel = 1 },		/* 1: univpll_d2_d2 */
	{ .id = TOP_DXCC_SEL, .sel = 1 },		/* 1: mainpll_d2_d2 */
	/* CLK_CFG_9 */
	{ .id = TOP_AUD_ENGEN1_SEL, .sel = 3 },		/* 3: apll1_d8 */
	{ .id = TOP_AUD_ENGEN2_SEL, .sel = 3 },		/* 3: apll2_d8 */
	{ .id = TOP_AES_UFSFDE_SEL, .sel = 3 },		/* 3: mainpll_d3 */
	{ .id = TOP_UFS_SEL, .sel = 1 },		/* 1: mainpll_d2_d4 */
	/* CLK_CFG_10 */
	{ .id = TOP_AUD_1_SEL, .sel = 1 },		/* 1: apll1_ck */
	{ .id = TOP_AUD_2_SEL, .sel = 1 },		/* 1: apll2_ck */
};

#define MMPLL_RSTB_SHIFT	(23)

enum pll_id {
	APMIXED_ARMPLL_LL,
	APMIXED_ARMPLL_L,
	APMIXED_CCIPLL,
	APMIXED_MAINPLL,
	APMIXED_UNIVPLL,
	APMIXED_MSDCPLL,
	APMIXED_MMPLL,
	APMIXED_MFGPLL,
	APMIXED_TVDPLL,
	APMIXED_APLL1,
	APMIXED_APLL2,
	APMIXED_NR_PLL
};

const u32 pll_div_rate[] = {
	3800UL * MHz,
	1248 * MHz,
	624 * MHz,
	384 * MHz,
	200 * MHz,
	0,
};

static const struct pll plls[] = {
	PLL(APMIXED_ARMPLL_LL, armpll_ll_con0, armpll_ll_pwr_con0,
		PLL_RSTB_SHIFT, 22, armpll_ll_con1, 24, armpll_ll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_ARMPLL_L, armpll_l_con0, armpll_l_pwr_con0,
		PLL_RSTB_SHIFT, 22, armpll_l_con1, 24, armpll_l_con1, 0,
		pll_div_rate),
	PLL(APMIXED_CCIPLL, ccipll_con0, ccipll_pwr_con0,
		PLL_RSTB_SHIFT, 22, ccipll_con1, 24, ccipll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MAINPLL, mainpll_con0, mainpll_pwr_con0,
		PLL_RSTB_SHIFT, 22, mainpll_con1, 24, mainpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_UNIVPLL, univpll_con0, univpll_pwr_con0,
		PLL_RSTB_SHIFT, 22, univpll_con1, 24, univpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MSDCPLL, msdcpll_con0, msdcpll_pwr_con0,
		NO_RSTB_SHIFT, 22, msdcpll_con1, 24, msdcpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MMPLL, mmpll_con0, mmpll_pwr_con0,
		MMPLL_RSTB_SHIFT, 22, mmpll_con1, 24, mmpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MFGPLL, mfgpll_con0, mfgpll_pwr_con0,
		NO_RSTB_SHIFT, 22, mfgpll_con1, 24, mfgpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_TVDPLL, tvdpll_con0, tvdpll_pwr_con0,
		NO_RSTB_SHIFT, 22, tvdpll_con1, 24, tvdpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_APLL1, apll1_con0, apll1_pwr_con0,
		NO_RSTB_SHIFT, 32, apll1_con0, 1, apll1_con1, 0,
		pll_div_rate),
	PLL(APMIXED_APLL2, apll2_con0, apll2_pwr_con0,
		NO_RSTB_SHIFT, 32, apll2_con0, 1, apll2_con1, 0,
		pll_div_rate),
};

struct rate {
	enum pll_id id;
	u32 rate;
};

static const struct rate rates[] = {
	{ .id = APMIXED_ARMPLL_LL, .rate = ARMPLL_LL_HZ },
	{ .id = APMIXED_ARMPLL_L, .rate = ARMPLL_L_HZ },
	{ .id = APMIXED_CCIPLL, .rate = CCIPLL_HZ },
	{ .id = APMIXED_MAINPLL, .rate = MAINPLL_HZ },
	{ .id = APMIXED_UNIVPLL, .rate = UNIVPLL_HZ },
	{ .id = APMIXED_MSDCPLL, .rate = MSDCPLL_HZ },
	{ .id = APMIXED_MMPLL, .rate = MMPLL_HZ },
	{ .id = APMIXED_MFGPLL, .rate = MFGPLL_HZ },
	{ .id = APMIXED_TVDPLL, .rate = TVDPLL_HZ },
	{ .id = APMIXED_APLL1, .rate = APLL1_HZ },
	{ .id = APMIXED_APLL2, .rate = APLL2_HZ },
};

void pll_set_pcw_change(const struct pll *pll)
{
	setbits_le32(pll->div_reg, PLL_PCW_CHG);
}

void mt_pll_init(void)
{
	int i;

	/* enable univpll & mainpll div */
	setbits_le32(&mtk_apmixed->ap_pll_con2, 0x1FFE << 16);

	/* enable clock square1 low-pass filter */
	setbits_le32(&mtk_apmixed->ap_pll_con0, 0x2);

	/* xPLL PWR ON */
	for (i = 0; i < APMIXED_NR_PLL; i++)
		setbits_le32(plls[i].pwr_reg, PLL_PWR_ON);

	udelay(PLL_PWR_ON_DELAY);

	/* xPLL ISO Disable */
	for (i = 0; i < APMIXED_NR_PLL; i++)
		clrbits_le32(plls[i].pwr_reg, PLL_ISO);

	udelay(PLL_ISO_DELAY);

	/* xPLL Frequency Set */
	for (i = 0; i < ARRAY_SIZE(rates); i++)
		pll_set_rate(&plls[rates[i].id], rates[i].rate);

	/* AUDPLL Tuner Frequency Set */
	write32(&mtk_apmixed->apll1_con2,
		read32(&mtk_apmixed->apll1_con1) + 1);
	write32(&mtk_apmixed->apll2_con2,
		read32(&mtk_apmixed->apll2_con1) + 1);

	/* xPLL Frequency Enable */
	for (i = 0; i < APMIXED_NR_PLL; i++)
		setbits_le32(plls[i].reg, PLL_EN);

	/* wait for PLL stable */
	udelay(PLL_EN_DELAY);

	/* xPLL DIV RSTB */
	for (i = 0; i < APMIXED_NR_PLL; i++) {
		if (plls[i].rstb_shift != NO_RSTB_SHIFT)
			setbits_le32(plls[i].reg, 1 << plls[i].rstb_shift);
	}

	/* MCUCFG CLKMUX */
	clrsetbits_le32(&mt8183_mcucfg->mp0_pll_divider_cfg, DIV_MASK, DIV_1);
	clrsetbits_le32(&mt8183_mcucfg->mp2_pll_divider_cfg, DIV_MASK, DIV_1);
	clrsetbits_le32(&mt8183_mcucfg->bus_pll_divider_cfg, DIV_MASK, DIV_2);

	clrsetbits_le32(&mt8183_mcucfg->mp0_pll_divider_cfg, MUX_MASK,
		MUX_SRC_ARMPLL);
	clrsetbits_le32(&mt8183_mcucfg->mp2_pll_divider_cfg, MUX_MASK,
		MUX_SRC_ARMPLL);
	clrsetbits_le32(&mt8183_mcucfg->bus_pll_divider_cfg, MUX_MASK,
		MUX_SRC_ARMPLL);

	/* enable infrasys DCM */
	setbits_le32(&mt8183_infracfg->infra_bus_dcm_ctrl, 0x3 << 21);

	/*
	 * TOP CLKMUX -- DO NOT CHANGE WITHOUT ADJUSTING <soc/pll.h> CONSTANTS!
	 */
	for (i = 0; i < ARRAY_SIZE(mux_sels); i++)
		mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

	/* enable [14] dramc_pll104m_ck */
	setbits_le32(&mtk_topckgen->clk_misc_cfg_0, 1 << 14);
}
