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

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <stddef.h>

#include <soc/addressmap.h>
#include <soc/infracfg.h>
#include <soc/pll.h>

#define GENMASK(h, l)	(((1U << ((h) - (l) + 1)) - 1) << (l))

enum mux_id {
	TOP_AXI_SEL,
	TOP_MEM_SEL,
	TOP_DDRPHYCFG_SEL,
	TOP_MM_SEL,
	TOP_PWM_SEL,
	TOP_VDEC_SEL,
	TOP_VENC_SEL,
	TOP_MFG_SEL,
	TOP_CAMTG_SEL,
	TOP_UART_SEL,
	TOP_SPI_SEL,
	TOP_USB20_SEL,
	TOP_USB30_SEL,
	TOP_MSDC50_0_H_SEL,
	TOP_MSDC50_0_SEL,
	TOP_MSDC30_1_SEL,
	TOP_MSDC30_2_SEL,
	TOP_MSDC30_3_SEL,
	TOP_AUDIO_SEL,
	TOP_AUD_INTBUS_SEL,
	TOP_PMICSPI_SEL,
	TOP_SCP_SEL,
	TOP_ATB_SEL,
	TOP_VENC_LT_SEL,
	TOP_DPI0_SEL,
	TOP_IRDA_SEL,
	TOP_CCI400_SEL,
	TOP_AUD_1_SEL,
	TOP_AUD_2_SEL,
	TOP_MEM_MFG_IN_SEL,
	TOP_AXI_MFG_IN_SEL,
	TOP_SCAM_SEL,
	TOP_SPINFI_IFR_SEL,
	TOP_HDMI_SEL,
	TOP_DPILVDS_SEL,
	TOP_MSDC50_2_H_SEL,
	TOP_HDCP_SEL,
	TOP_HDCP_24M_SEL,
	TOP_RTC_SEL,
	TOP_NR_MUX
};

#define TOPCKGEN_REG(x)	(CKSYS_BASE + offsetof(struct mt8173_topckgen_regs, x))
#define APMIXED_REG(x)	(APMIXED_BASE + offsetof(struct mt8173_apmixed_regs, x))

struct mux {
	void *reg;
	u8 mux_shift;
	u8 mux_width;
};

#define MUX(_id, _reg, _mux_shift, _mux_width)		\
	[_id] = {					\
		.reg = (void *)TOPCKGEN_REG(_reg),	\
		.mux_shift = _mux_shift,		\
		.mux_width = _mux_width,		\
	}

static const struct mux muxes[] = {
	/* CLK_CFG_0 */
	MUX(TOP_AXI_SEL, clk_cfg_0, 0, 3),
	MUX(TOP_MEM_SEL, clk_cfg_0, 8, 1),
	MUX(TOP_DDRPHYCFG_SEL, clk_cfg_0, 16, 1),
	MUX(TOP_MM_SEL, clk_cfg_0, 24, 4),
	/* CLK_CFG_1 */
	MUX(TOP_PWM_SEL, clk_cfg_1, 0, 2),
	MUX(TOP_VDEC_SEL, clk_cfg_1, 8, 4),
	MUX(TOP_VENC_SEL, clk_cfg_1, 16, 4),
	MUX(TOP_MFG_SEL, clk_cfg_1, 24, 4),
	/* CLK_CFG_2 */
	MUX(TOP_CAMTG_SEL, clk_cfg_2, 0, 3),
	MUX(TOP_UART_SEL, clk_cfg_2, 8, 1),
	MUX(TOP_SPI_SEL, clk_cfg_2, 16, 3),
	MUX(TOP_USB20_SEL, clk_cfg_2, 24, 2),
	/* CLK_CFG_3 */
	MUX(TOP_USB30_SEL, clk_cfg_3, 0, 2),
	MUX(TOP_MSDC50_0_H_SEL, clk_cfg_3, 8, 3),
	MUX(TOP_MSDC50_0_SEL, clk_cfg_3, 16, 4),
	MUX(TOP_MSDC30_1_SEL, clk_cfg_3, 24, 3),
	/* CLK_CFG_4 */
	MUX(TOP_MSDC30_2_SEL, clk_cfg_4, 0, 3),
	MUX(TOP_MSDC30_3_SEL, clk_cfg_4, 8, 4),
	MUX(TOP_AUDIO_SEL, clk_cfg_4, 16, 2),
	MUX(TOP_AUD_INTBUS_SEL, clk_cfg_4, 24, 3),
	/* CLK_CFG_5 */
	MUX(TOP_PMICSPI_SEL, clk_cfg_5, 0, 3),
	MUX(TOP_SCP_SEL, clk_cfg_5, 8, 3),
	MUX(TOP_ATB_SEL, clk_cfg_5, 16, 2),
	MUX(TOP_VENC_LT_SEL, clk_cfg_5, 24, 4),
	/* CLK_CFG_6 */
	MUX(TOP_DPI0_SEL, clk_cfg_6, 0, 3),
	MUX(TOP_IRDA_SEL, clk_cfg_6, 8, 2),
	MUX(TOP_CCI400_SEL, clk_cfg_6, 16, 3),
	MUX(TOP_AUD_1_SEL, clk_cfg_6, 24, 2),
	/* CLK_CFG_7 */
	MUX(TOP_AUD_2_SEL, clk_cfg_7, 0, 2),
	MUX(TOP_MEM_MFG_IN_SEL, clk_cfg_7, 8, 2),
	MUX(TOP_AXI_MFG_IN_SEL, clk_cfg_7, 16, 2),
	MUX(TOP_SCAM_SEL, clk_cfg_7, 24, 2),
	/* CLK_CFG_12 */
	MUX(TOP_SPINFI_IFR_SEL, clk_cfg_12, 0, 3),
	MUX(TOP_HDMI_SEL, clk_cfg_12, 8, 2),
	MUX(TOP_DPILVDS_SEL, clk_cfg_12, 24, 3),
	/* CLK_CFG_13 */
	MUX(TOP_MSDC50_2_H_SEL, clk_cfg_13, 0, 3),
	MUX(TOP_HDCP_SEL, clk_cfg_13, 8, 2),
	MUX(TOP_HDCP_24M_SEL, clk_cfg_13, 16, 2),
	MUX(TOP_RTC_SEL, clk_cfg_13, 24, 2),
};

static void mux_set_sel(const struct mux *mux, u32 sel)
{
	u32 mask = GENMASK(mux->mux_width - 1, 0);
	u32 val = read32(mux->reg);

	val &= ~(mask << mux->mux_shift);
	val |= (sel & mask) << mux->mux_shift;
	write32(mux->reg, val);
}

#define PLL_PWR_ON		(1 << 0)
#define PLL_EN			(1 << 0)
#define PLL_ISO			(1 << 1)
#define PLL_RSTB		(1 << 24)
#define PLL_PCW_CHG		(1 << 31)
#define PLL_POSTDIV_MASK	0x7
#define PCW_INTEGER_BITS	7

enum pll_id {
	APMIXED_ARMCA15PLL,
	APMIXED_ARMCA7PLL,
	APMIXED_MAINPLL,
	APMIXED_UNIVPLL,
	APMIXED_MMPLL,
	APMIXED_MSDCPLL,
	APMIXED_VENCPLL,
	APMIXED_TVDPLL,
	APMIXED_MPLL,
	APMIXED_VCODECPLL,
	APMIXED_APLL1,
	APMIXED_APLL2,
	APMIXED_LVDSPLL,
	APMIXED_MSDCPLL2,
	APMIXED_NR_PLL
};

const u32 pll_div_rate[] = {
	3UL * GHz,
	1 * GHz,
	500 * MHz,
	250 * MHz,
	125 * MHz,
	0,
};

const u32 univpll_div_rate[] = {
	3UL * GHz,
	1500 * MHz,
	750 * MHz,
	375 * MHz,
	187500 * KHz,
	0,
};

const u32 mmpll_div_rate[] = {
	3UL * GHz,
	1 * GHz,
	702 * MHz,
	253500 * KHz,
	126750 * KHz,
	0,
};

struct pll {
	void *reg;
	void *pwr_reg;
	u32 rstb;
	u8 pcwbits;
	void *div_reg;
	u8 div_shift;
	void *pcw_reg;
	u8 pcw_shift;
	const u32 *div_rate;
};

#define PLL(_id, _reg, _pwr_reg, _rstb, _pcwbits, _div_reg, _div_shift, \
			_pcw_reg, _pcw_shift, _div_rate)		\
	[_id] = {							\
		.reg = (void *)APMIXED_REG(_reg),			\
		.pwr_reg = (void *)APMIXED_REG(_pwr_reg),		\
		.rstb = _rstb,						\
		.pcwbits = _pcwbits,					\
		.div_reg = (void *)APMIXED_REG(_div_reg),		\
		.div_shift = _div_shift,				\
		.pcw_reg = (void *)APMIXED_REG(_pcw_reg),		\
		.pcw_shift = _pcw_shift,				\
		.div_rate = _div_rate,					\
	}

static const struct pll plls[] = {
	PLL(APMIXED_ARMCA15PLL, armca15pll_con0, armca15pll_pwr_con0, 0, 21,
		armca15pll_con1, 24, armca15pll_con1, 0, pll_div_rate),
	PLL(APMIXED_ARMCA7PLL, armca7pll_con0, armca7pll_pwr_con0, PLL_RSTB, 21,
		armca7pll_con1, 24, armca7pll_con1, 0, pll_div_rate),
	PLL(APMIXED_MAINPLL, mainpll_con0, mainpll_pwr_con0, PLL_RSTB, 21,
		mainpll_con0, 4, mainpll_con1, 0, pll_div_rate),
	PLL(APMIXED_UNIVPLL, univpll_con0, univpll_pwr_con0, PLL_RSTB, 7,
		univpll_con0, 4, univpll_con1, 14, univpll_div_rate),
	PLL(APMIXED_MMPLL, mmpll_con0, mmpll_pwr_con0, 0, 21,
		mmpll_con1, 24, mmpll_con1, 0, mmpll_div_rate),
	PLL(APMIXED_MSDCPLL, msdcpll_con0, msdcpll_pwr_con0, 0, 21,
		msdcpll_con0, 4, msdcpll_con1, 0, pll_div_rate),
	PLL(APMIXED_VENCPLL, vencpll_con0, vencpll_pwr_con0, 0, 21,
		vencpll_con0, 4, vencpll_con1, 0, pll_div_rate),
	PLL(APMIXED_TVDPLL, tvdpll_con0, tvdpll_pwr_con0, 0, 21,
		tvdpll_con0, 4, tvdpll_con1, 0, pll_div_rate),
	PLL(APMIXED_MPLL, mpll_con0, mpll_pwr_con0, 0, 21,
		mpll_con0, 4, mpll_con1, 0, pll_div_rate),
	PLL(APMIXED_VCODECPLL, vcodecpll_con0, vcodecpll_pwr_con0, 0, 21,
		vcodecpll_con0, 4, vcodecpll_con1, 0, pll_div_rate),
	PLL(APMIXED_APLL1, apll1_con0, apll1_pwr_con0, 0, 31,
		apll1_con0, 4, apll1_con1, 0, pll_div_rate),
	PLL(APMIXED_APLL2, apll2_con0, apll2_pwr_con0, 0, 31,
		apll2_con0, 4, apll2_con1, 0, pll_div_rate),
	PLL(APMIXED_LVDSPLL, lvdspll_con0, lvdspll_pwr_con0, 0, 21,
		lvdspll_con0, 4, lvdspll_con1, 0, pll_div_rate),
	PLL(APMIXED_MSDCPLL2, msdcpll2_con0, msdcpll2_pwr_con0, 0, 21,
		msdcpll2_con0, 4, msdcpll2_con1, 0, pll_div_rate),
};

static void pll_set_rate_regs(const struct pll *pll, u32 pcw, u32 postdiv)
{
	u32 val;

	/* set postdiv */
	val = read32(pll->div_reg);
	val &= ~(PLL_POSTDIV_MASK << pll->div_shift);
	val |= postdiv << pll->div_shift;

	/* postdiv and pcw need to set at the same time if on same register */
	if (pll->div_reg != pll->pcw_reg) {
		write32(pll->div_reg, val);
		val = read32(pll->pcw_reg);
	}

	/* set pcw */
	val &= ~GENMASK(pll->pcw_shift + pll->pcwbits - 1, pll->pcw_shift);
	val |= pcw << pll->pcw_shift;
	val |= PLL_PCW_CHG;
	write32(pll->pcw_reg, val);
}

static void pll_calc_values(const struct pll *pll, u32 *pcw, u32 *postdiv,
			    u32 freq)
{
	const u32 fin_hz = CLK26M_HZ;
	const u32 *div_rate = pll->div_rate;
	u32 val;

	assert(freq <= div_rate[0]);
	assert(freq >= 1 * GHz / 16);

	for (val = 1; div_rate[val] != 0; val++) {
		if (freq > div_rate[val])
			break;
	}
	val--;
	*postdiv = val;

	/* _pcw = freq * 2^postdiv / fin * 2^pcwbits_fractional */
	val += pll->pcwbits - PCW_INTEGER_BITS;

	*pcw = ((u64)freq << val) / fin_hz;
}

static int pll_set_rate(const struct pll *pll, u32 rate)
{
	u32 pcw = 0;
	u32 postdiv;

	pll_calc_values(pll, &pcw, &postdiv, rate);
	pll_set_rate_regs(pll, pcw, postdiv);

	return 0;
}

void mt_pll_init(void)
{
	int i;

	/* reduce CLKSQ disable time */
	write32(&mt8173_apmixed->clksq_stb_con0, (0x05 << 8) | (0x01 << 0));
	/* extend PWR/ISO control timing to 1us */
	write32(&mt8173_apmixed->pll_iso_con0, (0x8 << 16) | (0x8 << 0));
	write32(&mt8173_apmixed->ap_pll_con6, 0x00000000);

	/*************
	 * xPLL PWR ON
	 **************/
	for (i = 0; i < APMIXED_NR_PLL; i++)
		setbits_le32(plls[i].pwr_reg, PLL_PWR_ON);

	udelay(5);  /* wait for xPLL_PWR_ON ready (min delay is 1us) */

	/******************
	 * xPLL ISO Disable
	 *******************/
	for (i = 0; i < APMIXED_NR_PLL; i++)
		clrbits_le32(plls[i].pwr_reg, PLL_ISO);

	/********************
	 * xPLL Frequency Set
	 *********************/

	pll_set_rate(&plls[APMIXED_ARMCA15PLL], ARMCA15PLL_HZ);
	pll_set_rate(&plls[APMIXED_ARMCA7PLL], ARMCA7PLL_HZ);
	pll_set_rate(&plls[APMIXED_MAINPLL], MAINPLL_HZ);
	pll_set_rate(&plls[APMIXED_UNIVPLL], UNIVPLL_HZ);
	pll_set_rate(&plls[APMIXED_MMPLL], MMPLL_HZ);
	pll_set_rate(&plls[APMIXED_MSDCPLL], MSDCPLL_HZ);
	pll_set_rate(&plls[APMIXED_VENCPLL], VENCPLL_HZ);
	pll_set_rate(&plls[APMIXED_TVDPLL], TVDPLL_HZ);
	pll_set_rate(&plls[APMIXED_MPLL], MPLL_HZ);
	pll_set_rate(&plls[APMIXED_VCODECPLL], VCODECPLL_HZ);
	pll_set_rate(&plls[APMIXED_LVDSPLL], LVDSPLL_HZ);
	pll_set_rate(&plls[APMIXED_MSDCPLL2], MSDCPLL2_HZ);
	pll_set_rate(&plls[APMIXED_APLL1], APLL1_HZ);
	pll_set_rate(&plls[APMIXED_APLL2], APLL2_HZ);

	/***********************
	 * xPLL Frequency Enable
	 ************************/
	for (i = 0; i < APMIXED_NR_PLL; i++)
		setbits_le32(plls[i].reg, PLL_EN);

	udelay(40);  /* wait for PLL stable (min delay is 20us) */

	/***************
	 * xPLL DIV RSTB
	 ****************/
	for (i = 0; i < APMIXED_NR_PLL; i++) {
		if (plls[i].rstb)
			setbits_le32(plls[i].reg, plls[i].rstb);
	}

	/**************
	 * INFRA CLKMUX
	 ***************/

	/* enable infrasys DCM */
	setbits_le32(&mt8173_infracfg->top_dcmctl, 0x1);

	write32(&mt8173_topckgen->clk_mode, 0x1);
	write32(&mt8173_topckgen->clk_mode, 0x0); /* enable TOPCKGEN */

	/************
	 * TOP CLKMUX -- DO NOT CHANGE WITHOUT ADJUSTING <soc/pll.h> CONSTANTS!
	 *************/

	/* CLK_CFG_0 */
	mux_set_sel(&muxes[TOP_AXI_SEL], 5);		/* 5: univpll2_d2 */
	mux_set_sel(&muxes[TOP_MEM_SEL], 0);		/* 0: clk26m */
	mux_set_sel(&muxes[TOP_DDRPHYCFG_SEL], 0);	/* 0: clk26m */
	mux_set_sel(&muxes[TOP_MM_SEL], 1);		/* 1: vencpll_d2 */
	/* CLK_CFG_1 */
	mux_set_sel(&muxes[TOP_PWM_SEL], 0);		/* 0: clk26m */
	mux_set_sel(&muxes[TOP_VDEC_SEL], 1);		/* 1: vcodecpll_ck */
	mux_set_sel(&muxes[TOP_VENC_SEL], 1);		/* 1: vcodecpll_ck */
	mux_set_sel(&muxes[TOP_MFG_SEL], 1);		/* 1: mmpll_ck */
	/* CLK_CFG_2 */
	mux_set_sel(&muxes[TOP_CAMTG_SEL], 0);		/* 0: clk26m */
	mux_set_sel(&muxes[TOP_UART_SEL], 0);		/* 0: clk26m */
	mux_set_sel(&muxes[TOP_SPI_SEL], 1);		/* 1: syspll3_d2 */
	mux_set_sel(&muxes[TOP_USB20_SEL], 1);		/* 1: univpll1_d8 */
	/* CLK_CFG_4 */
	mux_set_sel(&muxes[TOP_MSDC30_2_SEL], 2);	/* 2: msdcpll_d4 */
	mux_set_sel(&muxes[TOP_MSDC30_3_SEL], 5);	/* 5: msdcpll_d4 */
	mux_set_sel(&muxes[TOP_AUDIO_SEL], 0);		/* 0: clk26m */
	mux_set_sel(&muxes[TOP_AUD_INTBUS_SEL], 1);	/* 1: syspll1_d4 */
	/* CLK_CFG_5 */
	mux_set_sel(&muxes[TOP_PMICSPI_SEL], 0);	/* 0: clk26m */
	mux_set_sel(&muxes[TOP_SCP_SEL], 1);		/* 1: syspll1_d2 */
	mux_set_sel(&muxes[TOP_ATB_SEL], 0);		/* 0: clk26m */
	mux_set_sel(&muxes[TOP_VENC_LT_SEL], 6);	/* 6: univpll1_d2 */
	/* CLK_CFG_6 */
	mux_set_sel(&muxes[TOP_DPI0_SEL], 1);		/* 1: tvdpll_d2 */
	mux_set_sel(&muxes[TOP_IRDA_SEL], 1);		/* 1: univpll2_d4 */
	mux_set_sel(&muxes[TOP_CCI400_SEL], 5);		/* 5: syspll_d2 */
	mux_set_sel(&muxes[TOP_AUD_1_SEL], 1);		/* 1: apll1_ck */
	/* CLK_CFG_7 */
	mux_set_sel(&muxes[TOP_AUD_2_SEL], 1);		/* 1: apll2_ck */
	mux_set_sel(&muxes[TOP_MEM_MFG_IN_SEL], 1);	/* 1: mmpll_ck */
	mux_set_sel(&muxes[TOP_AXI_MFG_IN_SEL], 1);	/* 1: hd_faxi_ck */
	mux_set_sel(&muxes[TOP_SCAM_SEL], 1);		/* 1: syspll3_d2 */
	/* CLK_CFG_12 */
	mux_set_sel(&muxes[TOP_SPINFI_IFR_SEL], 0);	/* 0: clk26m */
	mux_set_sel(&muxes[TOP_HDMI_SEL], 1);		/* 1: AD_HDMITX_CLK */
	mux_set_sel(&muxes[TOP_DPILVDS_SEL], 1);	/* 1: AD_LVDSPLL_CK */
	/* CLK_CFG_13 */
	mux_set_sel(&muxes[TOP_MSDC50_2_H_SEL], 2);	/* 2: syspll2_d2 */
	mux_set_sel(&muxes[TOP_HDCP_SEL], 2);		/* 2: syspll3_d4 */
	mux_set_sel(&muxes[TOP_HDCP_24M_SEL], 2);	/* 2: univpll_d52 */
	mux_set_sel(&muxes[TOP_RTC_SEL], 1);		/* 1: clkrtc_ext */
	/* CLK_CFG_3 */
	mux_set_sel(&muxes[TOP_USB30_SEL], 1);		/* 1: univpll3_d2 */
	mux_set_sel(&muxes[TOP_MSDC50_0_H_SEL], 2);	/* 2: syspll2_d2 */
	mux_set_sel(&muxes[TOP_MSDC50_0_SEL], 6);	/* 6: msdcpll_d4 */
	mux_set_sel(&muxes[TOP_MSDC30_1_SEL], 2);	/* 2: msdcpll_d4 */

	/* enable scpsys clock off control */
	write32(&mt8173_topckgen->clk_scp_cfg_0,
		(1 << 10) | (1 << 9) | (1 << 5) | (1 << 4) | (1 << 2) |
		(1 << 1) | (1 << 0));
	write32(&mt8173_topckgen->clk_scp_cfg_1,
		(1 << 4) | (1 << 2) | (1 << 0));
}

/* Turn on ADA_SSUSB_XTAL_CK 26MHz */
void mt_pll_enable_ssusb_clk(void)
{
	/* set  RG_LTECLKSQ_EN */
	setbits_le32(&mt8173_apmixed->ap_pll_con0, 0x1);
	udelay(100);  /* wait for PLL stable */

	/* set RG_LTECLKSQ_LPF_EN & DA_REF2USB_TX_EN */
	setbits_le32(&mt8173_apmixed->ap_pll_con0, 0x1 << 1);
	setbits_le32(&mt8173_apmixed->ap_pll_con2, 0x1);
	udelay(100);  /* wait for PLL stable */

	/* set DA_REF2USB_TX_LPF_EN & DA_REF2USB_TX_OUT_EN */
	setbits_le32(&mt8173_apmixed->ap_pll_con2, (0x1 << 2) | (0x1 << 1));
}


/* after pmic_init */
void mt_pll_post_init(void)
{
	/* CPU clock divide by 1 */
	clrbits_le32(&mt8173_infracfg->top_ckdiv1, 0x3ff);

	/* select ARMPLL */
	write32(&mt8173_infracfg->top_ckmuxsel, (1 << 2) | 1);
}

void mt_pll_set_aud_div(u32 rate)
{
	u32 mclk_div;
	u32 apll_clock = APLL2_CK_HZ;
	int apll1 = 0;

	if (rate % 11025 == 0) {
		/* use APLL1 instead */
		apll1 = 1;
		apll_clock = APLL1_CK_HZ;
	}
	/* I2S1 clock */
	mclk_div = (apll_clock / 256 / rate) - 1;
	assert(apll_clock == rate * 256 * (mclk_div + 1));

	if (apll1) {
		/* mclk */
		clrbits_le32(&mt8173_topckgen->clk_auddiv_0, 1 << 5);
		clrsetbits_le32(&mt8173_topckgen->clk_auddiv_1, 0xff << 8,
				mclk_div << 8);
		/* bclk */
		clrsetbits_le32(&mt8173_topckgen->clk_auddiv_0, 0xf << 24,
				7 << 24);
	} else {
		/* mclk */
		setbits_le32(&mt8173_topckgen->clk_auddiv_0, 1 << 5);
		clrsetbits_le32(&mt8173_topckgen->clk_auddiv_2, 0xff << 8,
				mclk_div << 8);
		/* bclk */
		clrsetbits_le32(&mt8173_topckgen->clk_auddiv_0, 0xf << 28,
				7 << 28);
	}
}

void mt_pll_raise_ca53_freq(u32 freq) {
	pll_set_rate(&plls[APMIXED_ARMCA7PLL], freq); /* freq in Hz */
}

void mt_mem_pll_config_pre(const struct mt8173_sdram_params *sdram_params)
{
	u32 mpll_sdm_pcw_20_0 = 0xF13B1;

	/* disable MPLL for adjusting memory clk frequency */
	clrbits_le32(&mt8173_apmixed->mpll_con0, BIT(0));
	/* MPLL configuration: mode selection */
	setbits_le32(&mt8173_apmixed->mpll_con0, BIT(16));
	clrbits_le32(&mt8173_apmixed->mpll_con0, 0x7 << 4);
	clrbits_le32(&mt8173_apmixed->pll_test_con0, 1 << 31);
	/* set RG_MPLL_SDM_PCW for feedback divide ratio */
	clrsetbits_le32(&mt8173_apmixed->mpll_con1, 0x1fffff, mpll_sdm_pcw_20_0);
}

void mt_mem_pll_config_post(void)
{
	/* power up sequence starts: enable MPLL */
	setbits_le32(&mt8173_apmixed->mpll_con0, BIT(0));
}

void mt_mem_pll_mux(void)
{
	/* CLK_CFG_0 */
	mux_set_sel(&muxes[TOP_MEM_SEL], 1); /* 1: dmpll_ck */
}
