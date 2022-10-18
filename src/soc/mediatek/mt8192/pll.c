/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <stddef.h>
#include <timer.h>

#include <soc/addressmap.h>
#include <soc/infracfg.h>
#include <soc/mcucfg.h>
#include <soc/pll.h>
#include <soc/wdt.h>

enum mux_id {
	TOP_AXI_SEL,
	TOP_SPM_SEL,
	TOP_SCP_SEL,
	TOP_BUS_AXIMEM_SEL,
	TOP_DISP_SEL,
	TOP_MDP_SEL,
	TOP_IMG1_SEL,
	TOP_IMG2_SEL,
	TOP_IPE_SEL,
	TOP_DPE_SEL,
	TOP_CAM_SEL,
	TOP_CCU_SEL,
	TOP_DSP_SEL,
	TOP_DSP7_SEL,
	TOP_MFG_REF_SEL,
	TOP_MFG_PLL_SEL,
	TOP_CAMTG_SEL,
	TOP_CAMTG2_SEL,
	TOP_CAMTG3_SEL,
	TOP_CAMTG4_SEL,
	TOP_CAMTG5_SEL,
	TOP_CAMTG6_SEL,
	TOP_UART_SEL,
	TOP_SPI_SEL,
	TOP_MSDC50_0_HCLK_SEL,
	TOP_MSDC50_0_SEL,
	TOP_MSDC30_1_SEL,
	TOP_MSDC30_2_SEL,
	TOP_AUDIO_SEL,
	TOP_AUD_INTBUS_SEL,
	TOP_PWRAP_ULPOSC_SEL,
	TOP_ATB_SEL,
	TOP_PWRMCU_SEL,
	TOP_DPI_SEL,
	TOP_SCAM_SEL,
	TOP_DISP_PWM_SEL,
	TOP_USB_TOP_SEL,
	TOP_SSUSB_XHCI_SEL,
	TOP_I2C_SEL,
	TOP_SENINF_SEL,
	TOP_SENINF1_SEL,
	TOP_SENINF2_SEL,
	TOP_SENINF3_SEL,
	TOP_TL_SEL,
	TOP_DXCC_SEL,
	TOP_AUD_ENGEN1_SEL,
	TOP_AUD_ENGEN2_SEL,
	TOP_AES_UFSFDE_SEL,
	TOP_UFS_SEL,
	TOP_AUD_1_SEL,
	TOP_AUD_2_SEL,
	TOP_ADSP_SEL,
	TOP_DPMAIF_MAIN_SEL,
	TOP_VENC_SEL,
	TOP_VDEC_SEL,
	TOP_CAMTM_SEL,
	TOP_PWM_SEL,
	TOP_AUDIO_H_SEL,
	TOP_SPMI_MST_SEL,
	TOP_DVFSRC_SEL,
	TOP_AES_MSDCFDE_SEL,
	TOP_MCUPM_SEL,
	TOP_SFLASH_SEL,
	TOP_NR_MUX
};

#define MUX(_id, _reg, _mux_shift, _mux_width)		\
	[_id] = {					\
		.reg = &mtk_topckgen->_reg,		\
		.set_reg = &mtk_topckgen->_reg##_set,	\
		.clr_reg = &mtk_topckgen->_reg##_clr,	\
		.mux_shift = _mux_shift,		\
		.mux_width = _mux_width,		\
		.upd_reg = NULL,			\
		.upd_shift = 0,				\
	}

#define MUX_UPD(_id, _reg, _mux_shift, _mux_width, _upd_reg, _upd_shift)\
	[_id] = {							\
		.reg = &mtk_topckgen->_reg,				\
		.set_reg = &mtk_topckgen->_reg##_set,			\
		.clr_reg = &mtk_topckgen->_reg##_clr,			\
		.mux_shift = _mux_shift,				\
		.mux_width = _mux_width,				\
		.upd_reg = &mtk_topckgen->_upd_reg,			\
		.upd_shift = _upd_shift,				\
	}

static const struct mux muxes[] = {
	/* CLK_CFG_0 */
	MUX_UPD(TOP_AXI_SEL, clk_cfg_0, 0, 3, clk_cfg_update, 0),
	MUX_UPD(TOP_SPM_SEL, clk_cfg_0, 8, 2, clk_cfg_update, 1),
	MUX_UPD(TOP_SCP_SEL, clk_cfg_0, 16, 3, clk_cfg_update, 2),
	MUX_UPD(TOP_BUS_AXIMEM_SEL, clk_cfg_0, 24, 3, clk_cfg_update, 3),
	/* CLK_CFG_1 */
	MUX_UPD(TOP_DISP_SEL, clk_cfg_1, 0, 4, clk_cfg_update, 4),
	MUX_UPD(TOP_MDP_SEL, clk_cfg_1, 8, 4, clk_cfg_update, 5),
	MUX_UPD(TOP_IMG1_SEL, clk_cfg_1, 16, 4, clk_cfg_update, 6),
	MUX_UPD(TOP_IMG2_SEL, clk_cfg_1, 24, 4, clk_cfg_update, 7),
	/* CLK_CFG_2 */
	MUX_UPD(TOP_IPE_SEL, clk_cfg_2, 0, 4, clk_cfg_update, 8),
	MUX_UPD(TOP_DPE_SEL, clk_cfg_2, 8, 3, clk_cfg_update, 9),
	MUX_UPD(TOP_CAM_SEL, clk_cfg_2, 16, 4, clk_cfg_update, 10),
	MUX_UPD(TOP_CCU_SEL, clk_cfg_2, 24, 4, clk_cfg_update, 11),
	/* CLK_CFG_3 */
	MUX_UPD(TOP_DSP_SEL, clk_cfg_3, 0, 3, clk_cfg_update, 12),
	/* CLK_CFG_4 */
	MUX_UPD(TOP_DSP7_SEL, clk_cfg_4, 0, 3, clk_cfg_update, 16),
	MUX_UPD(TOP_MFG_REF_SEL, clk_cfg_4, 16, 2, clk_cfg_update, 18),
	MUX(TOP_MFG_PLL_SEL, clk_cfg_4, 18, 1),
	MUX_UPD(TOP_CAMTG_SEL, clk_cfg_4, 24, 3, clk_cfg_update, 19),
	/* CLK_CFG_5 */
	MUX_UPD(TOP_CAMTG2_SEL, clk_cfg_5, 0, 3, clk_cfg_update, 20),
	MUX_UPD(TOP_CAMTG3_SEL, clk_cfg_5, 8, 3, clk_cfg_update, 21),
	MUX_UPD(TOP_CAMTG4_SEL, clk_cfg_5, 16, 3, clk_cfg_update, 22),
	MUX_UPD(TOP_CAMTG5_SEL, clk_cfg_5, 24, 3, clk_cfg_update, 23),
	/* CLK_CFG_6 */
	MUX_UPD(TOP_CAMTG6_SEL, clk_cfg_6, 0, 3, clk_cfg_update, 24),
	MUX_UPD(TOP_UART_SEL, clk_cfg_6, 8, 1, clk_cfg_update, 25),
	MUX_UPD(TOP_SPI_SEL, clk_cfg_6, 16, 2, clk_cfg_update, 26),
	MUX_UPD(TOP_MSDC50_0_HCLK_SEL, clk_cfg_6, 24, 2, clk_cfg_update, 27),
	/* CLK_CFG_7 */
	MUX_UPD(TOP_MSDC50_0_SEL, clk_cfg_7, 0, 3, clk_cfg_update, 28),
	MUX_UPD(TOP_MSDC30_1_SEL, clk_cfg_7, 8, 3, clk_cfg_update, 29),
	MUX_UPD(TOP_MSDC30_2_SEL, clk_cfg_7, 16, 3, clk_cfg_update, 30),
	MUX_UPD(TOP_AUDIO_SEL, clk_cfg_7, 24, 2, clk_cfg_update1, 0),
	/* CLK_CFG_8 */
	MUX_UPD(TOP_AUD_INTBUS_SEL, clk_cfg_8, 0, 2, clk_cfg_update1, 1),
	MUX_UPD(TOP_PWRAP_ULPOSC_SEL, clk_cfg_8, 8, 3, clk_cfg_update1, 2),
	MUX_UPD(TOP_ATB_SEL, clk_cfg_8, 16, 2, clk_cfg_update1, 3),
	MUX_UPD(TOP_PWRMCU_SEL, clk_cfg_8, 24, 3, clk_cfg_update1, 4),
	/* CLK_CFG_9 */
	MUX_UPD(TOP_DPI_SEL, clk_cfg_9, 0, 3, clk_cfg_update1, 5),
	MUX_UPD(TOP_SCAM_SEL, clk_cfg_9, 8, 1, clk_cfg_update1, 6),
	MUX_UPD(TOP_DISP_PWM_SEL, clk_cfg_9, 16, 3, clk_cfg_update1, 7),
	MUX_UPD(TOP_USB_TOP_SEL, clk_cfg_9, 24, 2, clk_cfg_update1, 8),
	/* CLK_CFG_10 */
	MUX_UPD(TOP_SSUSB_XHCI_SEL, clk_cfg_10, 0, 2, clk_cfg_update1, 9),
	MUX_UPD(TOP_I2C_SEL, clk_cfg_10, 8, 2, clk_cfg_update1, 10),
	MUX_UPD(TOP_SENINF_SEL, clk_cfg_10, 16, 3, clk_cfg_update1, 11),
	MUX_UPD(TOP_SENINF1_SEL, clk_cfg_10, 24, 3, clk_cfg_update1, 12),
	/* CLK_CFG_11 */
	MUX_UPD(TOP_SENINF2_SEL, clk_cfg_11, 0, 3, clk_cfg_update1, 13),
	MUX_UPD(TOP_SENINF3_SEL, clk_cfg_11, 8, 3, clk_cfg_update1, 14),
	MUX_UPD(TOP_TL_SEL, clk_cfg_11, 16, 2, clk_cfg_update1, 15),
	MUX_UPD(TOP_DXCC_SEL, clk_cfg_11, 24, 2, clk_cfg_update1, 16),
	/* CLK_CFG_12 */
	MUX_UPD(TOP_AUD_ENGEN1_SEL, clk_cfg_12, 0, 2, clk_cfg_update1, 17),
	MUX_UPD(TOP_AUD_ENGEN2_SEL, clk_cfg_12, 8, 2, clk_cfg_update1, 18),
	MUX_UPD(TOP_AES_UFSFDE_SEL, clk_cfg_12, 16, 3, clk_cfg_update1, 19),
	MUX_UPD(TOP_UFS_SEL, clk_cfg_12, 24, 3, clk_cfg_update1, 20),
	/* CLK_CFG_13 */
	MUX_UPD(TOP_AUD_1_SEL, clk_cfg_13, 0, 1, clk_cfg_update1, 21),
	MUX_UPD(TOP_AUD_2_SEL, clk_cfg_13, 8, 1, clk_cfg_update1, 22),
	MUX_UPD(TOP_ADSP_SEL, clk_cfg_13, 16, 3, clk_cfg_update1, 23),
	MUX_UPD(TOP_DPMAIF_MAIN_SEL, clk_cfg_13, 24, 3, clk_cfg_update1, 24),
	/* CLK_CFG_14 */
	MUX_UPD(TOP_VENC_SEL, clk_cfg_14, 0, 4, clk_cfg_update1, 25),
	MUX_UPD(TOP_VDEC_SEL, clk_cfg_14, 8, 4, clk_cfg_update1, 26),
	MUX_UPD(TOP_CAMTM_SEL, clk_cfg_14, 16, 2, clk_cfg_update1, 27),
	MUX_UPD(TOP_PWM_SEL, clk_cfg_14, 24, 1, clk_cfg_update1, 28),
	/* CLK_CFG_15 */
	MUX_UPD(TOP_AUDIO_H_SEL, clk_cfg_15, 0, 2, clk_cfg_update1, 29),
	MUX_UPD(TOP_SPMI_MST_SEL, clk_cfg_15, 8, 3, clk_cfg_update1, 30),
	MUX_UPD(TOP_DVFSRC_SEL, clk_cfg_15, 16, 1, clk_cfg_update2, 0),
	MUX_UPD(TOP_AES_MSDCFDE_SEL, clk_cfg_15, 24, 3, clk_cfg_update2, 1),
	/* CLK_CFG_16 */
	MUX_UPD(TOP_MCUPM_SEL, clk_cfg_16, 0, 2, clk_cfg_update2, 2),
	MUX_UPD(TOP_SFLASH_SEL, clk_cfg_16, 8, 2, clk_cfg_update2, 3),
};

struct mux_sel {
	enum mux_id id;
	u32 sel;
};

static const struct mux_sel mux_sels[] = {
	/* CLK_CFG_0 */
	{ .id = TOP_AXI_SEL, .sel = 2 },		/* 2: mainpll_d7_d2 */
	{ .id = TOP_SPM_SEL, .sel = 2 },		/* 2: mainpll_d7_d4 */
	{ .id = TOP_SCP_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_BUS_AXIMEM_SEL, .sel = 3 },		/* 3: mainpll_d5_d2 */
	/* CLK_CFG_1 */
	{ .id = TOP_DISP_SEL, .sel = 8 },		/* 8: mainpll_d4 */
	{ .id = TOP_MDP_SEL, .sel = 8 },		/* 8: tvdpll_ck */
	{ .id = TOP_IMG1_SEL, .sel = 1 },		/* 1: univpll_d4 */
	{ .id = TOP_IMG2_SEL, .sel = 1 },		/* 1: univpll_d4 */
	/* CLK_CFG_2 */
	{ .id = TOP_IPE_SEL, .sel = 1 },		/* 1: mainpll_d4 */
	{ .id = TOP_DPE_SEL, .sel = 1 },		/* 1: mainpll_d4 */
	{ .id = TOP_CAM_SEL, .sel = 3 },		/* 3: univpll_d4 */
	{ .id = TOP_CCU_SEL, .sel = 8 },		/* 8: univpll_d5 */
	/* CLK_CFG_3 */
	{ .id = TOP_DSP_SEL, .sel = 1 },		/* 1: univpll_d6_d2 */
	/* CLK_CFG_4 */
	{ .id = TOP_DSP7_SEL, .sel = 1 },		/* 1: mainpll_d4_d2 */
	{ .id = TOP_MFG_REF_SEL, .sel = 3 },		/* 3: mainpll_d5_d2 */
	{ .id = TOP_MFG_PLL_SEL, .sel = 1 },		/* 1: mfgpll */
	{ .id = TOP_CAMTG_SEL, .sel = 1 },		/* 1: univpll_192m_d8 */
	/* CLK_CFG_5 */
	{ .id = TOP_CAMTG2_SEL, .sel = 1 },		/* 1: univpll_192m_d8 */
	{ .id = TOP_CAMTG3_SEL, .sel = 1 },		/* 1: univpll_192m_d8 */
	{ .id = TOP_CAMTG4_SEL, .sel = 1 },		/* 1: univpll_192m_d8 */
	{ .id = TOP_CAMTG5_SEL, .sel = 1 },		/* 1: univpll_192m_d8 */
	/* CLK_CFG_6 */
	{ .id = TOP_CAMTG6_SEL, .sel = 1 },		/* 1: univpll_192m_d8 */
	{ .id = TOP_UART_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_SPI_SEL, .sel = 1 },		/* 1: mainpll_d5_d4 */
	{ .id = TOP_MSDC50_0_HCLK_SEL, .sel = 1 },	/* 1: mainpll_d4_d2 */
	/* CLK_CFG_7 */
	{ .id = TOP_MSDC50_0_SEL, .sel = 1 },		/* 1: msdcpll_ck */
	{ .id = TOP_MSDC30_1_SEL, .sel = 4 },		/* 4: msdcpll_d2 */
	{ .id = TOP_MSDC30_2_SEL, .sel = 4 },		/* 4: msdcpll_d2 */
	{ .id = TOP_AUDIO_SEL, .sel = 0 },		/* 0: clk26m */
	/* CLK_CFG_8 */
	{ .id = TOP_AUD_INTBUS_SEL, .sel = 1 },		/* 1: mainpll_d4_d4 */
	{ .id = TOP_PWRAP_ULPOSC_SEL, .sel = 0 },	/* 0: osc_d10 */
	{ .id = TOP_ATB_SEL, .sel = 1 },		/* 1: mainpll_d4_d2 */
	{ .id = TOP_PWRMCU_SEL, .sel = 3 },		/* 3: mainpll_d4_d2 */
	/* CLK_CFG_9 */
	{ .id = TOP_DPI_SEL, .sel = 1 },		/* 1: tvdpll_d2 */
	{ .id = TOP_SCAM_SEL, .sel = 1 },		/* 1: mainpll_d5_d4 */
	{ .id = TOP_DISP_PWM_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_USB_TOP_SEL, .sel = 1 },		/* 1: univpll_d5_d4 */
	/* CLK_CFG_10 */
	{ .id = TOP_SSUSB_XHCI_SEL, .sel = 1 },		/* 1: univpll_d5_d4 */
	{ .id = TOP_I2C_SEL, .sel = 2 },		/* 2: univpll_d5_d4 */
	{ .id = TOP_SENINF_SEL, .sel = 4 },		/* 4: univpll_d7 */
	{ .id = TOP_SENINF1_SEL, .sel = 4 },		/* 4: univpll_d7 */
	/* CLK_CFG_11 */
	{ .id = TOP_SENINF2_SEL, .sel = 4 },		/* 4: univpll_d7 */
	{ .id = TOP_SENINF3_SEL, .sel = 4 },		/* 4: univpll_d7 */
	{ .id = TOP_TL_SEL, .sel = 1 },			/* 1: univpll_192m_d2 */
	{ .id = TOP_DXCC_SEL, .sel = 1 },		/* 1: mainpll_d4_d2 */
	/* CLK_CFG_12 */
	{ .id = TOP_AUD_ENGEN1_SEL, .sel = 2 },		/* 2: apll1_d4 */
	{ .id = TOP_AUD_ENGEN2_SEL, .sel = 2 },		/* 2: apll2_d4 */
	{ .id = TOP_AES_UFSFDE_SEL, .sel = 6 },		/* 6: univpll_d6 */
	{ .id = TOP_UFS_SEL, .sel = 6 },		/* 6: msdcpll_d2 */
	/* CLK_CFG_13 */
	{ .id = TOP_AUD_1_SEL, .sel = 1 },		/* 1: apll1_ck */
	{ .id = TOP_AUD_2_SEL, .sel = 1 },		/* 1: apll2_ck */
	{ .id = TOP_ADSP_SEL, .sel = 7 },		/* 7: adsppll_ck */
	{ .id = TOP_DPMAIF_MAIN_SEL, .sel = 3 },	/* 3: mainpll_d4_d2 */
	/* CLK_CFG_14 */
	{ .id = TOP_VENC_SEL, .sel = 14 },		/* 14: univpll_d5_d2 */
	{ .id = TOP_VDEC_SEL, .sel = 4 },		/* 4: mainpll_d5_d2 */
	{ .id = TOP_CAMTM_SEL, .sel = 2 },		/* 2: univpll_d6_d2 */
	{ .id = TOP_PWM_SEL, .sel = 0 },		/* 0: clk26m */
	/* CLK_CFG_15 */
	{ .id = TOP_AUDIO_H_SEL, .sel = 3 },		/* 3: apll2_ck */
	{ .id = TOP_SPMI_MST_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_DVFSRC_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_AES_MSDCFDE_SEL, .sel = 5 },	/* 5: univpll_d6 */
	/* CLK_CFG_16 */
	{ .id = TOP_MCUPM_SEL, .sel = 2 },		/* 2: mainpll_d6_d2 */
	{ .id = TOP_SFLASH_SEL, .sel = 1 },		/* 1: mainpll_d7_d8 */
};

enum pll_id {
	APMIXED_ARMPLL_LL,
	APMIXED_ARMPLL_BL,
	APMIXED_CCIPLL,
	APMIXED_MAINPLL,
	APMIXED_UNIVPLL,
	APMIXED_USBPLL,
	APMIXED_MSDCPLL,
	APMIXED_MMPLL,
	APMIXED_ADSPPLL,
	APMIXED_MFGPLL,
	APMIXED_TVDPLL,
	APMIXED_APLL1,
	APMIXED_APLL2,
	APMIXED_PLL_MAX
};

const u32 pll_div_rate[] = {
	3800UL * MHz,
	1900 * MHz,
	950 * MHz,
	475 * MHz,
	237500 * KHz,
	0,
};

static const struct pll plls[] = {
	PLL(APMIXED_ARMPLL_LL, armpll_ll_con0, armpll_ll_con3,
		NO_RSTB_SHIFT, 22, armpll_ll_con1, 24, armpll_ll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_ARMPLL_BL, armpll_bl0_con0, armpll_bl_con3,
		NO_RSTB_SHIFT, 22, armpll_bl_con1, 24, armpll_bl_con1, 0,
		pll_div_rate),
	PLL(APMIXED_CCIPLL, ccipll_con0, ccipll_con3,
		NO_RSTB_SHIFT, 22, ccipll_con1, 24, ccipll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MAINPLL, mainpll_con0, mainpll_con3,
		23, 22, mainpll_con1, 24, mainpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_UNIVPLL, univpll_con0, univpll_con3,
		23, 22, univpll_con1, 24, univpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_USBPLL, usbpll_con0, usbpll_con2,
		NO_RSTB_SHIFT, 22, usbpll_con0, 24, usbpll_con0, 0,
		pll_div_rate),
	PLL(APMIXED_MSDCPLL, msdcpll_con0, msdcpll_con3,
		NO_RSTB_SHIFT, 22, msdcpll_con1, 24, msdcpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MMPLL, mmpll_con0, mmpll_con3,
		23, 22, mmpll_con1, 24, mmpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_ADSPPLL, adsppll_con0, adsppll_con3,
		NO_RSTB_SHIFT, 22, adsppll_con1, 24, adsppll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_MFGPLL, mfgpll_con0, mfgpll_con3,
		NO_RSTB_SHIFT, 22, mfgpll_con1, 24, mfgpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_TVDPLL, tvdpll_con0, tvdpll_con3,
		NO_RSTB_SHIFT, 22, tvdpll_con1, 24, tvdpll_con1, 0,
		pll_div_rate),
	PLL(APMIXED_APLL1, apll1_con0, apll1_con4,
		NO_RSTB_SHIFT, 32, apll1_con1, 24, apll1_con2, 0,
		pll_div_rate),
	PLL(APMIXED_APLL2, apll2_con0, apll2_con4,
		NO_RSTB_SHIFT, 32, apll2_con1, 24, apll2_con2, 0,
		pll_div_rate),
};

struct rate {
	enum pll_id id;
	u32 rate;
};

static const struct rate rates[] = {
	{ .id = APMIXED_ARMPLL_LL, .rate = ARMPLL_LL_HZ },
	{ .id = APMIXED_ARMPLL_BL, .rate = ARMPLL_BL_HZ },
	{ .id = APMIXED_CCIPLL, .rate = CCIPLL_HZ },
	{ .id = APMIXED_MAINPLL, .rate = MAINPLL_HZ },
	{ .id = APMIXED_UNIVPLL, .rate = UNIVPLL_HZ },
	{ .id = APMIXED_USBPLL, .rate = USBPLL_HZ },
	{ .id = APMIXED_MSDCPLL, .rate = MSDCPLL_HZ },
	{ .id = APMIXED_MMPLL, .rate = MMPLL_HZ },
	{ .id = APMIXED_ADSPPLL, .rate = ADSPPLL_HZ },
	{ .id = APMIXED_MFGPLL, .rate = MFGPLL_HZ },
	{ .id = APMIXED_TVDPLL, .rate = TVDPLL_HZ },
	{ .id = APMIXED_APLL1, .rate = APLL1_HZ },
	{ .id = APMIXED_APLL2, .rate = APLL2_HZ },
};

void pll_set_pcw_change(const struct pll *pll)
{
	setbits32(pll->div_reg, PLL_PCW_CHG);
}

void mt_pll_init(void)
{
	int i;

	/* enable clock square1 low-pass filter */
	setbits32(&mtk_apmixed->ap_pll_con0, 0x2);

	/* reduce PLL current */
	SET32_BITFIELDS(&mtk_apmixed->ap_pllgp1_con1, PLLGP1_LVRREF, 1);
	SET32_BITFIELDS(&mtk_apmixed->ap_pllgp2_con1, PLLGP2_LVRREF, 1);

	/* xPLL PWR ON */
	for (i = 0; i < APMIXED_PLL_MAX; i++)
		setbits32(plls[i].pwr_reg, PLL_PWR_ON);

	udelay(PLL_PWR_ON_DELAY);

	/* xPLL ISO Disable */
	for (i = 0; i < APMIXED_PLL_MAX; i++)
		clrbits32(plls[i].pwr_reg, PLL_ISO);

	udelay(PLL_ISO_DELAY);

	/* xPLL Frequency Set */
	for (i = 0; i < ARRAY_SIZE(rates); i++)
		pll_set_rate(&plls[rates[i].id], rates[i].rate);

	/* AUDPLL Tuner Frequency Set */
	write32(&mtk_apmixed->apll1_tuner_con0, read32(&mtk_apmixed->apll1_con2) + 1);
	write32(&mtk_apmixed->apll2_tuner_con0, read32(&mtk_apmixed->apll2_con2) + 1);

	/* xPLL Frequency Enable */
	for (i = 0; i < APMIXED_PLL_MAX; i++) {
		if (i == APMIXED_USBPLL)
			setbits32(plls[APMIXED_USBPLL].pwr_reg, USBPLL_EN);
		else
			setbits32(plls[i].reg, PLL_EN);
	}

	/* wait for PLL stable */
	udelay(PLL_EN_DELAY);

	/* xPLL DIV Enable & RSTB */
	for (i = 0; i < APMIXED_PLL_MAX; i++) {
		if (plls[i].rstb_shift != NO_RSTB_SHIFT) {
			setbits32(plls[i].reg, PLL_DIV_EN);
			setbits32(plls[i].reg, 1 << plls[i].rstb_shift);
		}
	}

	/* MCUCFG CLKMUX */
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_DIV_MASK, MCU_DIV_1);
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg1, MCU_DIV_MASK, MCU_DIV_1);
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_DIV_MASK, MCU_DIV_1);

	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg1, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_PLL);

	/* enable infrasys DCM */
	setbits32(&mt8192_infracfg->infra_bus_dcm_ctrl, 0x3 << 21);

	/* dcm_infracfg_ao_aximem_bus_dcm */
	clrsetbits32(&mt8192_infracfg->infra_aximem_idle_bit_en_0,
		     INFRACFG_AO_AXIMEM_BUS_DCM_REG0_MASK,
		     INFRACFG_AO_AXIMEM_BUS_DCM_REG0_ON);
	/* dcm_infracfg_ao_infra_bus_dcm */
	clrsetbits32(&mt8192_infracfg->infra_bus_dcm_ctrl,
		     INFRACFG_AO_INFRA_BUS_DCM_REG0_MASK,
		     INFRACFG_AO_INFRA_BUS_DCM_REG0_ON);
	/* dcm_infracfg_ao_infra_conn_bus_dcm */
	clrsetbits32(&mt8192_infracfg->module_sw_cg_2_set,
		     INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_MASK,
		     INFRACFG_AO_INFRA_CONN_BUS_DCM_REG0_ON);
	clrsetbits32(&mt8192_infracfg->module_sw_cg_2_clr,
		     INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_MASK,
		     INFRACFG_AO_INFRA_CONN_BUS_DCM_REG1_ON);
	/* dcm_infracfg_ao_infra_rx_p2p_dcm */
	clrsetbits32(&mt8192_infracfg->p2p_rx_clk_on,
		     INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_MASK,
		     INFRACFG_AO_INFRA_RX_P2P_DCM_REG0_ON);
	/* dcm_infracfg_ao_peri_bus_dcm */
	clrsetbits32(&mt8192_infracfg->peri_bus_dcm_ctrl,
		     INFRACFG_AO_PERI_BUS_DCM_REG0_MASK,
		     INFRACFG_AO_PERI_BUS_DCM_REG0_ON);
	/* dcm_infracfg_ao_peri_module_dcm */
	clrsetbits32(&mt8192_infracfg->peri_bus_dcm_ctrl,
		     INFRACFG_AO_PERI_MODULE_DCM_REG0_MASK,
		     INFRACFG_AO_PERI_MODULE_DCM_REG0_ON);

	/* initialize SPM request */
	setbits32(&mtk_topckgen->clk_scp_cfg_0, 0x3ff);
	clrsetbits32(&mtk_topckgen->clk_scp_cfg_1, 0x100c, 0x3);

	/*
	 * TOP CLKMUX -- DO NOT CHANGE WITHOUT ADJUSTING <soc/pll.h> CONSTANTS!
	 */
	for (i = 0; i < ARRAY_SIZE(mux_sels); i++)
		pll_mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

	/* enable [14] dramc_pll104m_ck */
	setbits32(&mtk_topckgen->clk_misc_cfg_0, 1 << 14);

	/* reset CONNSYS MCU */
	SET32_BITFIELDS(&mtk_wdt->wdt_swsysrst,
			WDT_SWSYSRST_KEY, 0x88,
			WDT_SWSYSRST_CONN_MCU, 0x1);
}

void mt_pll_raise_little_cpu_freq(u32 freq)
{
	/* enable [4] intermediate clock armpll_divider_pll1_ck */
	setbits32(&mtk_topckgen->clk_misc_cfg_0, 1 << 4);

	/* switch ca55 clock source to intermediate clock */
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_DIV_PLL1);

	/* disable armpll_ll frequency output */
	clrbits32(plls[APMIXED_ARMPLL_LL].reg, PLL_EN);

	/* raise armpll_ll frequency */
	pll_set_rate(&plls[APMIXED_ARMPLL_LL], freq);

	/* enable armpll_ll frequency output */
	setbits32(plls[APMIXED_ARMPLL_LL].reg, PLL_EN);
	udelay(PLL_EN_DELAY);

	/* switch ca55 clock source back to armpll_ll */
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_PLL);

	/* disable [4] intermediate clock armpll_divider_pll1_ck */
	clrbits32(&mtk_topckgen->clk_misc_cfg_0, 1 << 4);
}

u32 mt_fmeter_get_freq_khz(enum fmeter_type type, u32 id)
{
	u32 output, count, clk_dbg_cfg, clk_misc_cfg_0;

	/* backup */
	clk_dbg_cfg = read32(&mtk_topckgen->clk_dbg_cfg);
	clk_misc_cfg_0 = read32(&mtk_topckgen->clk_misc_cfg_0);

	/* set up frequency meter */
	if (type == FMETER_ABIST) {
		SET32_BITFIELDS(&mtk_topckgen->clk_dbg_cfg,
				CLK_DBG_CFG_ABIST_CK_SEL, id,
				CLK_DBG_CFG_CKGEN_CK_SEL, 0,
				CLK_DBG_CFG_METER_CK_SEL, 0);
		SET32_BITFIELDS(&mtk_topckgen->clk_misc_cfg_0,
				CLK_MISC_CFG_0_METER_DIV, 1);
	} else if (type == FMETER_CKGEN) {
		SET32_BITFIELDS(&mtk_topckgen->clk_dbg_cfg,
				CLK_DBG_CFG_ABIST_CK_SEL, 0,
				CLK_DBG_CFG_CKGEN_CK_SEL, id,
				CLK_DBG_CFG_METER_CK_SEL, 1);
		SET32_BITFIELDS(&mtk_topckgen->clk_misc_cfg_0,
				CLK_MISC_CFG_0_METER_DIV, 0);
	} else {
		die("unsupported fmeter type\n");
	}

	/* enable frequency meter */
	write32(&mtk_topckgen->clk26cali_0, 0x1000);

	/* set load count = 1024-1 */
	SET32_BITFIELDS(&mtk_topckgen->clk26cali_1, CLK26CALI_1_LOAD_CNT, 0x3ff);

	/* trigger frequency meter */
	SET32_BITFIELDS(&mtk_topckgen->clk26cali_0, CLK26CALI_0_TRIGGER, 1);

	/* wait frequency meter until finished */
	if (wait_us(200, !READ32_BITFIELD(&mtk_topckgen->clk26cali_0, CLK26CALI_0_TRIGGER))) {
		count = read32(&mtk_topckgen->clk26cali_1) & 0xffff;
		output = (count * 26000) / 1024; /* KHz */
	} else {
		printk(BIOS_WARNING, "fmeter timeout\n");
		output = 0;
	}

	/* disable frequency meter */
	write32(&mtk_topckgen->clk26cali_0, 0x0000);

	/* restore */
	write32(&mtk_topckgen->clk_dbg_cfg, clk_dbg_cfg);
	write32(&mtk_topckgen->clk_misc_cfg_0, clk_misc_cfg_0);

	if (type == FMETER_ABIST)
		return output * 2;
	else if (type == FMETER_CKGEN)
		return output;

	return 0;
}

void mt_pll_raise_cci_freq(u32 freq)
{
	/* enable [4] intermediate clock armpll_divider_pll1_ck */
	setbits32(&mtk_topckgen->clk_misc_cfg_0, 1 << 4);

	/* switch cci clock source to intermediate clock */
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_DIV_PLL1);

	/* disable ccipll frequency output */
	clrbits32(plls[APMIXED_CCIPLL].reg, PLL_EN);

	/* raise ccipll frequency */
	pll_set_rate(&plls[APMIXED_CCIPLL], freq);

	/* enable ccipll frequency output */
	setbits32(plls[APMIXED_CCIPLL].reg, PLL_EN);
	udelay(PLL_EN_DELAY);

	/* switch cci clock source back to ccipll */
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_PLL);

	/* disable [4] intermediate clock armpll_divider_pll1_ck */
	clrbits32(&mtk_topckgen->clk_misc_cfg_0, 1 << 4);
}
