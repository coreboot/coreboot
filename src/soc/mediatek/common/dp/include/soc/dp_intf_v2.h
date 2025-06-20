/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_COMMON_DP_DP_INTF_V2_H
#define SOC_MEDIATEK_COMMON_DP_DP_INTF_V2_H

#include <edid.h>
#include <soc/addressmap.h>
#include <types.h>

#define DVO_EN		0x00
#define EN		BIT(0)
#define DVO_FORCE_ON	BIT(4)
#define LINK_OFF	BIT(8)

#define MMSYS1_CG_CLR0		0x108
#define MMSYS1_CG_CLR1		0x118
#define DISP_EDPTX_PWR_CON	0x74

#define DVO_RET		0x04
#define SWRST		BIT(0)
#define SWRST_SEL	BIT(4)

#define DVO_INTEN		0x08
#define INT_VFP_START_EN	BIT(0)
#define INT_VSYNC_START_EN	BIT(1)
#define INT_VSYNC_END_EN	BIT(2)
#define INT_VDE_START_EN	BIT(3)
#define INT_VDE_END_EN		BIT(4)
#define INT_WR_INFOQ_REG_EN	BIT(5)
#define INT_TARGET_LINE0_EN	BIT(6)
#define INT_TARGET_LINE1_EN	BIT(7)
#define INT_TARGET_LINE2_EN	BIT(8)
#define INT_TARGET_LINE3_EN	BIT(9)
#define INT_WR_INFOQ_START_EN	BIT(10)
#define INT_WR_INFOQ_END_EN	BIT(11)
#define EXT_VSYNC_START_EN	BIT(12)
#define EXT_VSYNC_END_EN	BIT(13)
#define EXT_VDE_START_EN	BIT(14)
#define EXT_VDE_END_EN		BIT(15)
#define EXT_VBLANK_END_EN	BIT(16)
#define UNDERFLOW_EN		BIT(17)
#define INFOQ_ABORT_EN		BIT(18)

#define DVO_INTSTA		0x0C
#define INT_VFP_START_STA	BIT(0)
#define INT_VSYNC_START_STA	BIT(1)
#define INT_VSYNC_END_STA	BIT(2)
#define INT_VDE_START_STA	BIT(3)
#define INT_VDE_END_STA		BIT(4)
#define INT_WR_INFOQ_REG_STA	BIT(5)
#define INT_TARGET_LINE0_STA	BIT(6)
#define INT_TARGET_LINE1_STA	BIT(7)
#define INT_TARGET_LINE2_STA	BIT(8)
#define INT_TARGET_LINE3_STA	BIT(9)
#define INT_WR_INFOQ_START_STA	BIT(10)
#define INT_WR_INFOQ_END_STA	BIT(11)
#define EXT_VSYNC_START_STA	BIT(12)
#define EXT_VSYNC_END_STA	BIT(13)
#define EXT_VDE_START_STA	BIT(14)
#define EXT_VDE_END_STA		BIT(15)
#define EXT_VBLANK_END_STA	BIT(16)
#define INT_UNDERFLOW_STA	BIT(17)
#define INFOQ_ABORT_STA		BIT(18)

#define DVO_CON		0x10
#define INTL_EN		BIT(0)

#define DVO_OUTPUT_SET	0x18
#define OUT_NP_SEL	(0x3 <<	0)

#define BIT_SWAP	BIT(4)
#define CH_SWAP_MASK	(0x7 << 5)
#define SWAP_RGB	0x00
#define SWAP_GBR	0x01
#define SWAP_BRG	0x02
#define SWAP_RBG	0x03
#define SWAP_GRB	0x04
#define SWAP_BGR	0x05
#define PXL_SWAP	BIT(8)
#define R_MASK		BIT(12)
#define G_MASK		BIT(13)
#define B_MASK		BIT(14)
#define DE_MASK		BIT(16)
#define HS_MASK		BIT(17)
#define VS_MASK		BIT(18)
#define HS_INV		BIT(19)
#define VS_INV		BIT(20)

#define DVO_SRC_SIZE	0x20
#define SRC_HSIZE_SHIFT	0
#define SRC_HSIZE_MASK	(0xFFFF << 0)
#define SRC_VSIZE_SHIFT	16
#define SRC_VSIZE_MASK	(0xFFFF << 16)

#define DVO_PIC_SIZE	0x24
#define PIC_HSIZE_SHIFT	0
#define PIC_HSIZE_MASK	(0xFFFF << 0)
#define PIC_VSIZE_SHIFT	16
#define PIC_VSIZE_MASK	(0xFFFF << 16)

#define DVO_TGEN_H0	0x50
#define HFP_SHIFT	0
#define HFP_MASK	(0xFFFF << 0)
#define HSYNC_SHIFT	16
#define HSYNC_MASK	(0xFFFF << 16)

#define DVO_TGEN_H1	0x54
#define HSYNC2ACT_SHIFT	0
#define HSYNC2ACT_MASK	(0xFFFF << 0)
#define HACT_SHIFT	16
#define HACT_MASK	(0xFFFF << 16)

#define DVO_TGEN_V0	0x58
#define VFP_SHIFT	0
#define VFP_MASK	(0xFFFF << 0)
#define VSYNC_SHIFT	16
#define VSYNC_MASK	(0xFFFF << 16)

#define DVO_TGEN_V1	0x5C
#define VSYNC2ACT_SHIFT	0
#define VSYNC2ACT_MASK	(0xFFFF << 0)
#define VACT_SHIFT	16
#define VACT_MASK	(0xFFFF << 16)

#define DVO_TGEN_INFOQ_LATENCY		0x80
#define INFOQ_START_LATENCY		0
#define INFOQ_START_LATENCY_MASK	(0xFFFF << 0)
#define INFOQ_END_LATENCY		16
#define INFOQ_END_LATENCY_MASK		(0xFFFF << 16)

#define DVO_BUF_CON0			0x220
#define DISP_BUF_EN			BIT(0)
#define FIFO_UNDERFLOW_DONE_BLOCK	BIT(4)

#define DVO_TGEN_V_LAST_TRAILING_BLANK	0x6C
#define V_LAST_TRAILING_BLANK		0
#define V_LAST_TRAILING_BLANK_MASK	(0xFFFF << 0)

#define DVO_TGEN_OUTPUT_DELAY_LINE	0x7C
#define EXT_TG_DLY_LINE			0
#define EXT_TG_DLY_LINE_MASK		(0xFFFF << 0)

#define DVO_PATTERN_CTRL	0x100
#define PRE_PAT_EN		BIT(0)
#define PRE_PAT_SEL_MASK	(0x7 << 4)
#define COLOR_BAR		(0x4 << 4)
#define PRE_PAT_FORCE_ON	BIT(8)

#define DVO_PATTERN_COLOR	0x104
#define PAT_R			(0x3FF << 0)
#define PAT_G			(0x3FF << 10)
#define PAT_B			(0x3FF << 20)

#define DVO_SHADOW_CTRL	0x190
#define FORCE_COMMIT	BIT(0)
#define BYPASS_SHADOW	BIT(1)
#define READ_WRK_REG	BIT(2)

#define DVO_SIZE	0x18
#define DVO_TGEN_VWIDTH	0x28
#define DVO_TGEN_VPORCH	0x2C
#define DVO_TGEN_HPORCH	0x24
#define DVO_TGEN_HWIDTH	0x20

#define DVO_BUF_SODI_HIGHT	0x230
#define DVO_BUF_SODI_LOW	0x234

#define EDP_VERSION		2

enum mtk_dvo_golden_setting_level {
	MTK_DVO_FHD_60FPS_1920 = 0,
	MTK_DVO_FHD_60FPS_2180,
	MTK_DVO_FHD_60FPS_2400,
	MTK_DVO_FHD_60FPS_2520,
	MTK_DVO_FHD_90FPS,
	MTK_DVO_FHD_120FPS,
	MTK_DVO_WQHD_60FPS,
	MTK_DVO_WQHD_120FPS,
	MTK_DVO_8K_30FPS,
	MTK_DVO_GSL_MAX,
};

struct mtk_dvo_gs_info {
	u32 dvo_buf_sodi_high;
	u32 dvo_buf_sodi_low;
};

enum TVDPLL_CLK {
	TVDPLL_PLL = 0,
	TVDPLL_D16 = 1,
	TVDPLL_D8 = 2,
	TVDPLL_D4 = 3,
};

struct mtk_dvo {
	void *regs;
	void *regs_ck;
	void *regs_mm;
	enum mtk_dvo_golden_setting_level gs_level;
};

struct mtk_dvo_sync_param {
	u32 sync_width;
	u32 front_porch;
	u32 back_porch;
};

void dp_intf_config(const struct edid *edid);

#endif /* SOC_MEDIATEK_COMMON_DP_DP_INTF__V2_H */
