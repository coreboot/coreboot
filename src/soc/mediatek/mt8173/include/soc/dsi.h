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

#ifndef _DSI_REG_H_
#define _DSI_REG_H_

#include <edid.h>
#include <types.h>

enum mipi_dsi_pixel_format {
	MIPI_DSI_FMT_RGB888,
	MIPI_DSI_FMT_RGB666,
	MIPI_DSI_FMT_RGB666_PACKED,
	MIPI_DSI_FMT_RGB565
};

/* video mode */
enum {
	MIPI_DSI_MODE_VIDEO = BIT(0),
	/* video burst mode */
	MIPI_DSI_MODE_VIDEO_BURST = BIT(1),
	/* video pulse mode */
	MIPI_DSI_MODE_VIDEO_SYNC_PULSE = BIT(2),
	/* enable auto vertical count mode */
	MIPI_DSI_MODE_VIDEO_AUTO_VERT = BIT(3),
	/* enable hsync-end packets in vsync-pulse and v-porch area */
	MIPI_DSI_MODE_VIDEO_HSE = BIT(4),
	/* disable hfront-porch area */
	MIPI_DSI_MODE_VIDEO_HFP = BIT(5),
	/* disable hback-porch area */
	MIPI_DSI_MODE_VIDEO_HBP = BIT(6),
	/* disable hsync-active area */
	MIPI_DSI_MODE_VIDEO_HSA = BIT(7),
	/* flush display FIFO on vsync pulse */
	MIPI_DSI_MODE_VSYNC_FLUSH = BIT(8),
	/* disable EoT packets in HS mode */
	MIPI_DSI_MODE_EOT_PACKET = BIT(9),
	/* device supports non-continuous clock behavior (DSI spec 5.6.1) */
	MIPI_DSI_CLOCK_NON_CONTINUOUS = BIT(10),
	/* transmit data in low power */
	MIPI_DSI_MODE_LPM = BIT(11)
};

struct dsi_regs {
	u32 dsi_start;
	u8 reserved0[4];
	u32 dsi_inten;
	u32 dsi_intsta;
	u32 dsi_con_ctrl;
	u32 dsi_mode_ctrl;
	u32 dsi_txrx_ctrl;
	u32 dsi_psctrl;
	u32 dsi_vsa_nl;
	u32 dsi_vbp_nl;
	u32 dsi_vfp_nl;
	u32 dsi_vact_nl;
	u8 reserved1[32];
	u32 dsi_hsa_wc;
	u32 dsi_hbp_wc;
	u32 dsi_hfp_wc;
	u32 dsi_bllp_wc;
	u8 reserved2[4];
	u32 dsi_hstx_cklp_wc;
	u8 reserved3[156];
	u32 dsi_phy_lccon;
	u32 dsi_phy_ld0con;
	u8 reserved4[4];
	u32 dsi_phy_timecon0;
	u32 dsi_phy_timecon1;
	u32 dsi_phy_timecon2;
	u32 dsi_phy_timecon3;
};

check_member(dsi_regs, dsi_phy_lccon, 0x104);
check_member(dsi_regs, dsi_phy_timecon3, 0x11c);
static struct dsi_regs * const dsi0 = (void *)DSI0_BASE;

/* DSI_MODE_CTRL */
enum {
	MODE = 3,
	CMD_MODE = 0,
	SYNC_PULSE_MODE = 1,
	SYNC_EVENT_MODE = 2,
	BURST_MODE = 3,
	FRM_MODE = BIT(16),
	MIX_MODE = BIT(17)
};

/* DSI_PSCTRL */
enum {
	DSI_PS_WC = 0x3fff,
	DSI_PS_SEL = (3 << 16),
	PACKED_PS_16BIT_RGB565 = (0 << 16),
	LOOSELY_PS_18BIT_RGB666 = (1 << 16),
	PACKED_PS_18BIT_RGB666 = (2 << 16),
	PACKED_PS_24BIT_RGB888 = (3 << 16)
};

/* DSI_PHY_LCCON */
enum {
	LC_HS_TX_EN = BIT(0),
	LC_ULPM_EN = BIT(1),
	LC_WAKEUP_EN = BIT(2)
};

/*DSI_PHY_LD0CON */
enum {
	LD0_RM_TRIG_EN = BIT(0),
	LD0_ULPM_EN = BIT(1),
	LD0_WAKEUP_EN = BIT(2)
};

enum {
	LPX = (0xff << 0),
	HS_PRPR = (0xff << 8),
	HS_ZERO = (0xff << 16),
	HS_TRAIL = (0xff << 24)
};

enum {
	TA_GO = (0xff << 0),
	TA_SURE = (0xff << 8),
	TA_GET = (0xff << 16),
	DA_HS_EXIT = (0xff << 24)
};

enum {
	CONT_DET = (0xff << 0),
	CLK_ZERO = (0xf << 16),
	CLK_TRAIL = (0xff << 24)
};

enum {
	CLK_HS_PRPR = (0xff << 0),
	CLK_HS_POST = (0xff << 8),
	CLK_HS_EXIT = (0xf << 16)
};

/* MIPITX_REG */
struct mipi_tx_regs {
	u32 dsi_con;
	u32 dsi_clock_lane;
	u32 dsi_data_lane[4];
	u8 reserved0[40];
	u32 dsi_top_con;
	u32 dsi_bg_con;
	u8 reserved1[8];
	u32 dsi_pll_con0;
	u32 dsi_pll_con1;
	u32 dsi_pll_con2;
	u32 dsi_pll_con3;
	u32 dsi_pll_chg;
	u32 dsi_pll_top;
	u32 dsi_pll_pwr;
	u8 reserved2[4];
	u32 dsi_rgs;
	u32 dsi_gpi_en;
	u32 dsi_gpi_pull;
	u32 dsi_phy_sel;
	u32 dsi_sw_ctrl_en;
	u32 dsi_sw_ctrl_con0;
	u32 dsi_sw_ctrl_con1;
	u32 dsi_sw_ctrl_con2;
	u32 dsi_dbg_con;
	u32 dsi_dbg_out;
	u32 dsi_apb_async_sta;
};

check_member(mipi_tx_regs, dsi_top_con, 0x40);
check_member(mipi_tx_regs, dsi_pll_pwr, 0x68);

static struct mipi_tx_regs * const mipi_tx0 = (void *)MIPI_TX0_BASE;

/* MIPITX_DSI0_CON */
enum {
	RG_DSI0_LDOCORE_EN = BIT(0),
	RG_DSI0_CKG_LDOOUT_EN = BIT(1),
	RG_DSI0_BCLK_SEL = (3 << 2),
	RG_DSI0_LD_IDX_SEL = (7 << 4),
	RG_DSI0_PHYCLK_SEL = (2 << 8),
	RG_DSI0_DSICLK_FREQ_SEL = BIT(10),
	RG_DSI0_LPTX_CLMP_EN = BIT(11)
};

/* MIPITX_DSI0_CLOCK_LANE */
enum {
	LDOOUT_EN = BIT(0),
	CKLANE_EN = BIT(1),
	IPLUS1 = BIT(2),
	LPTX_IPLUS2 = BIT(3),
	LPTX_IMINUS = BIT(4),
	LPCD_IPLUS = BIT(5),
	LPCD_IMLUS = BIT(6),
	RT_CODE = (0xf << 8)
};

/* MIPITX_DSI_TOP_CON */
enum {
	RG_DSI_LNT_INTR_EN = BIT(0),
	RG_DSI_LNT_HS_BIAS_EN = BIT(1),
	RG_DSI_LNT_IMP_CAL_EN = BIT(2),
	RG_DSI_LNT_TESTMODE_EN = BIT(3),
	RG_DSI_LNT_IMP_CAL_CODE = (0xf << 4),
	RG_DSI_LNT_AIO_SEL = (7 << 8),
	RG_DSI_PAD_TIE_LOW_EN = BIT(11),
	RG_DSI_DEBUG_INPUT_EN = BIT(12),
	RG_DSI_PRESERVE = (7 << 13)
};

/* MIPITX_DSI_BG_CON */
enum {
	RG_DSI_BG_CORE_EN = BIT(0),
	RG_DSI_BG_CKEN = BIT(1),
	RG_DSI_BG_DIV = (0x3 << 2),
	RG_DSI_BG_FAST_CHARGE = BIT(4),
	RG_DSI_V12_SEL = (7 << 5),
	RG_DSI_V10_SEL = (7 << 8),
	RG_DSI_V072_SEL = (7 << 11),
	RG_DSI_V04_SEL = (7 << 14),
	RG_DSI_V032_SEL = (7 << 17),
	RG_DSI_V02_SEL = (7 << 20),
	rsv_23 = BIT(23),
	RG_DSI_BG_R1_TRIM = (0xf << 24),
	RG_DSI_BG_R2_TRIM = (0xf << 28)
};

/* MIPITX_DSI_PLL_CON0 */
enum {
	RG_DSI0_MPPLL_PLL_EN = BIT(0),
	RG_DSI0_MPPLL_PREDIV = (3 << 1),
	RG_DSI0_MPPLL_TXDIV0 = (3 << 3),
	RG_DSI0_MPPLL_TXDIV1 = (3 << 5),
	RG_DSI0_MPPLL_POSDIV = (7 << 7),
	RG_DSI0_MPPLL_MONVC_EN = BIT(10),
	RG_DSI0_MPPLL_MONREF_EN = BIT(11),
	RG_DSI0_MPPLL_VOD_EN = BIT(12)
};

/* MIPITX_DSI_PLL_CON1 */
enum {
	RG_DSI0_MPPLL_SDM_FRA_EN = BIT(0),
	RG_DSI0_MPPLL_SDM_SSC_PH_INIT = BIT(1),
	RG_DSI0_MPPLL_SDM_SSC_EN = BIT(2),
	RG_DSI0_MPPLL_SDM_SSC_PRD = (0xffff << 16)
};

/* MIPITX_DSI_PLL_PWR */
enum {
	RG_DSI_MPPLL_SDM_PWR_ON = BIT(0),
	RG_DSI_MPPLL_SDM_ISO_EN = BIT(1),
	RG_DSI_MPPLL_SDM_PWR_ACK = BIT(8)
};

/* LVDS_TX1_REG */
struct lvds_tx1_regs {
	u32 lvdstx1_ctl1;
	u32 lvdstx1_ctl2;
	u32 lvdstx1_ctl3;
	u32 lvdstx1_ctl4;
	u32 lvdstx1_ctl5;
	u32 vopll_ctl1;
	u32 vopll_ctl2;
	u32 vopll_ctl3;
};

static struct lvds_tx1_regs * const lvds_tx1 = (void *)(MIPI_TX0_BASE + 0x800);

/* LVDS_VOPLL_CTRL3 */
enum {
	RG_LVDSTX_21EDG = BIT(0),
	RG_LVDSTX_21LEV = BIT(1),
	RG_LVDSTX_51EDG = BIT(2),
	RG_LVDSTX_51LEV = BIT(3),
	RG_AD_LVDSTX_PWR_ACK = BIT(4),
	RG_DA_LVDS_ISO_EN = BIT(8),
	RG_DA_LVDSTX_PWR_ON = BIT(9)
};

int mtk_dsi_init(u32 mode_flags, enum mipi_dsi_pixel_format format, u32 lanes,
		 const struct edid *edid);
void mtk_dsi_pin_drv_ctrl(void);

#endif
