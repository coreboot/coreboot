/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <stddef.h>
#include <timer.h>

#include <soc/addressmap.h>
#include <soc/infracfg.h>
#include <soc/mcucfg.h>
#include <soc/pll.h>
#include <soc/spm.h>
#include <soc/wdt.h>

enum mux_id {
	TOP_AXI_SEL,
	TOP_SPM_SEL,
	TOP_SCP_SEL,
	TOP_BUS_AXIMEM_SEL,
	TOP_VPP_SEL,
	TOP_ETHDR_SEL,
	TOP_IPE_SEL,
	TOP_CAM_SEL,
	TOP_CCU_SEL,
	TOP_CCU_AHB_SEL,
	TOP_IMG_SEL,
	TOP_CAMTM_SEL,
	TOP_DSP_SEL,
	TOP_DSP1_SEL,
	TOP_DSP2_SEL,
	TOP_DSP3_SEL,
	TOP_DSP4_SEL,
	TOP_DSP5_SEL,
	TOP_DSP6_SEL,
	TOP_DSP7_SEL,
	TOP_MFG_CORE_TMP_SEL,
	TOP_CAMTG_SEL,
	TOP_CAMTG2_SEL,
	TOP_CAMTG3_SEL,
	TOP_UART_SEL,
	TOP_SPI_SEL,
	TOP_MSDC50_0_HSEL,
	TOP_MSDC50_0_SEL,
	TOP_MSDC30_1_SEL,
	TOP_MSDC30_2_SEL,
	TOP_INTDIR_SEL,
	TOP_AUD_INTBUS_SEL,
	TOP_AUDIO_H_SEL,
	TOP_PWRAP_ULPOSC_SEL,
	TOP_ATB_SEL,
	TOP_SSPM_SEL,
	TOP_DP_SEL,
	TOP_EDP_SEL,
	TOP_DPI_SEL,
	TOP_DISP_PWM0_SEL,
	TOP_DISP_PWM1_SEL,
	TOP_USB_TOP_SEL,
	TOP_SSUSB_XHCI_SEL,
	TOP_USB_TOP_2P_SEL,
	TOP_SSUSB_XHCI_2P_SEL,
	TOP_USB_TOP_3P_SEL,
	TOP_SSUSB_XHCI_3P_SEL,
	TOP_I2C_SEL,
	TOP_SENINF_SEL,
	TOP_SENINF1_SEL,
	TOP_GCPU_SEL,
	TOP_VENC_SEL,
	TOP_VDEC_SEL,
	TOP_PWM_SEL,
	TOP_MCUPM_SEL,
	TOP_SPMI_P_MST_SEL,
	TOP_SPMI_M_MST_SEL,
	TOP_DVFSRC_SEL,
	TOP_TL_SEL,
	TOP_AES_MSDCFDE_SEL,
	TOP_DSI_OCC_SEL,
	TOP_WPE_VPP_SEL,
	TOP_HDCP_SEL,
	TOP_HDCP_24M_SEL,
	TOP_HDMI_APB_SEL,
	TOP_SNPS_ETH_250M_SEL,
	TOP_SNPS_ETH_62P4M_PTP_SEL,
	TOP_SNPS_ETH_50M_RMII_SEL,
	TOP_ADSP_SEL,
	TOP_AUDIO_LOCAL_BUS_SEL,
	TOP_ASM_H_SEL,
	TOP_ASM_L_SEL,
	TOP_APLL1_SEL,
	TOP_APLL2_SEL,
	TOP_APLL3_SEL,
	TOP_APLL4_SEL,
	TOP_APLL5_SEL,
	TOP_I2SO1_SEL,
	TOP_I2SO2_SEL,
	TOP_I2SI1_SEL,
	TOP_I2SI2_SEL,
	TOP_DPTX_SEL,
	TOP_AUD_IEC_SEL,
	TOP_A1SYS_HP_SEL,
	TOP_A2SYS_SEL,
	TOP_A3SYS_SEL,
	TOP_A4SYS_SEL,
	TOP_ECC_SEL,
	TOP_SPINOR_SEL,
	TOP_ULPOSC_SEL,
	TOP_SRCK_SEL,
	TOP_MFG_CK_FAST_REF_SEL,
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
	MUX_UPD(TOP_AXI_SEL, clk_cfg_0, 0, 3, clk_cfg_update, 0),
	MUX_UPD(TOP_SPM_SEL, clk_cfg_0, 8, 2, clk_cfg_update, 1),
	MUX_UPD(TOP_SCP_SEL, clk_cfg_0, 16, 3, clk_cfg_update, 2),
	MUX_UPD(TOP_BUS_AXIMEM_SEL, clk_cfg_0, 24, 3, clk_cfg_update, 3),
	/* CLK_CFG_1 */
	MUX_UPD(TOP_VPP_SEL, clk_cfg_1, 0, 4, clk_cfg_update, 4),
	MUX_UPD(TOP_ETHDR_SEL, clk_cfg_1, 8, 4, clk_cfg_update, 5),
	MUX_UPD(TOP_IPE_SEL, clk_cfg_1, 16, 4, clk_cfg_update, 6),
	MUX_UPD(TOP_CAM_SEL, clk_cfg_1, 24, 4, clk_cfg_update, 7),
	/* CLK_CFG_2 */
	MUX_UPD(TOP_CCU_SEL, clk_cfg_2, 0, 4, clk_cfg_update, 8),
	MUX_UPD(TOP_CCU_AHB_SEL, clk_cfg_2, 8, 4, clk_cfg_update, 9),
	MUX_UPD(TOP_IMG_SEL, clk_cfg_2, 16, 2, clk_cfg_update, 10),
	MUX_UPD(TOP_CAMTM_SEL, clk_cfg_2, 24, 3, clk_cfg_update, 11),
	/* CLK_CFG_3 */
	MUX_UPD(TOP_DSP_SEL, clk_cfg_3, 0, 3, clk_cfg_update, 12),
	MUX_UPD(TOP_DSP1_SEL, clk_cfg_3, 8, 3, clk_cfg_update, 13),
	MUX_UPD(TOP_DSP2_SEL, clk_cfg_3, 16, 3, clk_cfg_update, 14),
	MUX_UPD(TOP_DSP3_SEL, clk_cfg_3, 24, 3, clk_cfg_update, 15),
	/* CLK_CFG_4 */
	MUX_UPD(TOP_DSP4_SEL, clk_cfg_4, 0, 3, clk_cfg_update, 16),
	MUX_UPD(TOP_DSP5_SEL, clk_cfg_4, 8, 3, clk_cfg_update, 17),
	MUX_UPD(TOP_DSP6_SEL, clk_cfg_4, 16, 3, clk_cfg_update, 18),
	MUX_UPD(TOP_DSP7_SEL, clk_cfg_4, 24, 3, clk_cfg_update, 19),
	/* CLK_CFG_5 */
	MUX_UPD(TOP_MFG_CORE_TMP_SEL, clk_cfg_5, 0, 2, clk_cfg_update, 20),
	MUX_UPD(TOP_CAMTG_SEL, clk_cfg_5, 8, 3, clk_cfg_update, 21),
	MUX_UPD(TOP_CAMTG2_SEL, clk_cfg_5, 16, 3, clk_cfg_update, 22),
	MUX_UPD(TOP_CAMTG3_SEL, clk_cfg_5, 24, 3, clk_cfg_update, 23),
	/* CLK_CFG_6 */
	MUX_UPD(TOP_UART_SEL, clk_cfg_6, 0, 3, clk_cfg_update, 24),
	MUX_UPD(TOP_SPI_SEL, clk_cfg_6, 8, 3, clk_cfg_update, 25),
	MUX_UPD(TOP_MSDC50_0_HSEL, clk_cfg_6, 16, 1, clk_cfg_update, 26),
	MUX_UPD(TOP_MSDC50_0_SEL, clk_cfg_6, 24, 3, clk_cfg_update, 27),
	/* CLK_CFG_7 */
	MUX_UPD(TOP_MSDC30_1_SEL, clk_cfg_7, 0, 3, clk_cfg_update, 28),
	MUX_UPD(TOP_MSDC30_2_SEL, clk_cfg_7, 8, 2, clk_cfg_update, 29),
	MUX_UPD(TOP_INTDIR_SEL, clk_cfg_7, 16, 3, clk_cfg_update, 30),
	MUX_UPD(TOP_AUD_INTBUS_SEL, clk_cfg_7, 24, 3, clk_cfg_update, 31),
	/* CLK_CFG_8 */
	MUX_UPD(TOP_AUDIO_H_SEL, clk_cfg_8, 0, 3, clk_cfg_update1, 0),
	MUX_UPD(TOP_PWRAP_ULPOSC_SEL, clk_cfg_8, 8, 2, clk_cfg_update1, 1),
	MUX_UPD(TOP_ATB_SEL, clk_cfg_8, 16, 2, clk_cfg_update1, 2),
	MUX_UPD(TOP_SSPM_SEL, clk_cfg_8, 24, 2, clk_cfg_update1, 3),
	/* CLK_CFG_9 */
	MUX_UPD(TOP_DP_SEL, clk_cfg_9, 0, 3, clk_cfg_update1, 4),
	MUX_UPD(TOP_EDP_SEL, clk_cfg_9, 8, 2, clk_cfg_update1, 5),
	MUX_UPD(TOP_DPI_SEL, clk_cfg_9, 16, 3, clk_cfg_update1, 6),
	MUX_UPD(TOP_DISP_PWM0_SEL, clk_cfg_9, 24, 4, clk_cfg_update1, 7),
	/* CLK_CFG_10 */
	MUX_UPD(TOP_DISP_PWM1_SEL, clk_cfg_10, 0, 4, clk_cfg_update1, 8),
	MUX_UPD(TOP_USB_TOP_SEL, clk_cfg_10, 8, 4, clk_cfg_update1, 9),
	MUX_UPD(TOP_SSUSB_XHCI_SEL, clk_cfg_10, 16, 3, clk_cfg_update1, 10),
	MUX_UPD(TOP_USB_TOP_2P_SEL, clk_cfg_10, 24, 3, clk_cfg_update1, 11),
	/* CLK_CFG_11 */
	MUX_UPD(TOP_SSUSB_XHCI_2P_SEL, clk_cfg_11, 0, 2, clk_cfg_update1, 12),
	MUX_UPD(TOP_USB_TOP_3P_SEL, clk_cfg_11, 8, 2, clk_cfg_update1, 13),
	MUX_UPD(TOP_SSUSB_XHCI_3P_SEL, clk_cfg_11, 16, 2, clk_cfg_update1, 14),
	MUX_UPD(TOP_I2C_SEL, clk_cfg_11, 24, 2, clk_cfg_update1, 15),
	/* CLK_CFG_12 */
	MUX_UPD(TOP_SENINF_SEL, clk_cfg_12, 0, 2, clk_cfg_update1, 16),
	MUX_UPD(TOP_SENINF1_SEL, clk_cfg_12, 8, 2, clk_cfg_update1, 17),
	MUX_UPD(TOP_GCPU_SEL, clk_cfg_12, 16, 2, clk_cfg_update1, 18),
	MUX_UPD(TOP_VENC_SEL, clk_cfg_12, 24, 2, clk_cfg_update1, 19),
	/* CLK_CFG_13 */
	MUX_UPD(TOP_VDEC_SEL, clk_cfg_13, 0, 2, clk_cfg_update1, 20),
	MUX_UPD(TOP_PWM_SEL, clk_cfg_13, 8, 3, clk_cfg_update1, 21),
	MUX_UPD(TOP_MCUPM_SEL, clk_cfg_13, 16, 3, clk_cfg_update1, 22),
	MUX_UPD(TOP_SPMI_P_MST_SEL, clk_cfg_13, 24, 3, clk_cfg_update1, 23),
	/* CLK_CFG_14 */
	MUX_UPD(TOP_SPMI_M_MST_SEL, clk_cfg_14, 0, 3, clk_cfg_update1, 24),
	MUX_UPD(TOP_DVFSRC_SEL, clk_cfg_14, 8, 3, clk_cfg_update1, 25),
	MUX_UPD(TOP_TL_SEL, clk_cfg_14, 16, 2, clk_cfg_update1, 26),
	MUX_UPD(TOP_AES_MSDCFDE_SEL, clk_cfg_14, 24, 3, clk_cfg_update1, 27),
	/* CLK_CFG_15 */
	MUX_UPD(TOP_DSI_OCC_SEL, clk_cfg_15, 0, 3, clk_cfg_update1, 28),
	MUX_UPD(TOP_WPE_VPP_SEL, clk_cfg_15, 8, 3, clk_cfg_update1, 29),
	MUX_UPD(TOP_HDCP_SEL, clk_cfg_15, 16, 1, clk_cfg_update1, 30),
	MUX_UPD(TOP_HDCP_24M_SEL, clk_cfg_15, 24, 1, clk_cfg_update1, 31),
	/* CLK_CFG_16 */
	MUX_UPD(TOP_HDMI_APB_SEL, clk_cfg_16, 0, 4, clk_cfg_update2, 0),
	MUX_UPD(TOP_SNPS_ETH_250M_SEL, clk_cfg_16, 8, 4, clk_cfg_update2, 1),
	MUX_UPD(TOP_SNPS_ETH_62P4M_PTP_SEL, clk_cfg_16, 16, 1, clk_cfg_update2, 2),
	MUX_UPD(TOP_SNPS_ETH_50M_RMII_SEL, clk_cfg_16, 24, 2, clk_cfg_update2, 3),
	/* CLK_CFG_17 */
	MUX_UPD(TOP_ADSP_SEL, clk_cfg_17, 0, 4, clk_cfg_update2, 4),
	MUX_UPD(TOP_AUDIO_LOCAL_BUS_SEL, clk_cfg_17, 8, 4, clk_cfg_update2, 5),
	MUX_UPD(TOP_ASM_H_SEL, clk_cfg_17, 16, 2, clk_cfg_update2, 6),
	MUX_UPD(TOP_ASM_L_SEL, clk_cfg_17, 24, 2, clk_cfg_update2, 7),
	/* CLK_CFG_18 */
	MUX_UPD(TOP_APLL1_SEL, clk_cfg_18, 0, 2, clk_cfg_update2, 8),
	MUX_UPD(TOP_APLL2_SEL, clk_cfg_18, 8, 3, clk_cfg_update2, 9),
	MUX_UPD(TOP_APLL3_SEL, clk_cfg_18, 16, 2, clk_cfg_update2, 10),
	MUX_UPD(TOP_APLL4_SEL, clk_cfg_18, 24, 4, clk_cfg_update2, 11),
	/* CLK_CFG_19 */
	MUX_UPD(TOP_APLL5_SEL, clk_cfg_19, 0, 2, clk_cfg_update2, 12),
	MUX_UPD(TOP_I2SO1_SEL, clk_cfg_19, 8, 2, clk_cfg_update2, 13),
	MUX_UPD(TOP_I2SO2_SEL, clk_cfg_19, 16, 2, clk_cfg_update2, 14),
	MUX_UPD(TOP_I2SI1_SEL, clk_cfg_19, 24, 2, clk_cfg_update2, 15),
	/* CLK_CFG_20 */
	MUX_UPD(TOP_I2SI2_SEL, clk_cfg_20, 0, 1, clk_cfg_update2, 16),
	MUX_UPD(TOP_DPTX_SEL, clk_cfg_20, 8, 2, clk_cfg_update2, 17),
	MUX_UPD(TOP_AUD_IEC_SEL, clk_cfg_20, 16, 1, clk_cfg_update2, 18),
	MUX_UPD(TOP_A1SYS_HP_SEL, clk_cfg_20, 24, 2, clk_cfg_update2, 19),
	/* CLK_CFG_21 */
	MUX_UPD(TOP_A2SYS_SEL, clk_cfg_21, 0, 1, clk_cfg_update2, 20),
	MUX_UPD(TOP_A3SYS_SEL, clk_cfg_21, 8, 3, clk_cfg_update2, 21),
	MUX_UPD(TOP_A4SYS_SEL, clk_cfg_21, 16, 4, clk_cfg_update2, 22),
	MUX_UPD(TOP_ECC_SEL, clk_cfg_21, 24, 4, clk_cfg_update2, 23),
	/* CLK_CFG_22 */
	MUX_UPD(TOP_SPINOR_SEL, clk_cfg_22, 0, 4, clk_cfg_update2, 24),
	MUX_UPD(TOP_ULPOSC_SEL, clk_cfg_22, 8, 2, clk_cfg_update2, 25),
	MUX_UPD(TOP_SRCK_SEL, clk_cfg_22, 16, 2, clk_cfg_update2, 26),
};

struct mux_sel {
	enum mux_id id;
	u32 sel;
};

static const struct mux_sel mux_sels[] = {
	/* CLK_CFG_0 */
	{ .id = TOP_AXI_SEL, .sel = 2 },			/* 2: mainpll_d7_d2 */
	{ .id = TOP_SPM_SEL, .sel = 2 },			/* 2: mainpll_d7_d4 */
	{ .id = TOP_SCP_SEL, .sel = 5 },			/* 5: mainpll_d4_d2 */
	{ .id = TOP_BUS_AXIMEM_SEL, .sel = 1 },			/* 1: mainpll_d7_d2 */
	/* CLK_CFG_1 */
	{ .id = TOP_VPP_SEL, .sel = 8 },			/* 8: mainpll_d6 */
	{ .id = TOP_ETHDR_SEL, .sel = 10 },			/* 10: mmpll_d5_d4 */
	{ .id = TOP_IPE_SEL, .sel = 8 },			/* 8: mainpll_d4_d2 */
	{ .id = TOP_CAM_SEL, .sel = 8 },			/* 8: mainpll_d4_d2 */
	/* CLK_CFG_2 */
	{ .id = TOP_CCU_SEL, .sel = 2 },			/* 2: mainpll_d4_d2 */
	{ .id = TOP_CCU_AHB_SEL, .sel = 11 },			/* 11: univpll_d5_d2 */
	{ .id = TOP_IMG_SEL, .sel = 2 },			/* 2: univpll_d6_d2 */
	{ .id = TOP_CAMTM_SEL, .sel = 7 },			/* 7: univpll_d3 */
	/* CLK_CFG_3 */
	{ .id = TOP_DSP_SEL, .sel = 7 },			/* 7: univpll_d3 */
	{ .id = TOP_DSP1_SEL, .sel = 7 },			/* 7: univpll_d3 */
	{ .id = TOP_DSP2_SEL, .sel = 7 },			/* 7: univpll_d3 */
	{ .id = TOP_DSP3_SEL, .sel = 7 },			/* 7: univpll_d3 */
	/* CLK_CFG_4 */
	{ .id = TOP_DSP4_SEL, .sel = 7 },			/* 7: univpll_d3 */
	{ .id = TOP_DSP5_SEL, .sel = 7 },			/* 7: univpll_d3 */
	{ .id = TOP_DSP6_SEL, .sel = 7 },			/* 7: univpll_d3 */
	{ .id = TOP_DSP7_SEL, .sel = 7 },			/* 7: mmpll_d4 */
	/* CLK_CFG_5 */
	{ .id = TOP_MFG_CORE_TMP_SEL, .sel = 3 },		/* 3: univpll_d7 */
	{ .id = TOP_CAMTG_SEL, .sel = 2 },			/* 2: univpll_d6_d8 */
	{ .id = TOP_CAMTG2_SEL, .sel = 2 },			/* 2: univpll_d6_d8 */
	{ .id = TOP_CAMTG3_SEL, .sel = 2 },			/* 2: univpll_d6_d8 */
	/* CLK_CFG_6 */
	{ .id = TOP_UART_SEL, .sel = 0 },			/* 0: clk26m */
	{ .id = TOP_SPI_SEL, .sel = 4 },			/* 4: univpll_d6_d2 */
	{ .id = TOP_MSDC50_0_HSEL, .sel = 1 },			/* 1: mainpll_d4_d2 */
	{ .id = TOP_MSDC50_0_SEL, .sel = 1 },			/* 1: msdcpll_ck */
	/* CLK_CFG_7 */
	{ .id = TOP_MSDC30_1_SEL, .sel = 1 },			/* 1: univpll_d6_d2 */
	{ .id = TOP_MSDC30_2_SEL, .sel = 1 },			/* 1: univpll_d6_d2 */
	{ .id = TOP_INTDIR_SEL, .sel = 3 },			/* 3: univpll_d4 */
	{ .id = TOP_AUD_INTBUS_SEL, .sel = 1 },			/* 1: mainpll_d4_d4 */
	/* CLK_CFG_8 */
	{ .id = TOP_AUDIO_H_SEL, .sel = 2 },			/* 2: apll1_ck */
	{ .id = TOP_PWRAP_ULPOSC_SEL, .sel = 1 },			/* 1: ulposc_d10 */
	{ .id = TOP_ATB_SEL, .sel = 1 },			/* 1: mainpll_d4_d2 */
	{ .id = TOP_SSPM_SEL, .sel = 3 },			/* 3: mainpll_d5_d2 */
	/* CLK_CFG_9 */
	{ .id = TOP_DP_SEL, .sel = 3 },				/* 3: tvdpll1_d4  */
	{ .id = TOP_EDP_SEL, .sel = 4 },			/* 4: tvdpll2_d4 */
	{ .id = TOP_DPI_SEL, .sel = 2 },			/* 2: tvdpll1_d2 */
	{ .id = TOP_DISP_PWM0_SEL, .sel = 2 },			/* 2: ulposc_d2 */
	/* CLK_CFG_10 */
	{ .id = TOP_DISP_PWM1_SEL, .sel = 2 },			/* 2: ulposc_d2 */
	{ .id = TOP_USB_TOP_SEL, .sel = 1 },			/* 1: univpll_d5_d4 */
	{ .id = TOP_SSUSB_XHCI_SEL, .sel = 1 },			/* 1: univpll_d6_d4 */
	{ .id = TOP_USB_TOP_2P_SEL, .sel = 1 },			/* 1: univpll_d6_d4 */
	/* CLK_CFG_11 */
	{ .id = TOP_SSUSB_XHCI_2P_SEL, .sel = 1 },		/* 1: univpll_d5_d4 */
	{ .id = TOP_USB_TOP_3P_SEL, .sel = 1 },			/* 1: univpll_d5_d4 */
	{ .id = TOP_SSUSB_XHCI_3P_SEL, .sel = 1 },		/* 1: univpll_d5_d4 */
	{ .id = TOP_I2C_SEL, .sel = 2 },			/* 2: univpll_d5_d4 */
	/* CLK_CFG_12 */
	{ .id = TOP_SENINF_SEL, .sel = 3 },			/* 3: mainpll_d4_d2 */
	{ .id = TOP_SENINF1_SEL, .sel = 3 },			/* 3: mainpll_d4_d2 */
	{ .id = TOP_GCPU_SEL, .sel = 3 },			/* 3: mmpll_d5_d2 */
	{ .id = TOP_VENC_SEL, .sel = 14 },			/* 14: univpll_d5_d2 */
	/* CLK_CFG_13 */
	{ .id = TOP_VDEC_SEL, .sel = 1 },			/* 1: mainpll_d5_d2 */
	{ .id = TOP_PWM_SEL, .sel = 3 },			/* 3: univpll_d6_d4 */
	{ .id = TOP_MCUPM_SEL, .sel = 1 },			/* 1: mainpll_d6_d2 */
	{ .id = TOP_SPMI_P_MST_SEL, .sel = 3 },			/* 7: mainpll_d7_d8 */
	/* CLK_CFG_14 */
	{ .id = TOP_SPMI_M_MST_SEL, .sel = 3 },			/* 7: mainpll_d7_d8 */
	{ .id = TOP_DVFSRC_SEL, .sel = 0 },			/* 0: clk26m */
	{ .id = TOP_TL_SEL, .sel = 2 },				/* 2: mainpll_d4_d4 */
	{ .id = TOP_AES_MSDCFDE_SEL, .sel = 5 },		/* 5: univpll_d6 */
	/* CLK_CFG_15 */
	{ .id = TOP_DSI_OCC_SEL, .sel = 1 },			/* 1: univpll_d6_d2 */
	{ .id = TOP_WPE_VPP_SEL, .sel = 7 },			/* 7: mainpll_d6 */
	{ .id = TOP_HDCP_SEL, .sel = 3 },			/* 3: univpll_d6_d4 */
	{ .id = TOP_HDCP_24M_SEL, .sel = 1 },			/* 1: univpll_192m_d4 */
	/* CLK_CFG_16 */
	{ .id = TOP_HDMI_APB_SEL, .sel = 2 },			/* 2: msdcpll_d2 */
	{ .id = TOP_SNPS_ETH_250M_SEL, .sel = 1 },		/* 1: ethpll_d2 */
	{ .id = TOP_SNPS_ETH_62P4M_PTP_SEL, .sel = 1 },		/* 1: ethpll_d8 */
	{ .id = TOP_SNPS_ETH_50M_RMII_SEL, .sel = 1 },		/* 1: ethpll_d10 */
	/* CLK_CFG_17 */
	{ .id = TOP_ADSP_SEL, .sel = 8 },			/* 8: adsppll_ck */
	{ .id = TOP_AUDIO_LOCAL_BUS_SEL, .sel = 4 },		/* 4: mainpll_d5_d2 */
	{ .id = TOP_ASM_H_SEL, .sel = 3 },			/* 3: mainpll_d5_d2 */
	{ .id = TOP_ASM_L_SEL, .sel = 3 },			/* 3: mainpll_d5_d2 */
	/* CLK_CFG_18 */
	{ .id = TOP_APLL1_SEL, .sel = 1 },			/* 1: apll1_d4 */
	{ .id = TOP_APLL2_SEL, .sel = 1 },			/* 1: apll2_d4 */
	{ .id = TOP_APLL3_SEL, .sel = 1 },			/* 1: apll3_d4 */
	{ .id = TOP_APLL4_SEL, .sel = 1 },			/* 1: apll4_d4 */
	/* CLK_CFG_19 */
	{ .id = TOP_APLL5_SEL, .sel = 1 },			/* 1: apll5_d4 */
	{ .id = TOP_I2SO1_SEL, .sel = 1 },			/* 1: apll1_ck */
	{ .id = TOP_I2SO2_SEL, .sel = 1 },			/* 1: apll1_ck */
	{ .id = TOP_I2SI1_SEL, .sel = 1 },			/* 1: apll1_ck */
	/* CLK_CFG_20 */
	{ .id = TOP_I2SI2_SEL, .sel = 1 },			/* 1: apll1_ck */
	{ .id = TOP_DPTX_SEL, .sel = 1 },			/* 1: apll1_ck */
	{ .id = TOP_AUD_IEC_SEL, .sel = 1 },			/* 1: apll1_ck */
	{ .id = TOP_A1SYS_HP_SEL, .sel = 1 },			/* 1: apll1_d4 */
	/* CLK_CFG_21 */
	{ .id = TOP_A2SYS_SEL, .sel = 1 },			/* 1: apll2_d4 */
	{ .id = TOP_A3SYS_SEL, .sel = 1 },			/* 1: apll3_d4 */
	{ .id = TOP_A4SYS_SEL, .sel = 2 },			/* 2: apll4_d4 */
	{ .id = TOP_ECC_SEL, .sel = 1 },			/* 1: mainpll_d4_d4 */
	/* CLK_CFG_22 */
	{ .id = TOP_SPINOR_SEL, .sel = 3 },			/* 3: univpll_d6_d8 */
	{ .id = TOP_ULPOSC_SEL, .sel = 0 },			/* 3: ulposc_ck */
	{ .id = TOP_SRCK_SEL, .sel = 0 },			/* 0: ulposc_d10 */
};

enum pll_id {
	APMIXED_ARMPLL_LL,
	APMIXED_ARMPLL_BL,
	APMIXED_CCIPLL,
	APMIXED_ETHPLL,
	APMIXED_MSDCPLL,
	APMIXED_TVDPLL1,
	APMIXED_TVDPLL2,
	APMIXED_MMPLL,
	APMIXED_MAINPLL,
	APMIXED_IMGPLL,
	APMIXED_UNIVPLL,
	APMIXED_ADSPPLL,
	APMIXED_APLL1,
	APMIXED_APLL2,
	APMIXED_APLL3,
	APMIXED_APLL4,
	APMIXED_APLL5,
	APMIXED_MFGPLL,
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
	PLL(APMIXED_ETHPLL, ethpll_con0, ethpll_con3,
	    NO_RSTB_SHIFT, 22, ethpll_con1, 24, ethpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_MSDCPLL, msdcpll_con0, msdcpll_con3,
	    NO_RSTB_SHIFT, 22, msdcpll_con1, 24, msdcpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_TVDPLL1, tvdpll1_con0, tvdpll1_con3,
	    NO_RSTB_SHIFT, 22, tvdpll1_con1, 24, tvdpll1_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_TVDPLL2, tvdpll2_con0, tvdpll2_con3,
	    NO_RSTB_SHIFT, 22, tvdpll2_con1, 24, tvdpll2_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_MMPLL, mmpll_con0, mmpll_con3,
	    23, 22, mmpll_con1, 24, mmpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_MAINPLL, mainpll_con0, mainpll_con3,
	    23, 22, mainpll_con1, 24, mainpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_IMGPLL, imgpll_con0, imgpll_con3,
	    NO_RSTB_SHIFT, 22, imgpll_con1, 24, imgpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_UNIVPLL, univpll_con0, univpll_con3,
	    23, 22, univpll_con1, 24, univpll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_ADSPPLL, adsppll_con0, adsppll_con3,
	    NO_RSTB_SHIFT, 22, adsppll_con1, 24, adsppll_con1, 0,
	    pll_div_rate),
	PLL(APMIXED_APLL1, apll1_con0, apll1_con4,
	    NO_RSTB_SHIFT, 32, apll1_con2, 24, apll1_con2, 0,
	    pll_div_rate),
	PLL(APMIXED_APLL2, apll2_con0, apll2_con4,
	    NO_RSTB_SHIFT, 32, apll2_con2, 24, apll2_con2, 0,
	    pll_div_rate),
	PLL(APMIXED_APLL3, apll3_con0, apll3_con4,
	    NO_RSTB_SHIFT, 32, apll3_con2, 24, apll3_con2, 0,
	    pll_div_rate),
	PLL(APMIXED_APLL4, apll4_con0, apll4_con4,
	    NO_RSTB_SHIFT, 32, apll4_con2, 24, apll4_con2, 0,
	    pll_div_rate),
	PLL(APMIXED_APLL5, apll5_con0, apll5_con4,
	    NO_RSTB_SHIFT, 32, apll5_con2, 24, apll5_con2, 0,
	    pll_div_rate),
	PLL(APMIXED_MFGPLL, mfgpll_con0, mfgpll_con3,
	    NO_RSTB_SHIFT, 22, mfgpll_con1, 24, mfgpll_con1, 0,
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
	{ .id = APMIXED_ETHPLL, .rate = ETHPLL_HZ },
	{ .id = APMIXED_MSDCPLL, .rate = MSDCPLL_HZ },
	{ .id = APMIXED_TVDPLL1, .rate = TVDPLL1_HZ },
	{ .id = APMIXED_TVDPLL2, .rate = TVDPLL2_HZ },
	{ .id = APMIXED_MMPLL, .rate = MMPLL_HZ },
	{ .id = APMIXED_MAINPLL, .rate = MAINPLL_HZ },
	{ .id = APMIXED_IMGPLL, .rate = IMGPLL_HZ },
	{ .id = APMIXED_UNIVPLL, .rate = UNIVPLL_HZ },
	{ .id = APMIXED_ADSPPLL, .rate = ADSPPLL_HZ },
	{ .id = APMIXED_APLL1, .rate = APLL1_HZ },
	{ .id = APMIXED_APLL2, .rate = APLL2_HZ },
	{ .id = APMIXED_APLL3, .rate = APLL3_HZ },
	{ .id = APMIXED_APLL4, .rate = APLL4_HZ },
	{ .id = APMIXED_APLL5, .rate = APLL5_HZ },
	{ .id = APMIXED_MFGPLL, .rate = MFGPLL_HZ },
};

void pll_set_pcw_change(const struct pll *pll)
{
	setbits32(pll->div_reg, PLL_PCW_CHG);
}

void mt_pll_init(void)
{
	int i;

	/* enable clock square */
	setbits32(&mtk_apmixed->ap_pll_con0, BIT(2));

	udelay(PLL_CKSQ_ON_DELAY);

	/* enable clock square1 low-pass filter */
	setbits32(&mtk_apmixed->ap_pll_con0, BIT(1));

	/*
	 * BIT(3): 1 for register control; 0 for sleep control
	 * BIT(8): 1 to enable clock square2; 0 to disable it
	 */
	clrbits32(&mtk_apmixed->ap_pll_con0, BIT(3) | BIT(8));

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

	/* disable mfg_ck_en[20], enable mfg_opp_ck_en[2]  */
	clrsetbits32(&mtk_apmixed->mfgpll_con0, BIT(20), BIT(2));

	/* xPLL Frequency Set */
	for (i = 0; i < ARRAY_SIZE(rates); i++)
		pll_set_rate(&plls[rates[i].id], rates[i].rate);

	/* AUDPLL Tuner Frequency Set */
	write32(&mtk_apmixed->apll1_tuner_con0, read32(&mtk_apmixed->apll1_con3) + 1);
	write32(&mtk_apmixed->apll2_tuner_con0, read32(&mtk_apmixed->apll2_con3) + 1);
	write32(&mtk_apmixed->apll3_tuner_con0, read32(&mtk_apmixed->apll3_con3) + 1);
	write32(&mtk_apmixed->apll4_tuner_con0, read32(&mtk_apmixed->apll4_con3) + 1);
	write32(&mtk_apmixed->apll5_tuner_con0, read32(&mtk_apmixed->apll5_con3) + 1);

	/* xPLL Frequency Enable */
	for (i = 0; i < APMIXED_PLL_MAX; i++) {
		if (i == APMIXED_APLL5)
			setbits32(plls[i].pwr_reg, MT8188_APLL5_EN);
		else
			setbits32(plls[i].reg, MT8188_PLL_EN);
	}

	/* enable univpll analog divider=13 */
	setbits32(&mtk_apmixed->univpll_con0, 0x8d);

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

	/* initialize SPM request */
	setbits32(&mtk_topckgen->clk_scp_cfg_0, 0x3ff);

	/*
	 * TOP CLKMUX -- DO NOT CHANGE WITHOUT ADJUSTING <soc/pll.h> CONSTANTS!
	 */
	for (i = 0; i < ARRAY_SIZE(mux_sels); i++)
		mux_set_sel(&muxes[mux_sels[i].id], mux_sels[i].sel);

	/* turn off unused clock in infra_ao */
	write32(&mt8188_infracfg_ao->module_sw_cg_1_set, 0x00004000);
	write32(&mt8188_infracfg_ao->module_sw_cg_2_set, 0xf0003802);
	write32(&mt8188_infracfg_ao->module_sw_cg_3_set, 0x30000000);
	write32(&mt8188_infracfg_ao->module_sw_cg_4_set, 0x81c20000);
}

void mt_pll_raise_little_cpu_freq(u32 freq)
{
	/* switch clock source to intermediate clock */
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_26M);

	/* disable armpll_ll frequency output */
	clrbits32(plls[APMIXED_ARMPLL_LL].reg, MT8188_PLL_EN);

	/* raise armpll_ll frequency */
	pll_set_rate(&plls[APMIXED_ARMPLL_LL], freq);

	/* enable armpll_ll frequency output */
	setbits32(plls[APMIXED_ARMPLL_LL].reg, MT8188_PLL_EN);
	udelay(PLL_EN_DELAY);

	/* switch clock source back to armpll_ll */
	clrsetbits32(&mtk_mcucfg->cpu_plldiv_cfg0, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
}

void mt_pll_raise_cci_freq(u32 freq)
{
	/* switch clock source to intermediate clock */
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_26M);

	/* disable ccipll frequency output */
	clrbits32(plls[APMIXED_CCIPLL].reg, MT8188_PLL_EN);

	/* raise ccipll frequency */
	pll_set_rate(&plls[APMIXED_CCIPLL], freq);

	/* enable ccipll frequency output */
	setbits32(plls[APMIXED_CCIPLL].reg, MT8188_PLL_EN);
	udelay(PLL_EN_DELAY);

	/* switch clock source back to ccipll */
	clrsetbits32(&mtk_mcucfg->bus_plldiv_cfg, MCU_MUX_MASK, MCU_MUX_SRC_PLL);
}

void mt_pll_set_tvd_pll1_freq(u32 freq)
{
	/* disable tvdpll frequency output */
	clrbits32(plls[APMIXED_TVDPLL1].reg, MT8188_PLL_EN);

	/* set tvdpll frequency */
	pll_set_rate(&plls[APMIXED_TVDPLL1], freq);

	/* enable tvdpll frequency output */
	setbits32(plls[APMIXED_TVDPLL1].reg, MT8188_PLL_EN);
	udelay(PLL_EN_DELAY);
}

void edp_mux_set_sel(u32 sel)
{
	mux_set_sel(&muxes[TOP_EDP_SEL], sel);
}

void mt_pll_set_usb_clock(void)
{
	/* port0 */
	setbits32(&mt8188_pericfg_ao->peri_module_sw_cg_0_clr, BIT(9) | BIT(10));
	setbits32(&mtk_topckgen->clk_misc_cfg_3, BIT(3) | BIT(5));

	/* port1 sys clk */
	setbits32(&mtk_topckgen->clk_cfg_10_clr, BIT(15));
	setbits32(&mtk_topckgen->clk_cfg_10_set, BIT(8));
	setbits32(&mtk_topckgen->clk_cfg_update1, BIT(9));
	setbits32(&mt8188_pericfg_ao->peri_module_sw_cg_0_clr, BIT(13));

	/* port1 xhci clk */
	setbits32(&mtk_topckgen->clk_cfg_10_clr, BIT(23));
	setbits32(&mtk_topckgen->clk_cfg_10_set, BIT(16));
	setbits32(&mtk_topckgen->clk_cfg_update1, BIT(10));
	setbits32(&mt8188_pericfg_ao->peri_module_sw_cg_0_clr, BIT(14));

	setbits32(&mtk_topckgen->clk_misc_cfg_3, BIT(2) | BIT(4));
}

u32 mt_fmeter_get_freq_khz(enum fmeter_type type, u32 id)
{
	u32 output, count, clk_dbg_cfg, clk_misc_cfg_0;
	u32 multiplier = 1;

	/* backup */
	clk_dbg_cfg = read32(&mtk_topckgen->clk_dbg_cfg);
	clk_misc_cfg_0 = read32(&mtk_topckgen->clk_misc_cfg_0);

	/* set up frequency meter */
	if (type == FMETER_ABIST) {
		SET32_BITFIELDS(&mtk_topckgen->clk_dbg_cfg,
				CLK_DBG_CFG_ABIST_CK_SEL, id,
				CLK_DBG_CFG_CKGEN_CK_SEL, 0,
				CLK_DBG_CFG_METER_CK_SEL, 0,
				CLK_DBG_CFG_CKGEN_EN, 0);
		SET32_BITFIELDS(&mtk_topckgen->clk_misc_cfg_0,
				CLK_MISC_CFG_0_METER_DIV, 3);
		multiplier = 4;
	} else if (type == FMETER_CKGEN) {
		SET32_BITFIELDS(&mtk_topckgen->clk_dbg_cfg,
				CLK_DBG_CFG_ABIST_CK_SEL, 0,
				CLK_DBG_CFG_CKGEN_CK_SEL, id,
				CLK_DBG_CFG_METER_CK_SEL, 1,
				CLK_DBG_CFG_CKGEN_EN, 1);
		SET32_BITFIELDS(&mtk_topckgen->clk_misc_cfg_0,
				CLK_MISC_CFG_0_METER_DIV, 0);
	} else {
		die("unsupported fmeter type\n");
	}

	/* enable frequency meter */
	write32(&mtk_topckgen->clk26cali_0, 0x80);

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

	return output * multiplier;
}
