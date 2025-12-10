/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_MEDIATEK_MT8173_DSI_H_
#define _SOC_MEDIATEK_MT8173_DSI_H_

#include <soc/dsi_common.h>
#include <soc/dsi_reg.h>
#include <types.h>

/* DSI features */
#define MTK_DSI_MIPI_RATIO_NUMERATOR 102
#define MTK_DSI_MIPI_RATIO_DENOMINATOR 100
#define MTK_DSI_DATA_RATE_MIN_MHZ 50
#define MTK_DSI_HAVE_SIZE_CON 0
#define PIXEL_STREAM_CUSTOM_HEADER 0

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

static struct lvds_tx1_regs *const lvds_tx0 = (void *)(MIPI_TX0_BASE + 0x800);
static struct lvds_tx1_regs *const lvds_tx1 = (void *)(MIPI_TX1_BASE + 0x800);

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

#endif /* _SOC_MEDIATEK_MT8173_DSI_H_ */
