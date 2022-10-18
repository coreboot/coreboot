/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.2
 */

#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <stddef.h>
#include <timer.h>

#include <soc/addressmap.h>
#include <soc/infracfg.h>
#include <soc/mcucfg.h>
#include <soc/pll.h>

enum mux_id {
	TOP_AXI_SEL,
	TOP_SCP_SEL,
	TOP_MFG_SEL,
	TOP_CAMTG_SEL,
	TOP_CAMTG1_SEL,
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
	TOP_AUDIO_SEL,
	TOP_AUD_INTBUS_SEL,
	TOP_AUD_1_SEL,
	TOP_AUD_2_SEL,
	TOP_AUD_ENGEN1_SEL,
	TOP_AUD_ENGEN2_SEL,
	TOP_DISP_PWM_SEL,
	TOP_SSPM_SEL,
	TOP_DXCC_SEL,
	TOP_USB_TOP_SEL,
	TOP_SRCK_SEL,
	TOP_SPM_SEL,
	TOP_I2C_SEL,
	TOP_PWM_SEL,
	TOP_SENINF_SEL,
	TOP_SENINF1_SEL,
	TOP_SENINF2_SEL,
	TOP_SENINF3_SEL,
	TOP_AES_MSDCFDE_SEL,
	TOP_PWRAP_ULPOSC_SEL,
	TOP_CAMTM_SEL,
	TOP_VENC_SEL,
	TOP_CAM_SEL,
	TOP_IMG1_SEL,
	TOP_IPE_SEL,
	TOP_DPMAIF_SEL,
	TOP_VDEC_SEL,
	TOP_DISP_SEL,
	TOP_MDP_SEL,
	TOP_AUDIO_H_SEL,
	TOP_UFS_SEL,
	TOP_AES_FDE_SEL,
	TOP_AUDIODSP_SEL,
	TOP_DVFSRC_SEL,
	TOP_DSI_OCC_SEL,
	TOP_SPMI_MST_SEL,
	TOP_SPINOR_SEL,
	TOP_NNA_SEL,
	TOP_NNA1_SEL,
	TOP_NNA2_SEL,
	TOP_SSUSB_XHCI_SEL,
	TOP_SSUSB_TOP_1P_SEL,
	TOP_SSUSB_XHCI_1P_SEL,
	TOP_WPE_SEL,
	TOP_MEM_SEL,
	TOP_DPI_SEL,
	TOP_U3_OCC_250M_SEL,
	TOP_U3_OCC_500M_SEL,
	TOP_ADSP_BUS_SEL,
	TOP_NR_MUX
};

#define MUX(_id, _reg, _mux_shift, _mux_width)		\
	[_id] = {					\
		.reg = &mtk_topckgen->_reg,		\
		.mux_shift = _mux_shift,		\
		.mux_width = _mux_width,		\
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
	MUX_UPD(TOP_AXI_SEL, clk_cfg_0, 0, 2, clk_cfg_update, 0),
	MUX_UPD(TOP_SCP_SEL, clk_cfg_0, 8, 3, clk_cfg_update, 1),
	MUX_UPD(TOP_MFG_SEL, clk_cfg_0, 16, 2, clk_cfg_update, 2),
	MUX_UPD(TOP_CAMTG_SEL, clk_cfg_0, 24, 3, clk_cfg_update, 3),
	/* CLK_CFG_1 */
	MUX_UPD(TOP_CAMTG1_SEL, clk_cfg_1, 0, 3, clk_cfg_update, 4),
	MUX_UPD(TOP_CAMTG2_SEL, clk_cfg_1, 8, 3, clk_cfg_update, 5),
	MUX_UPD(TOP_CAMTG3_SEL, clk_cfg_1, 16, 3, clk_cfg_update, 6),
	MUX_UPD(TOP_CAMTG4_SEL, clk_cfg_1, 24, 3, clk_cfg_update, 7),
	/* CLK_CFG_2 */
	MUX_UPD(TOP_CAMTG5_SEL, clk_cfg_2, 0, 3, clk_cfg_update, 8),
	MUX_UPD(TOP_CAMTG6_SEL, clk_cfg_2, 8, 3, clk_cfg_update, 9),
	MUX_UPD(TOP_UART_SEL, clk_cfg_2, 16, 1, clk_cfg_update, 10),
	MUX_UPD(TOP_SPI_SEL, clk_cfg_2, 24, 3, clk_cfg_update, 11),
	/* CLK_CFG_3 */
	MUX_UPD(TOP_MSDC50_0_HCLK_SEL, clk_cfg_3, 0, 2, clk_cfg_update, 12),
	MUX_UPD(TOP_MSDC50_0_SEL, clk_cfg_3, 8, 3, clk_cfg_update, 13),
	MUX_UPD(TOP_MSDC30_1_SEL, clk_cfg_3, 16, 3, clk_cfg_update, 14),
	MUX_UPD(TOP_AUDIO_SEL, clk_cfg_3, 24, 2, clk_cfg_update, 15),
	/* CLK_CFG_4 */
	MUX_UPD(TOP_AUD_INTBUS_SEL, clk_cfg_4, 0, 2, clk_cfg_update, 16),
	MUX_UPD(TOP_AUD_1_SEL, clk_cfg_4, 8, 1, clk_cfg_update, 17),
	MUX_UPD(TOP_AUD_2_SEL, clk_cfg_4, 16, 1, clk_cfg_update, 18),
	MUX_UPD(TOP_AUD_ENGEN1_SEL, clk_cfg_4, 24, 2, clk_cfg_update, 19),
	/* CLK_CFG_5 */
	MUX_UPD(TOP_AUD_ENGEN2_SEL, clk_cfg_5, 0, 2, clk_cfg_update, 20),
	MUX_UPD(TOP_DISP_PWM_SEL, clk_cfg_5, 8, 3, clk_cfg_update, 21),
	MUX_UPD(TOP_SSPM_SEL, clk_cfg_5, 16, 3, clk_cfg_update, 22),
	MUX_UPD(TOP_DXCC_SEL, clk_cfg_5, 24, 2, clk_cfg_update, 23),
	/* CLK_CFG_6 */
	MUX_UPD(TOP_USB_TOP_SEL, clk_cfg_6, 0, 2, clk_cfg_update, 24),
	MUX_UPD(TOP_SRCK_SEL, clk_cfg_6, 8, 2, clk_cfg_update, 25),
	MUX_UPD(TOP_SPM_SEL, clk_cfg_6, 16, 2, clk_cfg_update, 26),
	MUX_UPD(TOP_I2C_SEL, clk_cfg_6, 24, 2, clk_cfg_update, 27),
	/* CLK_CFG_7 */
	MUX_UPD(TOP_PWM_SEL, clk_cfg_7, 0, 2, clk_cfg_update, 28),
	MUX_UPD(TOP_SENINF_SEL, clk_cfg_7, 8, 2, clk_cfg_update, 29),
	MUX_UPD(TOP_SENINF1_SEL, clk_cfg_7, 16, 2, clk_cfg_update, 30),
	MUX_UPD(TOP_SENINF2_SEL, clk_cfg_7, 24, 2, clk_cfg_update1, 0),
	/* CLK_CFG_8 */
	MUX_UPD(TOP_SENINF3_SEL, clk_cfg_8, 0, 2, clk_cfg_update1, 1),
	MUX_UPD(TOP_AES_MSDCFDE_SEL, clk_cfg_8, 8, 3, clk_cfg_update1, 2),
	MUX_UPD(TOP_PWRAP_ULPOSC_SEL, clk_cfg_8, 16, 3, clk_cfg_update1, 3),
	MUX_UPD(TOP_CAMTM_SEL, clk_cfg_8, 24, 2, clk_cfg_update1, 4),
	/* CLK_CFG_9 */
	MUX_UPD(TOP_VENC_SEL, clk_cfg_9, 0, 3, clk_cfg_update1, 5),
	MUX_UPD(TOP_CAM_SEL, clk_cfg_9, 8, 4, clk_cfg_update1, 6),
	MUX_UPD(TOP_IMG1_SEL, clk_cfg_9, 16, 4, clk_cfg_update1, 7),
	MUX_UPD(TOP_IPE_SEL, clk_cfg_9, 24, 4, clk_cfg_update1, 8),
	/* CLK_CFG_10 */
	MUX_UPD(TOP_DPMAIF_SEL, clk_cfg_10, 0, 3, clk_cfg_update1, 9),
	MUX_UPD(TOP_VDEC_SEL, clk_cfg_10, 8, 3, clk_cfg_update1, 10),
	MUX_UPD(TOP_DISP_SEL, clk_cfg_10, 16, 4, clk_cfg_update1, 11),
	MUX_UPD(TOP_MDP_SEL, clk_cfg_10, 24, 4, clk_cfg_update1, 12),
	/* CLK_CFG_11 */
	MUX_UPD(TOP_AUDIO_H_SEL, clk_cfg_11, 0, 2, clk_cfg_update1, 13),
	MUX_UPD(TOP_UFS_SEL, clk_cfg_11, 8, 2, clk_cfg_update1, 14),
	MUX_UPD(TOP_AES_FDE_SEL, clk_cfg_11, 16, 2, clk_cfg_update1, 15),
	MUX_UPD(TOP_AUDIODSP_SEL, clk_cfg_11, 24, 3, clk_cfg_update1, 16),
	/* CLK_CFG_12 */
	MUX_UPD(TOP_DSI_OCC_SEL, clk_cfg_12, 8, 2, clk_cfg_update1, 18),
	MUX_UPD(TOP_SPMI_MST_SEL, clk_cfg_12, 16, 3, clk_cfg_update1, 19),
	/* CLK_CFG_13 */
	MUX_UPD(TOP_SPINOR_SEL, clk_cfg_13, 0, 3, clk_cfg_update1, 20),
	MUX_UPD(TOP_NNA_SEL, clk_cfg_13, 7, 4, clk_cfg_update1, 21),
	MUX_UPD(TOP_NNA1_SEL, clk_cfg_13, 15, 4, clk_cfg_update1, 22),
	MUX_UPD(TOP_NNA2_SEL, clk_cfg_13, 23, 4, clk_cfg_update1, 23),
	/* CLK_CFG_14 */
	MUX_UPD(TOP_SSUSB_XHCI_SEL, clk_cfg_14, 0, 2, clk_cfg_update1, 24),
	MUX_UPD(TOP_SSUSB_TOP_1P_SEL, clk_cfg_14, 6, 2, clk_cfg_update1, 25),
	MUX_UPD(TOP_SSUSB_XHCI_1P_SEL, clk_cfg_14, 12, 2, clk_cfg_update1, 26),
	MUX_UPD(TOP_WPE_SEL, clk_cfg_14, 18, 4, clk_cfg_update1, 27),
	/* CLK_CFG_15 */
	MUX_UPD(TOP_DPI_SEL, clk_cfg_15, 0, 3, clk_cfg_update1, 28),
	MUX_UPD(TOP_U3_OCC_250M_SEL, clk_cfg_15, 7, 1, clk_cfg_update1, 29),
	MUX_UPD(TOP_U3_OCC_500M_SEL, clk_cfg_15, 12, 1, clk_cfg_update1, 30),
	MUX_UPD(TOP_ADSP_BUS_SEL, clk_cfg_15, 17, 3, clk_cfg_update1, 31),
};

struct mux_sel {
	enum mux_id id;
	u32 sel;
};

static const struct mux_sel mux_sels[] = {
	/* CLK_CFG_0 */
	{ .id = TOP_AXI_SEL, .sel = 1 },		/* 1: mainpll_d7 */
	{ .id = TOP_SCP_SEL, .sel = 3 },		/* 3: mainpll_d2_d2 */
	{ .id = TOP_MFG_SEL, .sel = 1 },		/* 1: mfgpll_ck */
	{ .id = TOP_CAMTG_SEL, .sel = 2 },		/* 2: univpll_d3_d8 */
	/* CLK_CFG_1 */
	{ .id = TOP_CAMTG1_SEL, .sel = 2 },		/* 2: univpll_d3_d8 */
	{ .id = TOP_CAMTG2_SEL, .sel = 2 },		/* 2: univpll_d3_d8 */
	{ .id = TOP_CAMTG3_SEL, .sel = 2 },		/* 2: univpll_d3_d8 */
	{ .id = TOP_CAMTG4_SEL, .sel = 2 },		/* 2: univpll_d3_d8 */
	/* CLK_CFG_2 */
	{ .id = TOP_CAMTG5_SEL, .sel = 2 },		/* 2: univpll_d3_d8 */
	{ .id = TOP_CAMTG6_SEL, .sel = 2 },		/* 2: univpll_d3_d8 */
	{ .id = TOP_UART_SEL, .sel = 0 },		/* 0: clk26m */
	{ .id = TOP_SPI_SEL, .sel = 7 },		/* 7: mainpll_d5 */
	/* CLK_CFG_3 */
	{ .id = TOP_MSDC50_0_HCLK_SEL, .sel = 1 },	/* 1: mainpll_d2_d2 */
	{ .id = TOP_MSDC50_0_SEL, .sel = 1 },		/* 1: msdcpll_ck */
	{ .id = TOP_MSDC30_1_SEL, .sel = 1 },		/* 1: msdcpll_d2 */
	{ .id = TOP_AUDIO_SEL, .sel = 1 },		/* 1: mainpll_d5_d4 */
	/* CLK_CFG_4 */
	{ .id = TOP_AUD_INTBUS_SEL, .sel = 1 },		/* 1: mainpll_d2_d4 */
	{ .id = TOP_AUD_1_SEL, .sel = 1 },		/* 1: apll1_ck */
	{ .id = TOP_AUD_2_SEL, .sel = 1 },		/* 1: apll2_ck */
	{ .id = TOP_AUD_ENGEN1_SEL, .sel = 3 },		/* 3: apll1_d8 */
	/* CLK_CFG_5 */
	{ .id = TOP_AUD_ENGEN2_SEL, .sel = 3 },		/* 3: apll2_d8 */
	{ .id = TOP_DISP_PWM_SEL, .sel = 1 },		/* 1: univpll_d5_d2 */
	{ .id = TOP_SSPM_SEL, .sel = 1 },		/* 1: mainpll_d2_d2 */
	{ .id = TOP_DXCC_SEL, .sel = 1 },		/* 1: mainpll_d2_d2 */
	/* CLK_CFG_6 */
	{ .id = TOP_USB_TOP_SEL, .sel = 2 },		/* 2: univpll_d5_d2 */
	{ .id = TOP_SRCK_SEL, .sel = 2 },		/* 2: ulposc1_d10 */
	{ .id = TOP_SPM_SEL, .sel = 3 },		/* 3: mainpll_d7_d2 */
	{ .id = TOP_I2C_SEL, .sel = 3 },		/* 3: univpll_d5_d2 */
	/* CLK_CFG_7 */
	{ .id = TOP_PWM_SEL, .sel = 3 },		/* 3: univpll_d2_d4 */
	{ .id = TOP_SENINF_SEL, .sel = 3 },		/* 3: univpll_d3_d2 */
	{ .id = TOP_SENINF1_SEL, .sel = 3 },		/* 3: univpll_d3_d2 */
	{ .id = TOP_SENINF2_SEL, .sel = 3 },		/* 3: univpll_d3_d2 */
	/* CLK_CFG_8 */
	{ .id = TOP_SENINF3_SEL, .sel = 3 },		/* 3: univpll_d3_d2 */
	{ .id = TOP_AES_MSDCFDE_SEL, .sel = 1 },	/* 1: univpll_d3 */
	{ .id = TOP_PWRAP_ULPOSC_SEL, .sel = 0 },	/* 0: clk26m */
	{ .id = TOP_CAMTM_SEL, .sel = 2 },		/* 2: univpll_d3_d2 */
	/* CLK_CFG_9 */
	{ .id = TOP_VENC_SEL, .sel = 6 },		/* 6: mainpll_d3 */
	{ .id = TOP_CAM_SEL, .sel = 7 },		/* 7: univpll_d2_d2 */
	{ .id = TOP_IMG1_SEL, .sel = 7 },		/* 7: univpll_d2_d2 */
	{ .id = TOP_IPE_SEL, .sel = 7 },		/* 7: univpll_d2_d2 */
	/* CLK_CFG_10 */
	{ .id = TOP_DPMAIF_SEL, .sel = 1 },		/* 1: univpll_d2_d2 */
	{ .id = TOP_VDEC_SEL, .sel = 6 },		/* 6: univpll_d2_d2 */
	{ .id = TOP_DISP_SEL, .sel = 8 },		/* 8: mmpll_ck */
	{ .id = TOP_MDP_SEL, .sel = 8 },		/* 8: mmpll_ck */
	/* CLK_CFG_11 */
	{ .id = TOP_AUDIO_H_SEL, .sel = 3 },		/* 3: apll2_ck */
	{ .id = TOP_UFS_SEL, .sel = 1 },		/* 1: mainpll_d7 */
	{ .id = TOP_AES_FDE_SEL, .sel = 1 },		/* 1: univpll_d3 */
	{ .id = TOP_AUDIODSP_SEL, .sel = 0 },		/* 0: clk26m */
	/* CLK_CFG_12 */
	{ .id = TOP_DSI_OCC_SEL, .sel = 1 },		/* 1: univpll_d3_d2 */
	{ .id = TOP_SPMI_MST_SEL, .sel = 2 },		/* 2: ulposc1_d4 */
	/* CLK_CFG_13 */
	{ .id = TOP_SPINOR_SEL, .sel = 3 },		/* 3: univpll_d3_d8 */
	{ .id = TOP_NNA_SEL, .sel = 14 },		/* 14: nnapll_ck */
	{ .id = TOP_NNA1_SEL, .sel = 14 },		/* 14: nnapll_ck */
	{ .id = TOP_NNA2_SEL, .sel = 15 },		/* 15: nna2pll_ck */
	/* CLK_CFG_14 */
	{ .id = TOP_SSUSB_XHCI_SEL, .sel = 2 },		/* 2: univpll_d5_d2 */
	{ .id = TOP_SSUSB_TOP_1P_SEL, .sel = 2 },	/* 2: univpll_d5_d2 */
	{ .id = TOP_SSUSB_XHCI_1P_SEL, .sel = 2 },	/* 2: univpll_d5_d2 */
	{ .id = TOP_WPE_SEL, .sel = 8 },		/* 8: mmpll_ck */
	/* CLK_CFG_15 */
	{ .id = TOP_DPI_SEL, .sel = 1 },		/* 1: tvdpll_ck */
	{ .id = TOP_U3_OCC_250M_SEL, .sel = 1 },	/* 1: univpll_d5 */
	{ .id = TOP_U3_OCC_500M_SEL, .sel = 1 },	/* 1: nna2pll_d2 */
	{ .id = TOP_ADSP_BUS_SEL, .sel = 3 },		/* 3: mainpll_d2_d2 */
};

enum pll_id {
	APMIXED_ARMPLL_LL,
	APMIXED_ARMPLL_BL,
	APMIXED_CCIPLL,
	APMIXED_MAINPLL,
	APMIXED_UNIV2PLL,
	APMIXED_MSDCPLL,
	APMIXED_MMPLL,
	APMIXED_NNAPLL,
	APMIXED_NNA2PLL,
	APMIXED_ADSPPLL,
	APMIXED_MFGPLL,
	APMIXED_TVDPLL,
	APMIXED_APLL1,
	APMIXED_APLL2,
	APMIXED_PLL_MAX
};

static const u32 pll_div_rate[] = {
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
	PLL(APMIXED_ARMPLL_BL, armpll_bl_con0, armpll_bl_con3,
	    NO_RSTB_SHIFT, 22, armpll_bl_con1, 24, armpll_bl_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_CCIPLL, ccipll_con0, ccipll_con3,
	    NO_RSTB_SHIFT, 22, ccipll_con1, 24, ccipll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_MAINPLL, mainpll_con0, mainpll_con3,
	    23, 22, mainpll_con1, 24, mainpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_UNIV2PLL, univpll_con0, univpll_con3,
	    23, 22, univpll_con1, 24, univpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_MSDCPLL, msdcpll_con0, msdcpll_con3,
	    NO_RSTB_SHIFT, 22, msdcpll_con1, 24, msdcpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_MMPLL, mmpll_con0, mmpll_con3,
	    NO_RSTB_SHIFT, 22, mmpll_con1, 24, mmpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_NNAPLL, nnapll_con0, nnapll_con3,
	    NO_RSTB_SHIFT, 22, nnapll_con1, 24, nnapll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_NNA2PLL, nna2pll_con0, nna2pll_con3,
	    NO_RSTB_SHIFT, 22, nna2pll_con1, 24, nna2pll_con1, 0,
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
	{ .id = APMIXED_UNIV2PLL, .rate = UNIV2PLL_HZ },
	{ .id = APMIXED_MSDCPLL, .rate = MSDCPLL_HZ },
	{ .id = APMIXED_MMPLL, .rate = MMPLL_HZ },
	{ .id = APMIXED_NNAPLL, .rate = NNAPLL_HZ },
	{ .id = APMIXED_NNA2PLL, .rate = NNA2PLL_HZ },
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

	/* enable clock square */
	setbits32(&mtk_apmixed->ap_pll_con0, BIT(0));

	udelay(PLL_CKSQ_ON_DELAY);

	/* enable clock square1 low-pass filter */
	setbits32(&mtk_apmixed->ap_pll_con0, BIT(1));

	/* xPLL PWR ON */
	for (i = 0; i < APMIXED_PLL_MAX; i++)
		setbits32(plls[i].pwr_reg, PLL_PWR_ON);

	udelay(PLL_PWR_ON_DELAY);

	/* xPLL ISO Disable */
	for (i = 0; i < APMIXED_PLL_MAX; i++)
		clrbits32(plls[i].pwr_reg, PLL_ISO);

	udelay(PLL_ISO_DELAY);

	/* disable glitch free if rate < 374MHz */
	for (i = 0; i < ARRAY_SIZE(rates); i++) {
		if (rates[i].rate < 374 * MHz)
			clrbits32(plls[rates[i].id].reg, GLITCH_FREE_EN);
	}

	/* xPLL Frequency Set */
	for (i = 0; i < ARRAY_SIZE(rates); i++)
		pll_set_rate(&plls[rates[i].id], rates[i].rate);

	/* AUDPLL Tuner Frequency Set */
	write32(&mtk_apmixed->apll1_tuner_con0, read32(&mtk_apmixed->apll1_con2) + 1);
	write32(&mtk_apmixed->apll2_tuner_con0, read32(&mtk_apmixed->apll2_con2) + 1);

	/* xPLL Frequency Enable */
	for (i = 0; i < APMIXED_PLL_MAX; i++)
		setbits32(plls[i].reg, MT8186_PLL_EN);

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
	setbits32(&mtk_topckgen->clk_misc_cfg_0, ARMPLL_DIVIDER_PLL1_EN);
	setbits32(&mtk_topckgen->clk_misc_cfg_0, ARMPLL_DIVIDER_PLL2_EN);

	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_DIV_MASK, MCU_DIV_1);
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg1, MCU_DIV_MASK, MCU_DIV_1);
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_DIV_MASK, MCU_DIV_1);

	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg1, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_PLL);

	write32(&mt8186_infracfg_ao->infra_bus_dcm_ctrl, 0x805f0603);
	write32(&mt8186_infracfg_ao->peri_bus_dcm_ctrl, 0xb07f0603);

	/* dcm_infracfg_ao_audio_bus and dcm_infracfg_ao_icusb_bus */
	SET32_BITFIELDS(&mt8186_infracfg_ao->peri_bus_dcm_ctrl,
			INFRACFG_AO_AUDIO_BUS_REG0, 0,
			INFRACFG_AO_ICUSB_BUS_REG0, 0,
			INFRACFG_AO_AUDIO_BUS_REG0, 1,
			INFRACFG_AO_ICUSB_BUS_REG0, 1);

	/* dcm_infracfg_ao_infra_bus */
	SET32_BITFIELDS(&mt8186_infracfg_ao->infra_bus_dcm_ctrl,
			INFRACFG_AO_INFRA_BUS_REG0_0, 0,
			INFRACFG_AO_INFRA_BUS_REG0_1, 0,
			INFRACFG_AO_INFRA_BUS_REG0_2, 0,
			INFRACFG_AO_INFRA_BUS_REG0_0, 0x603,
			INFRACFG_AO_INFRA_BUS_REG0_1, 0xF,
			INFRACFG_AO_INFRA_BUS_REG0_2, 1);

	/* dcm_infracfg_ao_p2p_rx_clk */
	SET32_BITFIELDS(&mt8186_infracfg_ao->p2p_rx_clk_on,
			INFRACFG_AO_P2P_RX_CLK_REG0_MASK_0, 0,
			INFRACFG_AO_P2P_RX_CLK_REG0_MASK_1, 0,
			INFRACFG_AO_P2P_RX_CLK_REG0_MASK_1, 1);

	/* dcm_infracfg_ao_peri_bus */
	SET32_BITFIELDS(&mt8186_infracfg_ao->peri_bus_dcm_ctrl,
			INFRACFG_AO_PERI_BUS_REG0_0, 0,
			INFRACFG_AO_PERI_BUS_REG0_1, 0,
			INFRACFG_AO_PERI_BUS_REG0_2, 0,
			INFRACFG_AO_PERI_BUS_REG0_0, 3,
			INFRACFG_AO_PERI_BUS_REG0_1, 0xFF07C,
			INFRACFG_AO_PERI_BUS_REG0_2, 1);

	for (i = 0; i < ARRAY_SIZE(mux_sels); i++)
		pll_mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

	/* [4] SCP_CORE_CK_CG, [5] SEJ_CG */
	write32(&mt8186_infracfg_ao->module_sw_cg_0_clr, 0x00000030);
	/* [7] DVFSRC_CG, [20] DEVICE_APC_CG */
	write32(&mt8186_infracfg_ao->module_sw_cg_1_clr, 0x00100080);
	/*
	 * [15] SEJ_F13M_CK_CG, [16] AES_TOP0_BCLK_CK_CG,
	 * [22] FADSP_26M_CG, [23] FADSP_32K_CG, [27] FADSP_CK_CG
	 */
	write32(&mt8186_infracfg_ao->module_sw_cg_3_clr, 0x08C18000);
}

void mt_pll_raise_little_cpu_freq(u32 freq)
{
	/* switch clock source to intermediate clock */
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_26M);

	/* disable armpll_ll frequency output */
	clrbits32(plls[APMIXED_ARMPLL_LL].reg, MT8186_PLL_EN);

	/* raise armpll_ll frequency */
	pll_set_rate(&plls[APMIXED_ARMPLL_LL], freq);

	/* enable armpll_ll frequency output */
	setbits32(plls[APMIXED_ARMPLL_LL].reg, MT8186_PLL_EN);
	udelay(PLL_EN_DELAY);

	/* switch clock source back to armpll_ll */
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
}

void mt_pll_raise_cci_freq(u32 freq)
{
	/* switch clock source to intermediate clock */
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_26M);

	/* disable ccipll frequency output */
	clrbits32(plls[APMIXED_CCIPLL].reg, MT8186_PLL_EN);

	/* raise ccipll frequency */
	pll_set_rate(&plls[APMIXED_CCIPLL], freq);

	/* enable ccipll frequency output */
	setbits32(plls[APMIXED_CCIPLL].reg, MT8186_PLL_EN);
	udelay(PLL_EN_DELAY);

	/* switch clock source back to ccipll */
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
}

void mt_pll_set_usb_clock(void)
{
	/* enable usb macro control */
	SET32_BITFIELDS(&mtk_topckgen->usb_top_cfg, USB_TOP_CFG_MACRO_CTRL, 3);
}

void mt_pll_spmi_mux_select(void)
{
	/* 4: ulposc1_d10 */
	pll_mux_set_sel(&muxes[TOP_SPMI_MST_SEL], 4);
}

u32 mt_fmeter_get_freq_khz(enum fmeter_type type, u32 id)
{
	u32 output, count, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_0, clk26cali_1;

	/* backup */
	clk_dbg_cfg = read32(&mtk_topckgen->clk_dbg_cfg);
	clk_misc_cfg_0 = read32(&mtk_topckgen->clk_misc_cfg_0);
	clk26cali_0 = read32(&mtk_topckgen->clk26cali_0);
	clk26cali_1 = read32(&mtk_topckgen->clk26cali_1);

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
	SET32_BITFIELDS(&mtk_topckgen->clk26cali_0, CLK26CALI_0_ENABLE, 1);

	/* trigger frequency meter */
	SET32_BITFIELDS(&mtk_topckgen->clk26cali_0, CLK26CALI_0_TRIGGER, 1);

	/* wait frequency meter until finished */
	if (wait_us(200, !READ32_BITFIELD(&mtk_topckgen->clk26cali_0, CLK26CALI_0_TRIGGER))) {
		count = read32(&mtk_topckgen->clk26cali_1) & 0xffff;
		output = (count * 26000) / 1024; /* KHz */
	} else {
		output = 0;
	}

	/* restore */
	write32(&mtk_topckgen->clk_dbg_cfg, clk_dbg_cfg);
	write32(&mtk_topckgen->clk_misc_cfg_0, clk_misc_cfg_0);
	write32(&mtk_topckgen->clk26cali_0, clk26cali_0);
	write32(&mtk_topckgen->clk26cali_1, clk26cali_1);

	if (type == FMETER_ABIST)
		return output * 2;
	else if (type == FMETER_CKGEN)
		return output;

	return 0;
}
