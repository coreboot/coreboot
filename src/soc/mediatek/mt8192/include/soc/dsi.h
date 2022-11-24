/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_DSI_H
#define SOC_MEDIATEK_MT8192_DSI_H

#include <soc/dsi_common.h>
#include <soc/dsi_register_v1.h>

/* DSI features */
#define MTK_DSI_MIPI_RATIO_NUMERATOR 100
#define MTK_DSI_MIPI_RATIO_DENOMINATOR 100
#define MTK_DSI_DATA_RATE_MIN_MHZ 125
#define MTK_DSI_HAVE_SIZE_CON 1
#define PIXEL_STREAM_CUSTOM_HEADER 0xb

/* MIPITX is SOC specific and cannot live in common. */

/* MIPITX_REG */
struct mipi_tx_regs {
	u32 reserved0[3];
	u32 lane_con;
	u32 reserved1[6];
	u32 pll_pwr;
	u32 pll_con0;
	u32 pll_con1;
	u32 pll_con2;
	u32 pll_con3;
	u32 pll_con4;
	u32 reserved2[65];
	u32 d2_sw_ctl_en;
	u32 reserved3[63];
	u32 d0_sw_ctl_en;
	u32 reserved4[56];
	u32 ck_ckmode_en;
	u32 reserved5[6];
	u32 ck_sw_ctl_en;
	u32 reserved6[63];
	u32 d1_sw_ctl_en;
	u32 reserved7[63];
	u32 d3_sw_ctl_en;
};

check_member(mipi_tx_regs, pll_con4, 0x3c);
check_member(mipi_tx_regs, d3_sw_ctl_en, 0x544);
static struct mipi_tx_regs *const mipi_tx = (void *)MIPITX_BASE;

/* Register values */
#define DSI_CK_CKMODE_EN	BIT(0)
#define DSI_SW_CTL_EN		BIT(0)
#define AD_DSI_PLL_SDM_PWR_ON	BIT(0)
#define AD_DSI_PLL_SDM_ISO_EN	BIT(1)

#define RG_DSI_PLL_EN		BIT(4)
#define RG_DSI_PLL_POSDIV	(0x7 << 8)

#endif
