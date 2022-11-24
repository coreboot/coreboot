/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DSI_REG_H_
#define _DSI_REG_H_

#include <soc/dsi_common.h>
#include <soc/dsi_register_v1.h>
#include <types.h>

/* DSI features */
#define MTK_DSI_MIPI_RATIO_NUMERATOR 102
#define MTK_DSI_MIPI_RATIO_DENOMINATOR 100
#define MTK_DSI_DATA_RATE_MIN_MHZ 50
#define MTK_DSI_HAVE_SIZE_CON 0
#define PIXEL_STREAM_CUSTOM_HEADER 0

/* MIPITX is SOC specific and cannot live in common. */

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

static struct mipi_tx_regs *const mipi_tx0 = (void *)MIPI_TX0_BASE;
static struct mipi_tx_regs *const mipi_tx1 = (void *)MIPI_TX0_BASE;

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

static struct lvds_tx1_regs *const lvds_tx1 = (void *)(MIPI_TX0_BASE + 0x800);
static struct lvds_tx1_regs *const lvds_tx2 = (void *)(MIPI_TX1_BASE + 0x800);

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

/* SOC specific functions */
void mtk_dsi_pin_drv_ctrl(void);

#endif
