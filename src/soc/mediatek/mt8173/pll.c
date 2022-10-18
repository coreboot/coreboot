/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/infracfg.h>
#include <soc/pll.h>
#include <types.h>

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

#define MUX(_id, _reg, _mux_shift, _mux_width)		\
	[_id] = {					\
		.reg = &mtk_topckgen->_reg,		\
		.mux_shift = _mux_shift,		\
		.mux_width = _mux_width,		\
		.upd_reg = NULL,			\
		.upd_shift = 0,				\
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

struct mux_sel {
	enum mux_id id;
	u32 sel;
};

static const struct mux_sel mux_sels[] = {
	/* CLK_CFG_0 */
	{ .id = TOP_AXI_SEL, .sel = 5 },		/* 5: univpll2_d2 */
	{ .id = TOP_MEM_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_DDRPHYCFG_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_MM_SEL, .sel = 1 },			/* 1: vencpll_d2 */
	/* CLK_CFG_1 */
	{ .id = TOP_PWM_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_VDEC_SEL, .sel = 1 },		/* 1: vcodecpll_ck */
	{ .id = TOP_VENC_SEL, .sel = 1 },		/* 1: vcodecpll_ck */
	{ .id = TOP_MFG_SEL, .sel = 1 },		/* 1: mmpll_ck */
	/* CLK_CFG_2 */
	{ .id = TOP_CAMTG_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_UART_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_SPI_SEL, .sel = 1 },		/* 1: syspll3_d2 */
	{ .id = TOP_USB20_SEL, .sel = 1 },		/* 1: univpll1_d8 */
	/* CLK_CFG_4 */
	{ .id = TOP_MSDC30_2_SEL, .sel = 2 },		/* 2: msdcpll_d4 */
	{ .id = TOP_MSDC30_3_SEL, .sel = 5 },		/* 5: msdcpll_d4 */
	{ .id = TOP_AUDIO_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_AUD_INTBUS_SEL, .sel = 1 },		/* 1: syspll1_d4 */
	/* CLK_CFG_5 */
	{ .id = TOP_PMICSPI_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_SCP_SEL, .sel = 1 },		/* 1: syspll1_d2 */
	{ .id = TOP_ATB_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_VENC_LT_SEL, .sel = 6 },		/* 6: univpll1_d2 */
	/* CLK_CFG_6 */
	{ .id = TOP_DPI0_SEL, .sel = 1 },		/* 1: tvdpll_d2 */
	{ .id = TOP_IRDA_SEL, .sel = 1 },		/* 1: univpll2_d4 */
	{ .id = TOP_CCI400_SEL, .sel = 5 },		/* 5: syspll_d2 */
	{ .id = TOP_AUD_1_SEL, .sel = 1 },		/* 1: apll1_ck */
	/* CLK_CFG_7 */
	{ .id = TOP_AUD_2_SEL, .sel = 1 },		/* 1: apll2_ck */
	{ .id = TOP_MEM_MFG_IN_SEL, .sel = 1 },		/* 1: mmpll_ck */
	{ .id = TOP_AXI_MFG_IN_SEL, .sel = 1 },		/* 1: hd_faxi_ck */
	{ .id = TOP_SCAM_SEL, .sel = 1 },		/* 1: syspll3_d2 */
	/* CLK_CFG_12 */
	{ .id = TOP_SPINFI_IFR_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_HDMI_SEL, .sel = 1 },		/* 1: AD_HDMITX_CLK */
	{ .id = TOP_DPILVDS_SEL, .sel = 1 },		/* 1: AD_LVDSPLL_CK */
	/* CLK_CFG_13 */
	{ .id = TOP_MSDC50_2_H_SEL, .sel = 2 },		/* 2: syspll2_d2 */
	{ .id = TOP_HDCP_SEL, .sel = 2 },		/* 2: syspll3_d4 */
	{ .id = TOP_HDCP_24M_SEL, .sel = 2 },		/* 2: univpll_d52 */
	{ .id = TOP_RTC_SEL, .sel = 1 },		/* 1: clkrtc_ext */
	/* CLK_CFG_3 */
	{ .id = TOP_USB30_SEL, .sel = 1 },		/* 1: univpll3_d2 */
	{ .id = TOP_MSDC50_0_H_SEL, .sel = 2 },		/* 2: syspll2_d2 */
	{ .id = TOP_MSDC50_0_SEL, .sel = 6 },		/* 6: msdcpll_d4 */
	{ .id = TOP_MSDC30_1_SEL, .sel = 2 },		/* 2: msdcpll_d4 */
};

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

static const struct pll plls[] = {
	PLL(APMIXED_ARMCA15PLL, armca15pll_con0, armca15pll_pwr_con0,
		NO_RSTB_SHIFT, 21, armca15pll_con1, 24, armca15pll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_ARMCA7PLL, armca7pll_con0, armca7pll_pwr_con0,
		PLL_RSTB_SHIFT, 21, armca7pll_con1, 24, armca7pll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MAINPLL, mainpll_con0, mainpll_pwr_con0,
		PLL_RSTB_SHIFT, 21, mainpll_con0, 4, mainpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_UNIVPLL, univpll_con0, univpll_pwr_con0,
		PLL_RSTB_SHIFT, 7, univpll_con0, 4, univpll_con1, 14,
		univpll_div_rate),
	PLL(APMIXED_MMPLL, mmpll_con0, mmpll_pwr_con0,
		NO_RSTB_SHIFT, 21, mmpll_con1, 24, mmpll_con1, 0,
		mmpll_div_rate),
	PLL(APMIXED_MSDCPLL, msdcpll_con0, msdcpll_pwr_con0,
		NO_RSTB_SHIFT, 21, msdcpll_con0, 4, msdcpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_VENCPLL, vencpll_con0, vencpll_pwr_con0,
		NO_RSTB_SHIFT, 21, vencpll_con0, 4, vencpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_TVDPLL, tvdpll_con0, tvdpll_pwr_con0,
		NO_RSTB_SHIFT, 21, tvdpll_con0, 4, tvdpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MPLL, mpll_con0, mpll_pwr_con0,
		NO_RSTB_SHIFT, 21, mpll_con0, 4, mpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_VCODECPLL, vcodecpll_con0, vcodecpll_pwr_con0,
		NO_RSTB_SHIFT, 21, vcodecpll_con0, 4, vcodecpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_APLL1, apll1_con0, apll1_pwr_con0,
		NO_RSTB_SHIFT, 31, apll1_con0, 4, apll1_con1, 0,
		pll_div_rate),
	PLL(APMIXED_APLL2, apll2_con0, apll2_pwr_con0,
		NO_RSTB_SHIFT, 31, apll2_con0, 4, apll2_con1, 0,
		pll_div_rate),
	PLL(APMIXED_LVDSPLL, lvdspll_con0, lvdspll_pwr_con0,
		NO_RSTB_SHIFT, 21, lvdspll_con0, 4, lvdspll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MSDCPLL2, msdcpll2_con0, msdcpll2_pwr_con0,
		NO_RSTB_SHIFT, 21, msdcpll2_con0, 4, msdcpll2_con1, 0,
		pll_div_rate),
};

struct rate {
	enum pll_id id;
	u32 rate;
};

static const struct rate rates[] = {
	{ .id = APMIXED_ARMCA15PLL, .rate = ARMCA15PLL_HZ },
	{ .id = APMIXED_ARMCA7PLL, .rate = ARMCA7PLL_HZ },
	{ .id = APMIXED_MAINPLL, .rate = MAINPLL_HZ },
	{ .id = APMIXED_UNIVPLL, .rate = UNIVPLL_HZ },
	{ .id = APMIXED_MMPLL, .rate = MMPLL_HZ },
	{ .id = APMIXED_MSDCPLL, .rate = MSDCPLL_HZ },
	{ .id = APMIXED_VENCPLL, .rate = VENCPLL_HZ },
	{ .id = APMIXED_TVDPLL, .rate = TVDPLL_HZ },
	{ .id = APMIXED_MPLL, .rate = MPLL_HZ },
	{ .id = APMIXED_VCODECPLL, .rate = VCODECPLL_HZ },
	{ .id = APMIXED_LVDSPLL, .rate = LVDSPLL_HZ },
	{ .id = APMIXED_MSDCPLL2, .rate = MSDCPLL2_HZ },
	{ .id = APMIXED_APLL1, .rate = APLL1_HZ },
	{ .id = APMIXED_APLL2, .rate = APLL2_HZ },
};

void pll_set_pcw_change(const struct pll *pll)
{
	setbits32(pll->pcw_reg, PLL_PCW_CHG);
}

void mt_pll_init(void)
{
	int i;

	/* reduce CLKSQ disable time */
	write32(&mtk_apmixed->clksq_stb_con0, (0x05 << 8) | (0x01 << 0));
	/* extend PWR/ISO control timing to 1us */
	write32(&mtk_apmixed->pll_iso_con0, (0x8 << 16) | (0x8 << 0));
	write32(&mtk_apmixed->ap_pll_con6, 0x00000000);

	/*************
	 * xPLL PWR ON
	 **************/
	for (i = 0; i < APMIXED_NR_PLL; i++)
		setbits32(plls[i].pwr_reg, PLL_PWR_ON);

	/* wait for xPLL_PWR_ON ready (min delay is 1us) */
	udelay(PLL_PWR_ON_DELAY);

	/******************
	 * xPLL ISO Disable
	 *******************/
	for (i = 0; i < APMIXED_NR_PLL; i++)
		clrbits32(plls[i].pwr_reg, PLL_ISO);

	/********************
	 * xPLL Frequency Set
	 *********************/
	for (i = 0; i < ARRAY_SIZE(rates); i++)
		pll_set_rate(&plls[rates[i].id], rates[i].rate);

	/***********************
	 * xPLL Frequency Enable
	 ************************/
	for (i = 0; i < APMIXED_NR_PLL; i++)
		setbits32(plls[i].reg, PLL_EN);

	udelay(PLL_EN_DELAY);  /* wait for PLL stable (min delay is 20us) */

	/***************
	 * xPLL DIV RSTB
	 ****************/
	for (i = 0; i < APMIXED_NR_PLL; i++) {
		if (plls[i].rstb_shift != NO_RSTB_SHIFT)
			setbits32(plls[i].reg, 1 << plls[i].rstb_shift);
	}

	/**************
	 * INFRA CLKMUX
	 ***************/

	/* enable infrasys DCM */
	setbits32(&mt8173_infracfg->top_dcmctl, 0x1);

	write32(&mtk_topckgen->clk_mode, 0x1);
	write32(&mtk_topckgen->clk_mode, 0x0); /* enable TOPCKGEN */

	/************
	 * TOP CLKMUX -- DO NOT CHANGE WITHOUT ADJUSTING <soc/pll.h> CONSTANTS!
	 *************/
	for (i = 0; i < ARRAY_SIZE(mux_sels); i++)
		pll_mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

	/* enable scpsys clock off control */
	write32(&mtk_topckgen->clk_scp_cfg_0,
		(1 << 10) | (1 << 9) | (1 << 5) | (1 << 4) | (1 << 2) |
		(1 << 1) | (1 << 0));
	write32(&mtk_topckgen->clk_scp_cfg_1,
		(1 << 4) | (1 << 2) | (1 << 0));
}

/* Turn on ADA_SSUSB_XTAL_CK 26MHz */
void mt_pll_enable_ssusb_clk(void)
{
	/* set  RG_LTECLKSQ_EN */
	setbits32(&mtk_apmixed->ap_pll_con0, 0x1);
	udelay(100);  /* wait for PLL stable */

	/* set RG_LTECLKSQ_LPF_EN & DA_REF2USB_TX_EN */
	setbits32(&mtk_apmixed->ap_pll_con0, 0x1 << 1);
	setbits32(&mtk_apmixed->ap_pll_con2, 0x1);
	udelay(100);  /* wait for PLL stable */

	/* set DA_REF2USB_TX_LPF_EN & DA_REF2USB_TX_OUT_EN */
	setbits32(&mtk_apmixed->ap_pll_con2, (0x1 << 2) | (0x1 << 1));
}

/* after pmic_init */
void mt_pll_post_init(void)
{
	/* CPU clock divide by 1 */
	clrbits32(&mt8173_infracfg->top_ckdiv1, 0x3ff);

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
		clrbits32(&mtk_topckgen->clk_auddiv_0, 1 << 5);
		clrsetbits32(&mtk_topckgen->clk_auddiv_1, 0xff << 8,
			     mclk_div << 8);
		/* bclk */
		clrsetbits32(&mtk_topckgen->clk_auddiv_0, 0xf << 24,
			     7 << 24);
	} else {
		/* mclk */
		setbits32(&mtk_topckgen->clk_auddiv_0, 1 << 5);
		clrsetbits32(&mtk_topckgen->clk_auddiv_2, 0xff << 8,
			     mclk_div << 8);
		/* bclk */
		clrsetbits32(&mtk_topckgen->clk_auddiv_0, 0xf << 28,
			     7 << 28);
	}
}

void mt_pll_raise_little_cpu_freq(u32 freq)
{
	pll_set_rate(&plls[APMIXED_ARMCA7PLL], freq); /* freq in Hz */
}

void mt_mem_pll_config_pre(const struct mt8173_sdram_params *sdram_params)
{
	u32 mpll_sdm_pcw_20_0 = 0xF13B1;

	/* disable MPLL for adjusting memory clk frequency */
	clrbits32(&mtk_apmixed->mpll_con0, BIT(0));
	/* MPLL configuration: mode selection */
	setbits32(&mtk_apmixed->mpll_con0, BIT(16));
	clrbits32(&mtk_apmixed->mpll_con0, 0x7 << 4);
	clrbits32(&mtk_apmixed->pll_test_con0, 1 << 31);
	/* set RG_MPLL_SDM_PCW for feedback divide ratio */
	clrsetbits32(&mtk_apmixed->mpll_con1, 0x1fffff, mpll_sdm_pcw_20_0);
}

void mt_mem_pll_config_post(void)
{
	/* power up sequence starts: enable MPLL */
	setbits32(&mtk_apmixed->mpll_con0, BIT(0));
}

void mt_mem_pll_mux(void)
{
	/* CLK_CFG_0 */
	pll_mux_set_sel(&muxes[TOP_MEM_SEL], 1); /* 1: dmpll_ck */
}
